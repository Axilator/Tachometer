/*
 * Debug.c
 *
 * Created: 2018-01-16 14:40:38
 *  Author: Bengt Palmkvist
 */ 

#include "General.h"
#include "SerialPort.h"
#include "Debug.h"

#ifdef DEBUG
uint8_t g_logg_level = LOGG_ALL_ON;
#else
uint8_t g_logg_level = LOGG_INHIBIT;
#endif

void log_message(uint8_t level, char *str)
{
    if((g_logg_level & level) && (str[0] != EOS)){
        sp_puts_flash(FLASH_STR("Msg: "));
        sp_puts(str);
        sp_puts_flash(FLASH_STR("\r\n"));
    }
}
