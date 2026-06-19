/*
 * Timer.h
 * Include file for timer functions
 * Created: 2018-01-18 
 *  Author: Bengt Palmkvist
 */ 


#ifndef TIMER_H_
#define TIMER_H_

#include <stdint.h>

//////////////////////////////////////////////////////////////////////////
// Definitions
#define HUNDRED_MS	1
#define TWO_SECONDS	(HUNDRED_MS * 20)

//////////////////////////////////////////////////////////////////////////
// Function prototypes
extern void tim_get_reference_val (uint32_t *ref_value);
extern uint32_t tim_get_diff_thenths (uint32_t ref_time);
extern void tim_wait_thenths(uint16_t time_tenths);

#endif /* TIMER_H_ */

