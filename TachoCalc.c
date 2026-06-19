/*
 * TachoCalc.c
 *
 * Created: 2018-01-16 14:40:24
 *  Author: Bengt Palmkvist
 */ 
#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>

#include "General.h"
#include "Init.h"
#include "IO.h"
#include "TachoCalc.h"
#include "Eeprom.h"
#include "Debug.h"

#ifdef DEBUG
//#define DEBUG_MODULE
#endif

//////////////////////////////////////////////////////////////////////////
// 4 cyl 4 stroke
// 1000 rpm -> 2000 pulse/min, 33.333 pulse/sec
// 7000 rpm -> 14000 pulse/min 233.333 pulse/sec

//////////////////////////////////////////////////////////////////////////
// Local variables 
static volatile uint32_t m_breaker_count = 0;
static volatile uint32_t m_timer_count = 0;
static volatile uint32_t m_new_counter_value = 0;
static volatile uint8_t  m_TCNT0_value = 0;

static uint8_t m_nbr_of_cylinders = 4;
static uint8_t m_is_4_stroke = true;

static uint16_t m_tacho_calib_val[EE_NBR_OF_TACHO_CALIB + 1];
static const uint16_t m_tacho_default_calib_val[EE_NBR_OF_TACHO_CALIB + 1] = {1300, 2200, 4600, 7000, 8900, 11400, 13400, 15700, 16000, 16300, 16300, 0};
    
static uint16_t m_red_LED_rpm_start = 5000;    
static uint16_t m_red_LED_rpm_inc   = 100;

static uint8_t m_green_LED_min = 20;
static uint8_t m_green_LED_max = 200;
    
//////////////////////////////////////////////////////////////////////////
#ifdef DEBUG_MODULE
static void deb_print_long(char* str, uint32_t value)
{
    char deb_str[100];
    sprintf(deb_str, "%s: %lu", str, value);
    DEBUG_TACHO(deb_str);
}    

#define TAC_DEBUG(str, value)  deb_print_long(str, value)
#define TAC_DEBUG_STR(deb_str)  DEBUG_TACHO(deb_str)
#else

#define TAC_DEBUG(str, value) /* str  value */
#define TAC_DEBUG_STR(deb_str)  /* deb_str) */
#endif    

//////////////////////////////////////////////////////////////////////////

// Init calibration values for tachometer
void tacho_init()
{
    uint16_t value;
    for (uint8_t i=0; i<EE_NBR_OF_TACHO_CALIB; i++ )
    {
        uint16_t address = (i*2) + EE_TACHO_CALIB_START;
        value = ee_read_uint16(address);
        if(value > PWM_TACHO_MAX_VAL) {
            m_tacho_calib_val[i] = m_tacho_default_calib_val[i];
        }
        else {
            m_tacho_calib_val[i] = value;
        }
    }    
    m_tacho_calib_val[EE_NBR_OF_TACHO_CALIB] = 0;
    
    value = ee_read_uint16(EE_RED_LED_RPM_START);
    if(value < 20000) {
        m_red_LED_rpm_start = value;
    }
    value = ee_read_uint16(EE_RED_LED_RPM_INC);
    if(value < 1000) {
        m_red_LED_rpm_inc = value;
    }
    
    value = ee_read_byte(EE_GREEN_LED_START);
    if(value < 250) {
        m_green_LED_min = value;
    }
    value = ee_read_byte(EE_GREEN_LED_MAX);
    if(value < 250) {
        m_green_LED_max = value;
    }
}

uint16_t tac_get_calib_value(uint8_t nbr)
{
    if(nbr > EE_NBR_OF_TACHO_CALIB) return 0;
    return m_tacho_calib_val[nbr];
}

void tac_set_calib_value(uint8_t nbr, uint16_t value)
{
    if(nbr > EE_NBR_OF_TACHO_CALIB) return;
    if(value > PWM_TACHO_MAX_VAL) return;
    ee_write_uint16((nbr*2) + EE_TACHO_CALIB_START, value);
    m_tacho_calib_val[nbr] = value;
}

static uint32_t get_mean_value_of_3(uint32_t value)
{
    static uint32_t tmp1 = 0;
    static uint32_t tmp2 = 0;

    uint32_t ret_val = (value + tmp1 + tmp2) / 3;
    tmp2 = tmp1;
    tmp1 = value;
    return ret_val;
}

