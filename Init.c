/***************************************************************
*
* C code
*
*
* Abstract: C-code program for initialization of the
*           Atmel ATmega 328P
*
* Change history :
* ----------------
* 2015-01-030 BP        Initial version
*
***************************************************************/
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <avr/wdt.h>

#include "General.h"
#include "Init.h"
#include "IO.h"

// Port B direction (1 = output, 0 = input)
#define DDRB_DIRECTION      ((1<<PUMP_RELAY_PIN) | (1<<TACHO_METER_PIN) | (1<<NC_B2_PIN) | (1<<MOSI_PIN) | (1<<MISO_PIN) | (1<<SCK_PIN) | (1<<NC_B7_PIN))
// Init port value (pin == input, then 1 = pullup)
#define PORTB_STATE         (0<<BRK_POINT_COUNT_PIN)

// Port C direction (1 = output, 0 = input)
#define DDRC_DIRECTION      ((1<<NC_C2_PIN) | (1<<NC_C3_PIN) | (1<<NC_C4_PIN) | (1<<NC_C5_PIN) | (1<<NC_C6_PIN) )
// Init port value (pin == input, then 1 = pullup)
#define PORTC_STATE         (1<<SWITCH_2_PIN)

// Port D direction (1 = output, 0 = input)
#define DDRD_DIRECTION      ((1<<SERIAL_TX_PIN) | (1<<NC_D4_PIN) | (1<<LED_RED_PIN) | (1<<LED_GREEN_PIN) | (1<<NC_D7_PIN))
// Init port value (pin == input, then 1 = pullup)
#define PORTD_STATE         ((1<<SERIAL_RX_PIN) | (1<<SERIAL_TX_PIN) | (1<<SWITCH_1_PIN) | (1<<BRK_POINT_INT_PIN))

//#define ADCSRA_INIT = (1<<ADEN) | (1<<ADPS1) | (1<<ADPS0);
//#define AD_CONVERTER_VALUE  ADCH
//#define START_AD_CONVERTER  (ADCSRA_INIT | (1<<ADSC))
//#define AD_CONVERTER_IS_READY   (ADCSRA & (1<<ADIF))

//////////////////////////////////////////////////////////////////////////
// Local
static void init_timer_0();
static void init_timer_1();
static void init_WD_timer();
static void init_analog();

//////////////////////////////////////////////////////////////////////////
void init_HW()
{
    // stop errant interrupts until set up
    cli(); //disable all interrupts
	
	// Internal osc 8 MHz defined by fuses
    // Oscillator division factor: 1
    CLKPR = (1<<CLKPCE); // Enable write
    CLKPR = (0<<CLKPCE) | (0<<CLKPS3) | (0<<CLKPS2) | (0<<CLKPS1) | (0<<CLKPS0); //  Clock Prescale Register

    PORTB = PORTB_STATE;
    DDRB = DDRB_DIRECTION;

    PORTC = PORTC_STATE;
    DDRC = DDRC_DIRECTION;

    PORTD = PORTD_STATE;
    DDRD = DDRD_DIRECTION;

	// --- Pulse input from breaker points ---
	// External Interrupt(s) initialization
	// INT1: On
	// INT1 Mode: Falling Edge
	
	EICRA = (1<<ISC11) | (0<<ISC10);
	EIMSK = (1<<INT1) | (0<<INT0); // Enable int 1
	
	PCIFR = 0; // Clear int flags
    
    
    init_timer_1();
    
    init_timer_0();

    init_WD_timer();	
 
    init_analog();

    sei(); // Global enable interrupts

    //all peripherals are now initialized
}

static void init_timer_0()
{
    // --- PWM to red & green LED & Fast system timer---
    // Timer/Counter 0 initialization
    // Clock source: System Clock
    // Clock value: clk(8MHz)/1
    // Mode: Fast PWM 8-bit top=0x0FF
    // OC0A output: OC0A=PWM, /OC0A disc.
    // OC1B output: OC0B=PWM, /OC0B disc.
    // Timer Period: 32 us
    // Output Pulse(s):
    // OC0A Period: 32 us
    // OC0B Period: 32 us
    // Timer0 Overflow Interrupt: On
    // Compare A Match Interrupt: Off
    // Compare B Match Interrupt: Off

    // Green LED PD6 OC0A
    // Red LED PD5 OC0B

    // Clear OC0A/OC0B on Compare Match, set OC0A/OC0B at BOTTOM (non-inverting Fast PWM mode)
    TCCR0A=(1<<COM0A1) | (0<<COM0A0) | (1<<COM0B1) | (0<<COM0B0) | (1<<WGM01) | (1<<WGM00);
    TCCR0B=(0<<WGM02) | (0<<CS02) | (0<<CS01) | (1<<CS00); // clk(8MHz)/1 (From prescaler)  Define this (T0_OWFL_TIME_us) in Init.h!

    TCNT0=0x00; // Timer/Counter Register
    OCR0A=0x00; //  Output Compare Register A
    OCR0B=0x00; //  Output Compare Register B

    TIMSK0 = (1<<TOIE0); // Int on T0 overflow
}

static void init_timer_1()
{
	// --- PWM Output to Tachometer. Timer 1 ---
	// Timer/Counter 1 initialization
	// Clock source: System Clock
	// Clock value: Internal 8 MHz
	// Mode: 14 Fast PWM  top=0x03FFF
	// OC1A output: Non-Inverted PWM
	// OC1B output: Disconnected
	// Timer Period: 16,383 ms
	// Output Pulse(s):
	// OC1A Period: 16,383 ms Width: 0 us
	
	PRR  = 0; // No power reduction

    ICR1 = PWM_TACHO_MAX_VAL;
	TCCR1A = (1<<COM1A1) | (0<<COM1A0) | (0<<COM1B1) | (0<<COM1B0) | (1<<WGM11) | (0<<WGM10); // Clear OC1A on Compare Match
	TCCR1B = (1<<WGM12) | (1<<WGM13) | (0<<CS12) | (1<<CS11) | (0<<CS10); // clk(8MHz)/8 (From prescaler)
	TCCR1C = 0;

	TCNT1 = 0x0000; // Timer/Counter Register
	OCR1A = 0x0000; //  Output Compare Register A
	OCR1B = 0x0000;

	TIMSK1 = 0; // No int
}

static void init_WD_timer()
{
    // Watchdog Timer initialization
    // Watchdog Timer Prescaler: 16 ms
    // Watchdog timeout action: Interrupt
    wdt_reset();
    MCUSR = 0;
    WDTCSR = (1<<WDCE) | (0<<WDE);
    WDTCSR = (1<<WDIF) | (1<<WDIE) | (0<<WDP3) | (0<<WDCE) | (0<<WDE) | (0<<WDP2) | (0<<WDP1) | (0<<WDP0); // 16 ms
}

static void init_analog()
{
    // Analog to Digital converter
    // Analog input ADC0 (pin PC0) is used
    // Reference: AVcc with external capacitor at AREF pin
    // Left adjusted result. Read ADCH for 8-bit result.
    // ADC Prescaler Select: 011 (8)
    
    ADMUX = (1<<REFS0) | (1<<ADLAR); 
    ADCSRA = ADCSRA_INIT;
    ADCSRB = 0; 
    DIDR0 = 0x01; // Use pin PC0
    START_AD_CONVERTER();
}    
