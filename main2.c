/*
 * main.c
 *
 * Created: 2018-01-12 13:06:09
 * Author : Bengt Palmkvist
 */ 


#include <avr/io.h>
#include <avr/builtins.h>
#include <avr/interrupt.h>
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
//#include <stdlib.h>
//#include <stdio.h>

#include "General.h"
#include "SerialPort.h"
#include "Init.h"
#include "IO.h"
#include "Version.h"
#include "Monitor.h"
#include "Timer.h"
#include "Main.h"
#include "TachoCalc.h"
#include "Handler.h"

/*
BODLEVEL = 4V3
RSTDISBL = [ ]
DWEN = [ ]
SPIEN = [X]
WDTON = [ ]
EESAVE = [X]
BOOTSZ = 2048W_3800
BOOTRST = [ ]
CKDIV8 = [ ]
CKOUT = [ ]
SUT_CKSEL = INTRCOSC_8MHZ_6CK_14CK_65MS

EXTENDED = 0xFC (valid)
HIGH = 0xD1 (valid)
LOW = 0xE2 (valid)

*/    
/*
bool g_stop_tacho_func = false;
uint8_t g_reset_reason = 0;

int8_t callback_from_timer(uint8_t nbr)
{
    return 0;
}

int8_t callback_from_serial_port(uint8_t nbr)
{
    return 0;
}

int main2(void)
{
#ifdef DEBUG
    char str[100];
    uint32_t time_ref_deb;
#endif
    uint32_t time_ref;
    uint16_t rpm, tacho, red_LED, green_LED;
    uint8_t ad_value;
    g_reset_reason = MCUSR;
    init_HW();
    sp_init(SP_INIT_SOFT_FLOW_CTR);
    
    sp_puts_flash(FLASH_STR("\r\nTacho  Version: " ));
    sp_puts_flash(g_version);
    sp_puts_flash(FLASH_STR("\r\n" ));

    hndl_set_timer_callback((i8_func_1_i8) callback_from_timer, 10);
    hndl_set_serial_port_callback((i8_func_1_i8) callback_from_serial_port, 0);
    
    monitor_main();
  
}
*/
