/*
 * SerialPort.c
 *
 * Created: 2018-01-15 12:28:24
 *  Author: Bengt Palmkvist
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include "SerialPort.h"
#include "Init.h"
#include "Timer.h"

/***********************************************************************
    Serial port SETUP AVR
***********************************************************************/
#define BAUD 19200
//#define BAUD 115200
#define MY_UBRR_REG (uint16_t)((F_CPU/16/BAUD)-1) /* F_CPU / (16 * BAUD) -1 */

#define MY_UCSR0B_REG  (_BV(RXCIE0) | _BV(TXCIE0)  | _BV(RXEN0)  | _BV(TXEN0))

/* Async. mode, 8 bits, no parity, 1 stop */
#define UART0_NO_PARITY  (_BV(UCSZ01) | _BV(UCSZ00))
#define MY_UCSR0C_REG  UART0_NO_PARITY




/*--------------------------------------------------------------------------*/
/*  Fifo size                                                               */
/*--------------------------------------------------------------------------*/
#define  TX_FIFO_0_SIZE  500
#define  RX_FIFO_0_SIZE  50

#define  MIN_BUF_SIZE_0  (RX_FIFO_0_SIZE - (RX_FIFO_0_SIZE/3))  /* send XON */
#define  BUFFER_OH_0     (RX_FIFO_0_SIZE-10)   /* Send XOFF  */

/***********************************************************************
    Function prototypes
***********************************************************************/
static void UART_0_transmit();

/***********************************************************************
    Local variables
***********************************************************************/
volatile static uint16_t   
rx_counter_0, tx_counter_0;

volatile static uint8_t     
*rx_emptyp_0, *rx_fillp_0,
*tx_emptyp_0, *tx_fillp_0;

volatile static uint8_t     
rx_fifo_0[RX_FIFO_0_SIZE],
tx_fifo_0[TX_FIFO_0_SIZE],
rx_stat_0,
sp_flow_mode_0 ;

volatile static bool     rx_xoff_0, xoff_sent_0;

//**************************************************************         
// local help functions
static void sp_putc_0(uint8_t ch)
{
   *tx_fillp_0++ = ch;
   if (tx_fillp_0 > &tx_fifo_0[TX_FIFO_0_SIZE-1]) { // fillpointer at end ?
      tx_fillp_0 = tx_fifo_0; // wrap fillpointer
   }
   AVR_ENTER_CRITICAL_REGION; //disable all interrupts
   tx_counter_0++;  //
   AVR_LEAVE_CRITICAL_REGION;// Global enable interrupts
}   

static uint8_t sp_getc_0(void)
{
   uint8_t  ch = *rx_emptyp_0++;
   if (rx_emptyp_0 > &rx_fifo_0[RX_FIFO_0_SIZE-1]) { // pointer at end ?
      rx_emptyp_0 = rx_fifo_0; // wrap fillpointer
   }
   AVR_ENTER_CRITICAL_REGION; //disable all interrupts
   rx_counter_0--;
   AVR_LEAVE_CRITICAL_REGION;// Global enable interrupts
   return ch;
}

/***************************************************************
*                                                              *
* Function: sp_init                                             *
*                                                              *
* Abstract: Initialize fifos. Interrupt is disabled.           *
*                                                              *
* Change history :                                             *
* ----------------                                             *
* 2018-01-15 BP    Initial version.                            *
*                                                              *
***************************************************************/
uint16_t sp_init(const uint8_t mode)
{
   if(mode == SP_INIT_TURN_OFF) {
       UCSR0A = UCSR0B  = 0x00;
       return SP_NO_ERROR;
   }
   
   cli(); //disable all interrupts
   sp_flow_mode_0 = mode;
   rx_xoff_0 = false;
   rx_counter_0 = 0;
   rx_emptyp_0 = rx_fillp_0 = rx_fifo_0;
   tx_counter_0 = 0;
   tx_emptyp_0 = tx_fillp_0 = tx_fifo_0;
   xoff_sent_0 = false;

/* The bit 3 (TXEN) in UCSRnB enables the UART transmitter when 
   set (one). When this bit is cleared (zero), the PD1 pin can 
   be used for general I/O. When TXEN is set, the UART 
   Transmitter will be connected to PD1, which is forced to 
   be an output pin regardless of the setting of the DDD1 
   bit in DDRD. 
   
   When bit 4 (RXEN) in the UCSRB register is cleared (zero), the 
   receiver is disabled. This means that the PD0 pin can be used
   as a general I/O pin. When RXEN is set, the UART Receiver will 
   be connected to PD0, which is forced to be an input pin
   regardless of the setting of the DDD0 bit in DDRD. When PD0 
   is forced to input by the UART, the PORTD0 bit can still be
   used to control the pull-up resistor on the pin. */
   
   UCSR0A = UCSR0B  = 0x00; //disable while setting baud rate

   UBRR0L = (uint8_t) MY_UBRR_REG; //set baud rate
   UBRR0H = (uint8_t) (MY_UBRR_REG >> 8);

   UCSR0A = _BV(TXC0);           /* Flush tx complete flag and error flags */
   UCSR0C = MY_UCSR0C_REG;
   UCSR0B = MY_UCSR0B_REG;

   sei(); // Global enable interrupts
   return(SP_NO_ERROR);
}