// Get approximated number of pointer break per minute
// If no pointer break since last time, return 0
static uint32_t tac_get_pulse_count_per_minute()
{
    static uint32_t old_timer_count = 0;
    static uint32_t old_breaker_count = 0;
    static uint8_t old_TCNT0_value = 0;
    
    cli(); //disable all interrupts
    uint32_t breaker_count = m_breaker_count;
    uint32_t timer_count = m_new_counter_value;
    uint32_t TCNT0_value = m_TCNT0_value;
    sei(); //re-enable interrupts
    
    if(m_breaker_count == old_breaker_count) return 0; // No pulse since last time, return 0
    
    // T0_OWFL_TIME_us is defined in C_init.h and set in C_init.c    
    uint32_t nbr_of_breaks = breaker_count - old_breaker_count;
    uint32_t elapsed_time = (uint32_t)(((uint32_t)(timer_count - old_timer_count) * (uint32_t)T0_OWFL_TIME_us)); // us    
    elapsed_time += ((TCNT0_value * T0_OWFL_TIME_us) / PWM_LED_MAX_VAL); // Add timer offset
    elapsed_time -= ((PWM_LED_MAX_VAL - old_TCNT0_value) * T0_OWFL_TIME_us) / PWM_LED_MAX_VAL; // Remove old timer offset     
     
    uint32_t mean_t_btw_breaks_us = elapsed_time / nbr_of_breaks;   
    uint32_t pulse_per_minute = (uint32_t)60000000 / mean_t_btw_breaks_us;
     
    TAC_DEBUG_STR("-----");    
    TAC_DEBUG("nbr_of_breaks", nbr_of_breaks);
    TAC_DEBUG("elapsed_time us", elapsed_time);
    TAC_DEBUG("mean_t_btw_breaks_us", mean_t_btw_breaks_us);
    TAC_DEBUG("pulse_per_minute", pulse_per_minute);
    
    old_timer_count = timer_count;
    old_breaker_count = breaker_count;
    old_TCNT0_value = TCNT0_value;

    return(pulse_per_minute);
}

uint16_t tac_get_RPM()
{
    if(m_is_4_stroke) {
        return tac_get_pulse_count_per_minute() / (m_nbr_of_cylinders / 2);
    }
    else { // two stroke
        return tac_get_pulse_count_per_minute() / m_nbr_of_cylinders;
    }        
}    

uint16_t tac_get_RPM_mean()
{
     if(m_is_4_stroke) {
         return get_mean_value_of_3(tac_get_pulse_count_per_minute()) / (m_nbr_of_cylinders / 2);
     }
     else { //two stroke
         return get_mean_value_of_3(tac_get_pulse_count_per_minute()) / (m_nbr_of_cylinders);
     }
}

void tac_set_nbr_of_cylinders(uint8_t nbr)
{
    m_nbr_of_cylinders = nbr;
}

uint8_t tac_get_nbr_of_cylinders()
{
    return m_nbr_of_cylinders;
}

uint16_t tac_calc_PWM_value(uint16_t rpm)
{
    uint32_t step, area;
    uint8_t pos_1, pos_2;
    
    area = 1000;
    
    if(rpm <500) {
        pos_1 = EE_NBR_OF_TACHO_CALIB; // = 0
        pos_2 = 0;
        step = 0;
        area = 500;
    }
    else if(rpm < 1000) {
        pos_1 = 0;
        pos_2 = 1;
        step = 500;
        area = 500;
    }
    else if(rpm < 2000) {
        pos_1 = 1;
        pos_2 = 2;
        step = 1000;
    }
    else if(rpm < 3000) {
        pos_1 = 2;
        pos_2 = 3;
        step = 2000;
    }
    else if(rpm < 4000) {
        pos_1 = 3;
        pos_2 = 4;
        step = 3000;
    }
    else if(rpm < 5000) {
        pos_1 = 4;
        pos_2 = 5;
        step = 4000;
    }
    else if(rpm < 6000) {
        pos_1 = 5;
        pos_2 = 6;
        step = 5000;
    }
    else if(rpm < 7000) {
        pos_1 = 6;
        pos_2 = 7;
        step = 6000;
    }
    else if(rpm < 8000) {
        pos_1 = 7;
        pos_2 = 8;
        step = 7000;
    }
    else if(rpm < 9000) {
        pos_1 = 8;
        pos_2 = 9;
        step = 8000;
    }
    else {
        pos_1 = 9;
        pos_2 = 10;
        step = 9000;
    }        
                         
    uint32_t offset = m_tacho_calib_val[pos_1];
    uint32_t span = m_tacho_calib_val[pos_2] - offset;
    TAC_DEBUG("offset", offset);
    TAC_DEBUG("span", span);
                                    
    uint32_t percent = (uint32_t)((uint32_t)(rpm - step) * (uint32_t)100) / (uint32_t)area; // Percent
    TAC_DEBUG("percent", percent);
    return ((uint32_t)(percent * span)/(uint32_t)100) + offset;
}

extern uint16_t tac_handle_red_LED(uint16_t rpm)
{
    uint32_t diff = 0;
    if(rpm > m_red_LED_rpm_start) {
        diff = rpm - m_red_LED_rpm_start;
        diff = (uint32_t)(diff * (uint32_t)m_red_LED_rpm_inc) / (uint32_t)1000;
        if(diff > PWM_LED_MAX_VAL) {
            diff = PWM_LED_MAX_VAL;
        }
    }
    return (uint16_t)diff;
}

extern uint16_t tac_handle_green_LED(uint16_t voltage)
{
    // 15 volt is about 100
    
    uint8_t span = m_green_LED_max - m_green_LED_min;
    uint16_t pwm = ((span * voltage) / 100) + m_green_LED_min;
    if(pwm > m_green_LED_max) {
        pwm = m_green_LED_max;
    }
    return pwm;
}

// Int from signal pin INT1/BRK_POINT_INT_PIN
ISR(INT1_vect)
{
    m_new_counter_value = m_timer_count;
    m_TCNT0_value = TCNT0; // us
    ++m_breaker_count;
};

// Timer 0 overflow interrupt every T0_OWFL_TIME_us
ISR(TIMER0_OVF_vect )
{
    ++m_timer_count;
};
