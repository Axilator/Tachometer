/*
 * Debug.h
 * Use it like this: DEBUG_TRACE("ABORT BACKUP");
 * Enable it by the #define in Def.h 
 * Created: 2016-11-07 11:21:08
 *  Author: Bengt Palmkvist
 */


#ifndef DEBUG_H_
#define DEBUG_H_

#include <stdint.h>

/***********************************************************************
    Debug defines
***********************************************************************/
#define  LOGG_INHIBIT   00
#define  LOGG_TRACE     0x01
#define  LOGG_REPORT    0x02
#define  LOGG_COMMENT   0x04
#define  LOGG_ERROR     0x08
#define  LOGG_TACHO     0x10
#define  LOGG_TYPE_6    0x20
#define  LOGG_TYPE_7    0x40
#define  LOGG_TYPE_8    0x80
#define  LOGG_ALL_ON    0xff

#define DEB_STR_SIZE 500


#ifdef DEBUG

//#define DEBUG_TRACE(x) DisplayLogData(x)
#   define DEBUG_TRACE(x) log_message(LOGG_TRACE, x)
#   define DEBUG_ERROR(x) log_message(LOGG_ERROR, x)
#   define DEBUG_TACHO(x) log_message(LOGG_TACHO, x)

#else // Just dummy
#   define DEBUG_TRACE(x) /* x */
#   define DEBUG_ERROR(x) /* x */
#   define DEBUG_TACHO(x) /* x */
#endif

extern uint8_t g_logg_level;

extern void log_message(uint8_t level, char *str);

#endif /* DEBUG_H_ */