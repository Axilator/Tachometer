/*****************************************************************************
*                                                                            *
*   --  General.h  --                                                          *
*                                                                            *
*   General include file with basic defines.                                 *
*                                                                            *
*                                                                            *
*  Change history :                                                          *
*  ----------------                                                          *
*  010809  BP    Initial version                                             *
*                                                                            *
*****************************************************************************/
// Target: ATmega328

#ifndef  C_BASIC_INCL
#define  C_BASIC_INCL

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <avr/interrupt.h>

/*--------------------------------------------------------------------------*/
/*  Defines                                                                 */
/*--------------------------------------------------------------------------*/

#define  STX            0X02       /*  Ascii-definition  */
#define  ETX            0X03       /*  Ascii-definition  */
#define  LF             0X0A       /*  Ascii-definition  */
#define  CR             0X0D       /*     -"-            */
#define  BS             0X08       /*     -"-            */
#define  XON            0X11       /*     -"-            */
#define  XOFF           0X13       /*     -"-            */
#define  EOS            0

#ifndef ERR_BASE
#define ERR_BASE  0x8000
#endif
#define  ERR_CHKSUM    (ERR_BASE | 1)
#define  ERR_PARAM     (ERR_BASE | 2)
#define  ERR_READY     (ERR_BASE | 3)
#define  ERR_BREAK     (ERR_BASE | 4)
#define  ERR_TIMEOUT   (ERR_BASE | 5)
#define  ERR_FRAME     (ERR_BASE | 6)
#define  ERR_OVERFLOW  (ERR_BASE | 7)
#define  ERR_EMPTY     (ERR_BASE | 8)
#define  ERR_NOT_EMPTY (ERR_BASE | 9)
#define  ERR_NAK       (ERR_BASE | 10)
#define  ERR_TO_APPLIC (ERR_BASE | 12)
#define  ERR_COMPARE   (ERR_BASE | 14)

/*--------------------------------------------------------------------------*/
/*  General defines                                                         */
/*--------------------------------------------------------------------------*/
#ifndef  NULL
#define  NULL              (void *) 0
#endif

#ifndef  EOS
#define  EOS                  0
#endif

#ifndef  LF
#define  LF                   10
#endif

#ifndef  CR
#define  CR                   13
#endif

#ifndef  OFF
#define  OFF                  0
#endif

#ifndef  ON
#define  ON                   1
#endif

#ifndef  LOW
#define  LOW                  0
#endif

#ifndef  HIGH
#define  HIGH                 1
#endif

#ifndef  NO_ERROR
#define  NO_ERROR             0
#endif

#ifndef  ERR_CODE
#define  ERR_CODE             1
#endif

#ifndef  ERR_CODE_NEG
#define  ERR_CODE_NEG         (-1)
#endif

#ifndef  OK
#define  OK                   0
#endif

#ifndef  EMPTY
#define  EMPTY                0
#endif

/*--------------------------------------------------------------------------*/
/*  Macro Definitions                                                       */
/*--------------------------------------------------------------------------*/
//#define FOREVER for(;;)


/*--------------------------------------------------------------------------*/
/*  Type definitions                                                        */
/*--------------------------------------------------------------------------*/

// define a function pointer
typedef void (*FUNC_PTR)();

/*--------------------------------------------------------------------------*/
/*  Atmel definitions                                                       */
/*--------------------------------------------------------------------------*/
#include <avr/pgmspace.h>

//#define FLASH_MEM  PROGMEM
#define FLASH_MEM  __attribute__((__progmem__))
#define FLASH_PNTR  PGM_P
#define FLASH_STR  PSTR
/*! \brief This macro will protect the following code from interrupts. */
#define AVR_ENTER_CRITICAL_REGION uint8_t volatile saved_sreg = SREG; \
                                     cli();
/*! \brief This macro must always be used in conjunction with AVR_ENTER_CRITICAL_REGION
 *        so the interrupts are enabled again.
 */
#define AVR_LEAVE_CRITICAL_REGION SREG = saved_sreg;

/*--------------------------------------------------------------------------*/
/*  Project specific includes                                               */
/*--------------------------------------------------------------------------*/
#include "Debug.h"



#endif