/***************************************************************
*                                                              *
* Function: sp_clear_rx_fifo                                      *
*                                                              *
* Abstract: Resets receive counters and pointers.              *
*                                                              *
* Change history :                                             *
* ----------------                                             *
* 2018-01-15 BP        Initial version.                        *
*                                                              *
***************************************************************/
uint16_t sp_clear_rx_fifo(void)
{
    rx_counter_0 = 0;
    rx_emptyp_0 = rx_fillp_0 = rx_fifo_0;
    return SP_NO_ERROR;
}

/***************************************************************
*                                                              *
* Function: sp_clear_tx_fifo                                      *
*                                                              *
* Abstract: Resets transmit counters and pointers.             *
*                                                              *
* Change history :                                             *
* ----------------                                             *
* 2018-01-15 BP        Initial version.                        *
*                                                              *
***************************************************************/
uint16_t sp_clear_tx_fifo(void)
{
    tx_counter_0 = 0;
    tx_emptyp_0 = tx_fillp_0 = tx_fifo_0;
    return SP_NO_ERROR;
}

/***************************************************************
*                                                              *
* Function: sp_peek                                             *
*                                                              *
* Abstract: Reads next character without affecting buffer      *
*                                                              *
* Change history :                                             *
* ----------------                                             *
* 2018-01-15 BP        Initial version.                        *
*                                                              *
***************************************************************/
uint16_t sp_peek(uint8_t *ch)
{
    if(rx_counter_0) *ch = *rx_emptyp_0;
    else  return SP_ERR_EMPTY;
    return SP_NO_ERROR;
}

/***************************************************************
*                                                              *
* Function: sp_get_nbr_in_buf                                  *
*                                                              *
* Abstract: Reads the number of characters in buffer           *
*                                                              *
* Change history :                                             *
* ----------------                                             *
* 2026-05-04 BP        Initial version.                        *
*                                                              *
***************************************************************/
uint16_t sp_get_nbr_in_buf()
{
    return rx_counter_0;
}

/***************************************************************
*                                                              *
* Function: sp_putc                                             *
*                                                              *
* Abstract: Puts a character in transmit fifo.                 *
*                                                              *
* Change history :                                             *
* ----------------                                             *
* 2018-01-15 BP        Initial version.                        *
***************************************************************/
uint16_t sp_putc(const uint8_t ch)
{
retry:    
    if(tx_counter_0 < TX_FIFO_0_SIZE) {
        sp_putc_0(ch);
    }
    else {
        tim_wait_thenths(1);
        goto retry;
    }
    return SP_NO_ERROR;
}

uint16_t sp_putc_no_wait(const uint8_t ch)
{
    if(tx_counter_0 < TX_FIFO_0_SIZE) {
        sp_putc_0(ch);
    }
    else {
        return SP_ERR_OVERFLOW;
    }
    return SP_NO_ERROR;
}

/***************************************************************
*                                                              *
* Function: sp_puts                                             *
*                                                              *
* Abstract: Puts a zero terminated string in transmit fifo.    *
*                                                              *
* Change history :                                             *
* ----------------                                             *
* 2018-01-15 BP        Initial version.                        *
***************************************************************/
uint16_t sp_puts(char *ch)
{
    while(*ch != EOS) {
       sp_putc(*ch);
       ch++;
    }
    return SP_NO_ERROR;
}

// String is located in flash
uint16_t sp_puts_flash(PGM_P ch)
{
    char tmp = pgm_read_byte(ch);
    while(tmp != EOS) {
        sp_putc(tmp);
        ch++;
        tmp = pgm_read_byte(ch);
    }
    return SP_NO_ERROR;
}

