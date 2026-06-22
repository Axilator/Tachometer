/*
 * SerialPort.h
 *
 * Created: 2018-01-15 12:28:38
 *  Author: Bengt Palmkvist
 */ 


#ifndef SERIALPORT_H_
#define SERIALPORT_H_

#include <avr/pgmspace.h>
#include "General.h"
#include <stdint.h>

#define  STX            0X02       /*  Ascii-definition  */
#define  ETX            0X03       /*  Ascii-definition  */
#define  LF             0X0A       /*  Ascii-definition  */
#define  CR             0X0D       /*     -"-            */
#define  BS             0X08       /*     -"-            */
#define  XON            0X11       /*     -"-            */
#define  XOFF           0X13       /*     -"-            */
#define  ESC            0X1B       /*     -"-            */
#define  EOS            0

#ifndef SP_ERR_BASE
#define SP_ERR_BASE  0x8000
#endif
#define  SP_ERR_CHKSUM    (SP_ERR_BASE | 1)
#define  SP_ERR_PARAM     (SP_ERR_BASE | 2)
#define  SP_ERR_READY     (SP_ERR_BASE | 3)
#define  SP_ERR_BREAK     (SP_ERR_BASE | 4)
#define  SP_ERR_TIMEOUT   (SP_ERR_BASE | 5)
#define  SP_ERR_FRAME     (SP_ERR_BASE | 6)
#define  SP_ERR_OVERFLOW  (SP_ERR_BASE | 7)
#define  SP_ERR_EMPTY     (SP_ERR_BASE | 8)
#define  SP_ERR_NOT_EMPTY (SP_ERR_BASE | 9)
#define  SP_ERR_NAK       (SP_ERR_BASE | 10)
#define  SP_ERR_UART      (SP_ERR_BASE | 11)
#define  SP_ERR_TO_APPLIC (SP_ERR_BASE | 12)
#define  SP_ERR_COMPARE   (SP_ERR_BASE | 14)

#ifndef  SP_NO_ERROR
#define  SP_NO_ERROR             0
#endif

#ifndef  SP_EMPTY
#define  SP_EMPTY                0
#endif

#define SP_INIT_TURN_OFF        0
#define SP_INIT_NO_FLOW_CTR     1  /* no flow control */
#define SP_INIT_SOFT_FLOW_CTR   2  /* use xon/xoff */

extern uint16_t sp_init(const uint8_t mode);
extern uint16_t sp_clear_rx_fifo(void);
extern uint16_t sp_clear_tx_fifo(void);
extern uint16_t sp_peek(uint8_t *ch);
extern uint16_t sp_get_nbr_in_buf();
extern uint16_t sp_putc(const uint8_t ch);
extern uint16_t sp_putc_no_wait(const uint8_t ch);
extern uint16_t sp_puts(char *ch);
extern uint16_t sp_getc(uint8_t *ch);
//extern USHORT SpGetcTimed(BYTE *l_chr , USHORT l_time);
extern uint16_t sp_get_status(void);
extern uint16_t sp_puts_flash(PGM_P ch);

extern void timer_int_UART();




#endif /* SERIALPORT_H_ */