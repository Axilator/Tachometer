/*
 * File:   handler.c
 * Author: Bengt Palmkvist
 *
 * Created on January 26, 2025, 9:05 PM
 */


#include "Handler.h"

#include "General.h"
#include "Timer.h"
#include "SerialPort.h"
#include "IO.h"
#include "Debug.h"
//#include <cstddef>
#include <stdint.h>


// i8_func_1_i8 m_func_sw = NULL;
// i8_func_1_i8 m_func_timer = NULL;

#define MAX_NBR_OF_SERIAL_P 1

typedef  struct {
    uint8_t port_number;
    i8_func_1_i8 serial_callback;
} SERIAL_PORT_STRUCT;

static SERIAL_PORT_STRUCT m_our_serial_ports[MAX_NBR_OF_SERIAL_P];

#define MAX_NBR_OF_TIMERS 5
typedef  struct {
    uint32_t timer_ref_value;
    uint32_t desired_time_value;
    i8_func_1_i8 timer_callback;
} TIMER_STRUCT;

static TIMER_STRUCT m_our_timers[MAX_NBR_OF_TIMERS];

#define MAX_NBR_OF_SWITCHES 2
typedef  struct {
    uint8_t switch_number;
    SWITCH_STAT_TYPE switch_status;
    i8_func_1_i8 switch_callback;
} SWITCH_STRUCT;

static SWITCH_STRUCT m_our_switch[MAX_NBR_OF_SWITCHES];


int8_t hndl_handler_init()
{
    uint8_t nbr;
    for (nbr = 0; nbr < MAX_NBR_OF_TIMERS; nbr++) {
        m_our_timers[nbr].desired_time_value = 0;
        m_our_timers[nbr].timer_callback = NULL;
        m_our_timers[nbr].timer_ref_value = 0;
    }

    for (nbr = 0; nbr < MAX_NBR_OF_SERIAL_P; nbr++) {
        m_our_serial_ports[nbr].port_number = 0;
        m_our_serial_ports[nbr].serial_callback = NULL;
    }

    for (nbr = 0; nbr < MAX_NBR_OF_SWITCHES; nbr++) {
        m_our_switch[nbr].switch_number = 0;
        m_our_switch[nbr].switch_status = 0;
        m_our_switch[nbr].switch_callback = NULL;
    }

    return 0;
}

// Handle timer callback
int8_t hndl_set_timer_callback(i8_func_1_i8 func, uint16_t hundredth_sec)
{
    uint8_t nbr;
    for (nbr = 0; nbr < MAX_NBR_OF_TIMERS; nbr++) {
        if (m_our_timers[nbr].desired_time_value == 0) { // Not in use?
            m_our_timers[nbr].desired_time_value = hundredth_sec;
            m_our_timers[nbr].timer_callback = func;
            tim_get_reference_val(&(m_our_timers[nbr].timer_ref_value));
            return nbr;
        }
    }
    return ERR_CODE_NEG;
}

// Handle serial port
int8_t hndl_set_serial_port_callback(i8_func_1_i8 func, uint8_t port_nbr)
{
    uint8_t nbr;
    for (nbr = 0; nbr < MAX_NBR_OF_SERIAL_P; nbr++) {
        if (m_our_serial_ports[nbr].serial_callback == NULL) { // Not in use?
            m_our_serial_ports[nbr].port_number = port_nbr;
            m_our_serial_ports[nbr].serial_callback = func;
            return nbr;
        }
    }
    return ERR_CODE_NEG;
}

// Handle keypress
int8_t hndl_set_keypress_callback(i8_func_1_i8 func, uint8_t sw_nbr)
{
    uint8_t nbr;
    for (nbr = 0; nbr < MAX_NBR_OF_SWITCHES; nbr++) {
        if (m_our_switch[nbr].switch_callback == NULL) { // Not in use?
            m_our_switch[nbr].switch_number = sw_nbr;
            m_our_switch[nbr].switch_callback = func;
            return nbr;
        }
    }
    return ERR_CODE_NEG;
}

int8_t hndl_handler_main()
{
    uint8_t nbr;
    // Check timers
    for (nbr=0; nbr<MAX_NBR_OF_TIMERS; nbr++) {
        if (m_our_timers[nbr].desired_time_value != 0) { // In use??
            if(tim_get_diff_thenths(m_our_timers[nbr].timer_ref_value) >= m_our_timers[nbr].desired_time_value) {
                // Timer expired
                if (m_our_timers[nbr].timer_callback(nbr)) { // call function
                    // Returned TRUE so start timer again
                    tim_get_reference_val(&(m_our_timers[nbr].timer_ref_value)); 
                }
                else { // Stop timer and reset it
                    m_our_timers[nbr].desired_time_value = 0;
                    m_our_timers[nbr].timer_callback = NULL;
                    m_our_timers[nbr].timer_ref_value = 0;
                }
            }
        }
    }

    // Check keypress port
    for (nbr=0; nbr<MAX_NBR_OF_SWITCHES; nbr++) {
        if (m_our_switch[nbr].switch_callback != NULL) { // In use??
            SWITCH_STAT_TYPE switch_status = io_get_switch_status();
            if (m_our_switch[nbr].switch_status != switch_status) {
                if(m_our_switch[nbr].switch_callback(m_our_switch[nbr].switch_status)) { // call function
                }
                m_our_switch[nbr].switch_status = switch_status;
             
            }
        }
    }

     // Check serial port
    for (nbr=0; nbr<MAX_NBR_OF_SERIAL_P; nbr++) {
        if (m_our_serial_ports[nbr].serial_callback != NULL) { // In use??
            uint8_t ch;
            if (sp_peek(&ch) == SP_NO_ERROR) {
                if(m_our_serial_ports[nbr].serial_callback(ch)) { // call function
                    sp_getc(&ch); // Returned TRUE so empty last character
                }
                else {
                    sp_getc(&ch); // Get char so far..
                }
            }
        }
    }

    return 0;
}