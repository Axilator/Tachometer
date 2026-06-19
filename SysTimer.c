/***************************************************************
*
* C code
*
*
* Abstract: C-code program for System Timer
*           Atmel ATmega328
*
* Change history :
* ----------------
* 2017-12-14 BP        Initial version
*
***************************************************************/
#include <avr/io.h>
#include <avr/interrupt.h>
#include <limits.h>
#include "General.h"
#include "Init.h"
#include "SysTimer.h"
#include "IO.h"
#include "SerialPort.h"

//////////////////////////////////////////////////////////////////////////
// Global
uint32_t g_timer_value=0xffffffff;


//////////////////////////////////////////////////////////////////////////
// Approximated time constants
#define TIM_100_MS	(100/WD_INT_TIME_MS)
#define TIM_1000_MS	(1000/WD_INT_TIME_MS)


/***********************************************************************
  System timer interrupt. Watch Dog TImer
***********************************************************************/
ISR(WDT_vect)
{
    static uint8_t system_timer_busy = false;
    static uint8_t tim_tenth_sec = TIM_100_MS;
    static uint8_t time_one_sec = TIM_1000_MS;

    // ----- System timer  -----
    --g_timer_value;
    
    if (system_timer_busy == false) // Avoid reentrance
    {
        system_timer_busy = true;	

        // ----- Handle timers -----
	    // ~ 1/10 sec
	    if(!tim_tenth_sec--) {
		    tim_tenth_sec = TIM_100_MS;
            timer_int_UART();
	    }	

	    // ~1 second
	    if(!time_one_sec--) {
		    time_one_sec = TIM_1000_MS;
	    }	

        // Read the switches
        io_handle_read_switch();
        
        system_timer_busy = false;
    }		
};

//////////////////////////////////////////////////////////////////////////
ISR(BADISR_vect)
{
    g_timer_value=0xffffffff;
}