/***************************************************************
*                                                              *
* Function: sp_getc                                             *
*                                                              *
* Abstract: Reads a character from receive fifo's.             *
*                                                              *
* Change history :                                             *
* ----------------                                             *
* 2018-01-15 BP        Initial version.                        *
***************************************************************/
uint16_t sp_getc(uint8_t *ch)
{
    if(rx_counter_0) {
        *ch = sp_getc_0();
        if(sp_flow_mode_0 != SP_INIT_NO_FLOW_CTR) {
            if(xoff_sent_0) {    /*  If XOFF have been sent  */
                if(rx_counter_0<MIN_BUF_SIZE_0) {
                    xoff_sent_0 = false;
                    rx_xoff_0=false;
                    if(sp_flow_mode_0 == SP_INIT_SOFT_FLOW_CTR ){
                        sp_putc(XON);    /* Send XON */
                    }
     
                }
            }
        }
    }
    else {
        return SP_ERR_EMPTY;
    }

    return SP_NO_ERROR;
}

/***************************************************************
*                                                              *
* Function: sp_get_status                                       *
*                                                              *
* Abstract: Check for missed character
*                                                              *
* Change history :                                             *
* ----------------                                             *
* 2018-01-15 BP        Initial version.                        *
***************************************************************/
uint16_t sp_get_status(void)
{
    if(rx_stat_0) {
        rx_stat_0=0;
        return SP_ERR_OVERFLOW;
    }
    if(UCSR0A & (_BV(FE0) & _BV(DOR0) & _BV(UPE0))) {
        return SP_ERR_UART;
    }
    else return SP_NO_ERROR;
}

/***************************************************************
*                                                              *
* Function: timer_int_UART                                      *
*                                                              *
* Abstract: Called from SystemTimer to handle the serial       *
*           transmission.                                      *
*                                                              *
* Change history :                                             *
* ----------------                                             *
* 2018-01-15 BP        Initial version.                        *
***************************************************************/
void timer_int_UART()
{  
    UART_0_transmit();
}

// Takes a byte from FIFO and put it in transmit register
static void UART_0_transmit()
{
    if (rx_xoff_0) { //Should XOFF be sent ??
        if(!xoff_sent_0) {
            if(sp_flow_mode_0 == SP_INIT_SOFT_FLOW_CTR) {
                UDR0 = XOFF;        // Send XOFF
            }
            xoff_sent_0 =  true; // but just once
        }
    }
    if (tx_counter_0 && (UCSR0A & (_BV(UDRE0)))) { // OK to transmit
        UDR0 = *tx_emptyp_0++;
        if (tx_emptyp_0 > &tx_fifo_0[TX_FIFO_0_SIZE-1]) { // pointer at end ?
            tx_emptyp_0 = tx_fifo_0; // wrap fillpointer
        }
        tx_counter_0--; // One byte less in fifo
    }
}


/***************************************************************
*                                                              
* Function: uart_rx_isr                                         
*                                                              
* Abstract: Interrupt routine. UART has received a character                 
*           in register UDR.                                                              
* Change history :                                             
* ----------------                                             
* 2018-01-15 BP        Initial version.                            
***************************************************************/         
ISR(USART_RX_vect)
{   
volatile uint8_t dummy;
   if (++rx_counter_0 > BUFFER_OH_0) { //time to send xoff??
      rx_xoff_0 = true;          
      if(rx_counter_0 >= RX_FIFO_0_SIZE) { // check for overflow
         rx_stat_0 = (uint8_t)SP_ERR_OVERFLOW; // indicate loss of characters
         rx_counter_0--;
         dummy=UDR0; // Empty the register
         goto rx_end_0; // allow no overflow, end
      }   
   }
   *rx_fillp_0++ = UDR0; 
   if (rx_fillp_0 > &rx_fifo_0[RX_FIFO_0_SIZE-1]) { // emptypointer at end ?
      rx_fillp_0 = rx_fifo_0; // wrap emptypointer
   }
rx_end_0:;
}


/***************************************************************
*                                                              
* Function: uart_udre_isr                                         
*                                                              
* Abstract: Interrupt routine. Character transferred to shift 
*           register so UDR is now empty.
*                                                              
* Change history :                                             
* ----------------                                             
* 2018-01-15 BP        Initial version.                            
***************************************************************/         

ISR(USART_UDRE_vect)
{
   // Not used
   UCSR0A &= ~(_BV(UDRE0));
}

/***************************************************************
*
* Function: uart_tx_isr
*
* Abstract: Interrupt routine. Character has been transmitted
*
* Change history :
* ----------------
* 2018-01-15 BP        Initial version.
***************************************************************/
ISR(USART_TX_vect)
{
   UART_0_transmit();
}
