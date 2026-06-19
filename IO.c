/*
 * IO.c
 *
 * Created: 2015-02-24 17:39:09
 *  Author: Bengt Palmkvist
 */ 

#include <avr/io.h>

#include "IO.h"
#include "Init.h"

#define SWITCH_1_IS_PRESSED(port) (port & (1<<SWITCH_1_PIN))
#define SWITCH_2_IS_PRESSED(port) (port & (1<<SWITCH_2_PIN))

//////////////////////////////////////////////////////////////////////////
// Local variables
static SWITCH_STAT_TYPE g_switch_status = 0;

////////////////////////////////////////////////////////////////////////
void io_set_duty_PWM_tacho(uint16_t duty_cycle)
{
    if(duty_cycle > PWM_TACHO_MAX_VAL) duty_cycle = PWM_TACHO_MAX_VAL;
    if(OCR1A != duty_cycle) 	OCR1A = duty_cycle;
}

void io_set_duty_PWM_LED_green(uint8_t duty_cycle)
{
	if(OCR0A != duty_cycle) 	OCR0A = duty_cycle;
}

void io_set_duty_PWM_LED_red(uint8_t duty_cycle)
{
	if(OCR0B != duty_cycle) 	OCR0B = duty_cycle;
}

uint16_t io_get_duty_PWM_tacho()
{
    return OCR1A;
}

uint8_t io_get_duty_PWM_LED_green()
{
    return OCR0A;
}

uint8_t io_get_duty_PWM_LED_red()
{
    return OCR0B;
}

void io_set_petrol_pump(bool onOff)
{
	if(onOff) {
		PUMP_RELAY_ON;
	}
	else {
		PUMP_RELAY_OFF;
	}
}

static uint8_t read_switch()
{
    return ((~(SWITCH_1_PORT)) & (1<<SWITCH_1_PIN)) | ((~(SWITCH_2_PORT)) & (1<<SWITCH_2_PIN));
}

bool io_switch_is_pressed()
{
	return read_switch() != 0;
}

SWITCH_STAT_TYPE io_get_switch_status()
{
	uint8_t retVal = g_switch_status;
	g_switch_status = 0;
	return retVal;
}

// Start A/D conversion, wait, and read value
uint8_t io_read_AD_value()
{
    START_AD_CONVERTER();
    while(!AD_CONVERTER_IS_READY);
    return AD_CONVERTER_VALUE;    
}

/////////////////////////

// Read previous A/D value and start a new conversion to be read at next call
uint8_t io_read_AD_value_continue()
{
    uint8_t value;
    while(!AD_CONVERTER_IS_READY);
    value = AD_CONVERTER_VALUE;
    START_AD_CONVERTER();
    return value;
}


//////////////////////////////////////////////////////////////////////////
// Approximated time constants
#define TIM_100_MS	(100/WD_INT_TIME_MS)
#define TIM_1000_MS	(1000/WD_INT_TIME_MS)

// Local
volatile static uint8_t m_sw_1_was_pressed = false;
volatile static uint8_t m_sw_2_was_pressed = false;
volatile static char m_long_press_timer_1 = 0;
volatile static char m_long_press_timer_2 = 0;

static SWITCH_STAT_TYPE switch_is_just_pressed(uint8_t sw_pressed)
{
    SWITCH_STAT_TYPE ret_val = 0;
    if(SWITCH_1_IS_PRESSED(sw_pressed)) {
        if(!m_sw_1_was_pressed) { //Debounce function
            m_sw_1_was_pressed = true;
            m_long_press_timer_1 = TIM_1000_MS + 1;
        }
        else {
            ret_val = SW_1_SHORT_PRESS;
        }
    }
    if(SWITCH_2_IS_PRESSED(sw_pressed)) {
        if(!m_sw_2_was_pressed) { //Debounce function
            m_sw_2_was_pressed = true;
            m_long_press_timer_2 = TIM_1000_MS + 1;
        }
        else {
            ret_val |= SW_2_SHORT_PRESS;
        }
    }
    return ret_val;
}

static SWITCH_STAT_TYPE switch_is_just_released(uint8_t sw_pressed)
{
    SWITCH_STAT_TYPE retVal = 0;
    if(m_sw_1_was_pressed && (!SWITCH_1_IS_PRESSED(sw_pressed))) {
        m_sw_1_was_pressed = false;
        retVal = SW_1_RELEASED;
    }
    if(m_sw_2_was_pressed && (!SWITCH_2_IS_PRESSED(sw_pressed))) {
        m_sw_2_was_pressed = false;
        retVal |= SW_2_RELEASED;
    }
    return retVal;
}
//////////////////////////////////////////////////////////////////////////
// Called from System timer
extern void io_handle_read_switch()
{
    uint8_t sw_pressed;
    SWITCH_STAT_TYPE sw_status;
    
    // ----- Handle switches -----
    sw_pressed =  read_switch();
    if(sw_pressed) {
        sw_status = switch_is_just_pressed(sw_pressed);
        // Check for long press...
        if(sw_status & SW_1_SHORT_PRESS) {
            if(m_long_press_timer_1 == 1) {
                m_long_press_timer_1 = 0;
                g_switch_status |= SW_1_LONG_PRESS;
                DEBUG_TRACE("SW_1_LONG_PRESS");
            }
            else if(m_long_press_timer_1 > 1) {
                --m_long_press_timer_1;
            }
        }
        if(sw_status & SW_2_SHORT_PRESS) {
            if(m_long_press_timer_2 == 1) {
                m_long_press_timer_2 = 0;
                g_switch_status |= SW_2_LONG_PRESS;
                DEBUG_TRACE("SW_2_LONG_PRESS");
            }
            else if(m_long_press_timer_2 > 1) {
                --m_long_press_timer_2;
            }
        }
    }
    // Short switch press is detected on release...
    sw_status |= switch_is_just_released(sw_pressed);
    if((sw_status & SW_1_RELEASED) && (m_long_press_timer_1 > 1)) {
        m_long_press_timer_1 = 0;
        g_switch_status |= SW_1_SHORT_PRESS;
        DEBUG_TRACE("SW_1_SHORT_PRESS");
    }
    if((sw_status & SW_2_RELEASED) && (m_long_press_timer_2 > 1)) {
        m_long_press_timer_2 = 0;
        g_switch_status |= SW_2_SHORT_PRESS;
        DEBUG_TRACE("SW_2_SHORT_PRESS");
    }

}

void io_set_switch_status_test(SWITCH_STAT_TYPE sw)
{
    g_switch_status = sw;
}

