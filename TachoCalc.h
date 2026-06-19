/*
 * TachoCalc.h
 *
 * Created: 2018-01-16 14:40:38
 *  Author: Bengt Palmkvist
 */ 


#ifndef TACHOCALC_H_
#define TACHOCALC_H_

#include <stdint.h>

//////////////////////////////////////////////////////////////////////////
// Function prototypes

extern void tacho_init();
extern uint16_t tac_get_calib_value(uint8_t nbr);
extern void tac_set_calib_value(uint8_t nbr, uint16_t value);

extern uint16_t tac_get_RPM();
extern uint16_t tac_get_RPM_mean();
extern void tac_set_nbr_of_cylinders(uint8_t nbr);
extern uint8_t tac_get_nbr_of_cylinders();

extern uint16_t tac_calc_PWM_value(uint16_t rpm);
extern uint16_t tac_handle_red_LED(uint16_t rpm);
extern uint16_t tac_handle_green_LED(uint16_t voltage);

#endif /* TACHOCALC_H_ */