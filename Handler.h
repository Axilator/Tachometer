
/* 
 * File:   
 * Author: 
 * Comments:
 * Revision history: 
 */

 
#ifndef HANDLER_H
#define	HANDLER_H

#include "General.h"

typedef int8_t (*i8_func_1_i8)(uint8_t);

extern int8_t hndl_set_timer_callback(i8_func_1_i8 func, uint16_t hundredth_sec);
extern int8_t hndl_set_serial_port_callback(i8_func_1_i8 func, uint8_t port_nbr);

#endif /* SHANDLER_H */
