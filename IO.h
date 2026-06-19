/*
 * IO.h
 *
 * Created: 2015-02-24 17:39:40
 *  Author: Bengt Palmkvist
 */ 


#ifndef C_IO_H_
#define C_IO_H_

#include "General.h"

//////////////////////////////////////////////////////////////////////////
// Defines
typedef enum SWITCH_STATUS {
    SW_1_SHORT_PRESS = 0x01,
    SW_1_LONG_PRESS  = 0x02,
    SW_2_SHORT_PRESS = 0x04,
    SW_2_LONG_PRESS  = 0x08,
    SW_1_RELEASED    = 0x10,
    SW_2_RELEASED    = 0x20,
} SWITCH_STAT_TYPE;

// Port B defines
#define PUMP_RELAY_PIN		PORTB0
#define TACHO_METER_PIN		PORTB1
#define NC_B2_PIN		    PORTB2
#define MOSI_PIN		    PORTB3
#define MISO_PIN		    PORTB4
#define SCK_PIN		        PORTB5
#define BRK_POINT_COUNT_PIN	PORTB6
#define NC_B7_PIN		    PORTB7


// Port C defines
#define ANALOG_IN_PIN		PORTC0
#define SWITCH_2_PIN		PORTC1
#define NC_C2_PIN		    PORTC2
#define NC_C3_PIN		    PORTC3
#define NC_C4_PIN		    PORTC4
#define NC_C5_PIN		    PORTC5
#define NC_C6_PIN		    PORTC6
//#define NC_C7_PIN		    PORTC7


// Port D defines
#define SERIAL_RX_PIN		PORTD0
#define SERIAL_TX_PIN		PORTD1
#define SWITCH_1_PIN		PORTD2
#define BRK_POINT_INT_PIN	PORTD3
#define NC_D4_PIN		    PORTD4
#define LED_RED_PIN			PORTD5
#define LED_GREEN_PIN		PORTD6
#define NC_D7_PIN		    PORTD7
    
#define SWITCH_1_PORT		PIND    
#define SWITCH_2_PORT		PINC

#define PUMP_RELAY_ON   (PORTB |= (1<<PUMP_RELAY_PIN))
#define PUMP_RELAY_OFF  (PORTB &= ~(1<<PUMP_RELAY_PIN))


//////////////////////////////////////////////////////////////////////////
// Function prototypes 
extern void io_set_duty_PWM_tacho(uint16_t dutyCycle);
extern void io_set_duty_PWM_LED_green(uint8_t dutyCycle);
extern void io_set_duty_PWM_LED_red(uint8_t dutyCycle);

extern uint16_t io_get_duty_PWM_tacho();
extern uint8_t io_get_duty_PWM_LED_green();
extern uint8_t io_get_duty_PWM_LED_red();

extern void io_set_petrol_pump(bool onOff);
extern uint8_t io_read_AD_value();

extern uint8_t io_read_AD_value_continue();
extern bool io_switch_is_pressed();
extern SWITCH_STAT_TYPE io_get_switch_status();

extern void io_handle_read_switch(); // Called from System timer
extern void io_set_switch_status_test(SWITCH_STAT_TYPE sw);

#endif /* C_IO_H_ */