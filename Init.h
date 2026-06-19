/*--------------------------------------------------------------------------*/
/* Include file for init functions                                           */
/*--------------------------------------------------------------------------*/

//////////////////////////////////////////////////////////////////////////
// Defines

// CPU frequency
#define F_CPU 8000000

#define PWM_LED_MAX_VAL	255
#define PWM_TACHO_MAX_VAL	0x3fff

// ( 8MHz / 1 -> 8 MHz -> T= 0.125 us -> Overflow 0.125 * 256 = 32 us
// ( 8MHz / 8 -> 1 MHz -> T= 1 us -> Overflow 1 * 256 = 256 us
// ( 8MHz / 64 -> 125kHz -> T= 8 us -> Overflow 8 * 256 = 2.048 ms
#define T0_OWFL_TIME_us     32

#define WD_INT_TIME_MS      16

#define ADCSRA_INIT             ((1<<ADEN) | (1<<ADPS1) | (1<<ADPS0))
#define AD_CONVERTER_VALUE      ADCH
#define START_AD_CONVERTER()    (ADCSRA = (ADCSRA_INIT | (1<<ADSC)))
#define AD_CONVERTER_IS_READY   (ADCSRA & (1<<ADIF))

#define TURN_OFF_RED_LED()      TCCR0A &= ~((1<<COM0B1) | (1<<COM0B0));
#define TURN_OFF_GREEN_LED()    TCCR0A &= ~((1<<COM0A1) | (1<<COM0A0));
#define TURN_ON_RED_LED()       TCCR0A |= ((1<<COM0B1) | (0<<COM0B0));
#define TURN_ON_GREEN_LED()     TCCR0A |= ((1<<COM0A1) | (0<<COM0A0));

/*--------------------------------------------------------------------------*/
/*  Function prototypes                                                     */
/*--------------------------------------------------------------------------*/
extern void init_HW(void);
