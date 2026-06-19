/***************************************************************
*
* C code
*
* File: Timer.c
*
* Abstract: C-code program handling timer and clock functions.
*           Time counter are decremented in SysTimer interrupt 
*           routine.
*
* Change history :
* ----------------
* 2002-08-03 BP        Initial version.
*
***************************************************************/
#include <avr/interrupt.h>
#include <limits.h>
#include "General.h"
#include "Init.h"
#include "SysTimer.h"
#include "Timer.h"

//*************************************************************
#define TIMER_100_MS	(100/WD_INT_TIME_MS)

/**************************************************************
*
* Function: read_timer_value
*
* Abstract: Inhibits interrupt and reads timer value.
*
* Change history :
* ----------------
* 2002-08-03 BP        Initial version.
*
***************************************************************/
static uint32_t read_timer_value(void)
{
   unsigned long temp;
   cli(); //disable all interrupts
   temp = g_timer_value;
   sei(); //re-enable interrupts
   return(temp);
}

//static ULONG get_adjusted_timer_value(void)
//{
    //unsigned long temp = read_timer_value();
    ////if(ULONG_MAX)
	//return (temp * WD_INT_TIME_MS) / 100; // ~96 ms
    ////return ((temp*10)/(1000 / (100/WD_INT_TIME_MS))); // ~96 ms
//}



/***************************************************************
*
* Function: get_timer_reference_val
*
* Abstract: Returns value from time counter. For reference
*           use in main program.
*           Time counter are decremented every WD_INT_TIME_MS 
*           in timer interrupt routine
*
* Change history :
* ----------------
* 2002-08-03 BP        Initial version.
*
***************************************************************/
void tim_get_reference_val(uint32_t *ref_value)
{
   *ref_value = read_timer_value();
}

/***************************************************************
*                                                              *
* Function: get_time_diff_thenths                                            *
*                                                              *
* Abstract: Calculate difference in time between parameter     *
*           and time counter. Return value in 1/10  of sec.    *
*                                                              *
* Change history :                                             *
* ----------------                                             *
* 2002-08-03 BP        Initial version.
*                                                              *
***************************************************************/
uint32_t tim_get_diff_thenths(uint32_t ref_time)
{
   return((ref_time - read_timer_value()) * WD_INT_TIME_MS) / 100;
}

/***************************************************************
*                                                              *
* Function: time_wait                                            *
*                                                              *
* Abstract: Just wait for the time to pass.                    *
*           Desired value in 1/10  seconds.                 *
*                                                              *
* Change history :                                             *
* ----------------                                             *
* 2002-08-03 BP        Initial version.
*                                                              *
***************************************************************/
void tim_wait_thenths(uint16_t time_tenths)
{
uint32_t start_time = read_timer_value();

   while(tim_get_diff_thenths(start_time) < time_tenths); /* wait */
}

