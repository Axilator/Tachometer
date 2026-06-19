/*
 * Monitor.c
 * Serial port Monitor for test purpose
 *
 * Created: 2018-01-16 13:07:11
 *  Author: Bengt Palmkvist
 */ 

#include <stdlib.h>
#include <avr/pgmspace.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "General.h"
#include "SerialPort.h"
#include "Init.h"
#include "IO.h"
#include "Version.h"
#include "Monitor.h"
#include "Timer.h"
#include "Main.h"
#include "TachoCalc.h"
#include "Eeprom.h"

enum INPUT_TYPE {INPUT_ALL, INPUT_TEXT, INPUT_HEX, INPUT_DEC};

static const char s_err_msg[] FLASH_MEM = {"ERROR"};
static const char s_OK_msg[] FLASH_MEM = {"OK"};
static const char s_CRLF_msg[] FLASH_MEM  = "\r\n";
static const char s_promt_msg[] FLASH_MEM = {"> "};
static const char s_logg_on[] FLASH_MEM = {" log on !\r\n"};
static const char s_wrong_selection[] FLASH_MEM = {" Wrong selection !\r\n"};    
    
    
static uint8_t m_sequence = 0;
#define TEMP_STR_LENGHT (8)
static char m_temp_str[TEMP_STR_LENGHT];
    
/***************************************************************
*  Local Function prototypes   
***************************************************************/
static uint8_t hex_to_numeric(uint8_t p_hexval);
static uint16_t put_int_to_str(uint16_t p_val);
static uint16_t put_long_to_str(uint32_t value);
static uint16_t put_int_to_hex_str(uint16_t p_val);
static void put_int_to_dec_and_hex_str(uint16_t value);


/***************************************************************
*  Transmits the Debug Log level to serial line
***************************************************************/
static void puts_logg_on(void)
{
    sp_puts_flash(FLASH_STR(" Log level: "));
    put_int_to_hex_str(g_logg_level);
    sp_puts_flash(s_logg_on);
}

/***************************************************************
*  Transmits the Help menu to serial line
***************************************************************/
static void help_menu_1(void) {
   sp_puts_flash(FLASH_STR("  H -> Help ................................\r\n"));   
   sp_puts_flash(FLASH_STR("  T -> Set Tach value.......................\r\n"));
   sp_puts_flash(FLASH_STR("  G -> Set Green LED value..................\r\n"));
   sp_puts_flash(FLASH_STR("  R -> Set Red LED value....................\r\n")); 
   sp_puts_flash(FLASH_STR("  A -> Read A/D value.......................\r\n"));
   sp_puts_flash(FLASH_STR("  I -> Get Misc Information.................\r\n"));
   sp_puts_flash(FLASH_STR("  K -> Emulate Key Press....................\r\n"));
   sp_puts_flash(FLASH_STR("  S -> Stop/Start the Tacho Function........\r\n"));
   sp_puts_flash(FLASH_STR("  C -> Set Tacho Calibration values.........\r\n"));
   sp_puts_flash(FLASH_STR("  L -> Set LED Calibration values...........\r\n"));
   sp_puts_flash(FLASH_STR("  + -> Increase Tacho value.................\r\n"));
   sp_puts_flash(FLASH_STR("  - -> Decrease Tacho value.................\r\n"));
   sp_puts_flash(FLASH_STR("  9 -> Log Help (1 All log on, 0 log off)...\r\n"));
   sp_puts_flash(FLASH_STR("  ESC -> Breaks the command..................\r\n"));
}

/***************************************************************
* Help function    converts a ascii hex value to numeric
* returns 0 - 16
* 970410 BP  Initial version
***************************************************************/
static uint8_t hex_to_numeric(uint8_t hexval) 
{
   if(isxdigit(hexval)) {    /* check for hex-digit */
      if(isdigit(hexval)) {  /* 0 - 9 */
         return (uint8_t) (hexval & 0x0f);
      }
      else {     /* hex a - f */
         return (uint8_t)(hexval & 0x07)+9;
      }
   }
   else return(0);
}

/***************************************************************
*                                                              
* Function: put_int_to_str                                          
*                                                              
* Abstract: Transmits the USHORT parameter on serial             
*           line in decimal format.                                
*                                                              
* Change history :                                             
* ----------------                                             
* 930315 BP        Initial version.                            
***************************************************************/
static uint16_t put_int_to_str(uint16_t value)
{
char str[6];
    //itoa(value, str, 10);
    sprintf(str,"%d",value);
    return sp_puts(str);
}

/***************************************************************
* Transmits the ULONG parameter on serial line in decimal format.
***************************************************************/
static uint16_t put_long_to_str(uint32_t value)
{
    char str[12];
    //ltoa(value, str, 10);
    sprintf(str,"%lu", value);
    return sp_puts(str);
}

/***************************************************************
* Transmits the USHORT parameter on serial line in hex format.
***************************************************************/
static uint16_t put_int_to_hex_str(uint16_t value)
{
char str[6];
    //itoa(value, str, 16);
    sprintf(str,"0x%x", value);
    return sp_puts(str);
}

/***************************************************************
* Transmits the USHORT parameter on serial line in both
* decimal and hex format
***************************************************************/
static void put_int_to_dec_and_hex_str(uint16_t value)
{
    put_int_to_str(value);
    sp_puts_flash(FLASH_STR("(dec), "));
    put_int_to_hex_str(value);
    sp_puts_flash(FLASH_STR("(hex)"));
}


/***************************************************************
*   Checks for end of a command, CR or LF
***************************************************************/
static bool is_end_of_command(uint8_t value)
{
    if((value == '\r') || (value == '\n')) {
        tim_wait_thenths(1);
        sp_clear_rx_fifo();
        return true;
    }
    return false;
}

/***************************************************************
*   Reads an input string
***************************************************************/
static bool read_our_temp_string(uint8_t value, uint8_t typeOfInput)
{
    // INPUT_ALL, INPUT_TEXT, INPUT_HEX, INPUT_DEC
    if(value == BS) {
        m_sequence--;
         if(m_sequence < TEMP_STR_LENGHT) {
             m_temp_str[m_sequence] = '\0';
             return true;
         }       
         goto ret_error;      
    }
    if(typeOfInput == INPUT_HEX) {
         if(!isxdigit(value)) { 
             return true; // Just skip
         } 
    }
    else if(typeOfInput == INPUT_DEC) {
        if(!isdigit(value)) {
            return true; // Just skip
        }            
    }        
    
    if(m_sequence < TEMP_STR_LENGHT) {
        m_temp_str[m_sequence] = value;
        m_sequence++;
        return true;
    }
ret_error:
    m_sequence = 0;
    return false;
}

/***************************************************************/

/***************************************************************
*                                                              
* Function: monitor_main                                        
*                                                              
* Abstract: Supports monitor function      
* Reads serial port for command. 
* Monitor is of "flow-trough" type so execution is not stopped
* inside it.                    
*                                                              
* Change history :                                             
* ----------------                                             
* 930310 BP        Initial version.                            
***************************************************************/
void monitor_main()
{
    uint8_t value;
    uint8_t mode;
    static uint8_t m_mode = 0, m_mode_sec = 0;
    static uint16_t value_sec = 0;
    int tmp_int;
    uint32_t tmp_ulong;

    if(sp_getc(&value) != SP_NO_ERROR) {
        return;
    }    

    sp_putc(value &= 0x7f);

    if(value == ESC) {
        m_sequence = m_mode = m_mode_sec = mode = 0;
        sp_puts_flash(FLASH_STR("^Break\r\n> "));
        return;
    }

    if(m_mode == 0) {
        mode = value;
    }
    else {
        mode = m_mode;
    }        

    switch(mode) { 
        case 't':
        case 'T': // Set Tacho PWM new value
            if(is_end_of_command(value)) {
                m_temp_str[m_sequence] = '\0';
                m_sequence = m_mode = m_mode_sec = 0;
                sp_puts_flash(s_CRLF_msg);
                tmp_int = atoi(m_temp_str);
                io_set_duty_PWM_tacho((uint16_t)tmp_int);
                sp_puts_flash(FLASH_STR("Tacho PWM new value: "));
                put_int_to_dec_and_hex_str(io_get_duty_PWM_tacho());
                sp_puts_flash(s_CRLF_msg);
            }
            else {
                if(m_mode == 0) {
                    sp_puts_flash(FLASH_STR("Set Tacho PWM value (0 - 16383): "));
                    m_mode = mode;
                }
                else {
                    if(!read_our_temp_string(value, INPUT_DEC)) {
                        sp_puts_flash(s_CRLF_msg);
                        sp_puts_flash(s_err_msg);
                    }
                }
            }
            break;    
            
        case 'g':
        case 'G': // Green LED
            if(is_end_of_command(value)) {
                m_temp_str[m_sequence] = '\0';
                m_sequence = m_mode = m_mode_sec = 0;
                sp_puts_flash(s_CRLF_msg);
                tmp_int = atoi(m_temp_str);
                io_set_duty_PWM_LED_green((uint8_t)tmp_int);
                sp_puts_flash(FLASH_STR("Green LED new value: "));
                put_int_to_dec_and_hex_str(io_get_duty_PWM_LED_green());
                sp_puts_flash(s_CRLF_msg);
            }
            else {
                if(m_mode == 0) {
                    sp_puts_flash(FLASH_STR("Set Green LED value (0-255): "));
                    m_mode = mode;
                }
                else {
                    if(!read_our_temp_string(value, INPUT_DEC)) {
                        sp_puts_flash(s_CRLF_msg);
                        sp_puts_flash(s_err_msg);
                    }
                }
            }
            break;  
            
        case 'r':
        case 'R': // Red LED
            if(is_end_of_command(value)) {
                m_temp_str[m_sequence] = '\0';
                m_sequence = m_mode = m_mode_sec = 0;
                sp_puts_flash(s_CRLF_msg);
                tmp_int = atoi(m_temp_str);
                io_set_duty_PWM_LED_red((uint8_t)tmp_int);
                sp_puts_flash(FLASH_STR("Red LED new value: "));
                put_int_to_dec_and_hex_str(io_get_duty_PWM_LED_red());
                sp_puts_flash(s_CRLF_msg);
            }
            else {
                if(m_mode == 0) {
                    sp_puts_flash(FLASH_STR("Set Red LED value (0-255): "));
                    m_mode = mode;
                }
                else {
                    if(!read_our_temp_string(value, INPUT_DEC)) {
                        sp_puts_flash(s_CRLF_msg);
                        sp_puts_flash(s_err_msg);
                    }
                }
            }
            break;                     

        case 'A':
        case 'a':
            tmp_int = io_read_AD_value();
            sp_puts_flash(FLASH_STR("A/D value is:  "));
            put_int_to_dec_and_hex_str((uint8_t)tmp_int);
            sp_puts_flash(s_CRLF_msg);
            break;

        case 'k':
        case 'K':
            if(m_sequence == 0) {
                sp_puts_flash(FLASH_STR("Emulate Key Press\r\n"));
                sp_puts_flash(FLASH_STR("A: Sw 1 short press   B: Sw 2 short press\r\n"));
                sp_puts_flash(FLASH_STR("C: Sw 1 long press    D: Sw 2 long press\r\n"));
                sp_puts_flash(FLASH_STR("E: Both keys pressed\r\n"));
                sp_puts_flash(FLASH_STR("Select Key, A-E : "));
                m_sequence++;
                m_mode = mode;
            } 
            else {
                value = tolower(value);
                if((value >= 'a') && (value <= 'e')) {
                    if(value == 'a') tmp_int = SW_1_SHORT_PRESS;
                    else if(value == 'b') tmp_int = SW_2_SHORT_PRESS;
                    else if(value == 'c') tmp_int = SW_1_LONG_PRESS;
                    else if(value == 'd') tmp_int = SW_2_LONG_PRESS;
                    else if(value == 'e') tmp_int = SW_1_SHORT_PRESS | SW_2_SHORT_PRESS;
                    io_set_switch_status_test((SWITCH_STAT_TYPE)tmp_int);
                    sp_puts_flash(s_OK_msg);
                }
                else {
                    sp_puts_flash(s_wrong_selection);
                }
                m_sequence = m_mode = m_mode_sec = 0;
                sp_puts_flash(s_CRLF_msg);
            }          
            break;     
            

        case 'c':
        case 'C': // Set Tacho PWM calibration value
            if(is_end_of_command(value)) {
                m_temp_str[m_sequence] = '\0';
                m_sequence = 0;
                if(m_mode_sec == 0) {
                    value_sec = atoi(m_temp_str);
                    m_mode_sec++;
                    sp_puts_flash(FLASH_STR("\r\nOld value is: "));
                    put_int_to_dec_and_hex_str(tac_get_calib_value(value_sec));
                    sp_puts_flash(FLASH_STR("\r\nSet new Tacho calibration value (0 - 16383): "));
                }                    
                else {
                    sp_puts_flash(s_CRLF_msg);
                    tmp_int = atoi(m_temp_str);
                    tac_set_calib_value(value_sec, tmp_int);
                    sp_puts_flash(FLASH_STR("New value is: "));
                    put_int_to_dec_and_hex_str(tac_get_calib_value(value_sec));
                    sp_puts_flash(s_CRLF_msg);
                    value_sec = m_mode = m_mode_sec = 0;
                }                
            }
            else {
                if(m_mode == 0) {
                    sp_puts_flash(FLASH_STR("Tacho calibration. Select which value (0 - 10)\r\n"));
                    sp_puts_flash(FLASH_STR("0 -> 500 rpm, 1 -> 1000 rpm, 2 -> 2000 rpm ....: "));
                    m_mode = mode;
                }
                else {
                    if(!read_our_temp_string(value, INPUT_DEC)) {
                        sp_puts_flash(s_CRLF_msg);
                        sp_puts_flash(s_err_msg);
                    }
                }
            }
            break;       
              
        case 'l':
        case 'L': // Set LED PWM calibration value
            if(is_end_of_command(value)) {
                m_temp_str[m_sequence] = '\0';
                m_sequence = 0;
                if(m_mode_sec == 0) {
                    value_sec = atoi(m_temp_str);
                    m_mode_sec++;
                    if(value_sec != 0) {
                        ee_write_uint16(EE_RED_LED_RPM_START, value_sec);
                    }
                    sp_puts_flash(FLASH_STR(" ok\r\nSet Red LED increment value: "));
                }
                else if(m_mode_sec == 1) {
                    value_sec = atoi(m_temp_str);
                    m_mode_sec++;
                    if(value_sec != 0) {
                        ee_write_uint16(EE_RED_LED_RPM_INC, value_sec);
                    }
                    sp_puts_flash(FLASH_STR(" ok\r\nSet Green LED min value: "));
                }     
                else if(m_mode_sec == 2) {
                    value_sec = atoi(m_temp_str);
                    m_mode_sec++;
                    if(value_sec != 0) {
                        ee_write_byte(EE_GREEN_LED_START, value_sec);
                    }
                    sp_puts_flash(FLASH_STR(" ok\r\nSet Green LED max value: "));
                }                             
                else { // 3
                    value_sec = atoi(m_temp_str);
                    if(value_sec != 0) {
                        ee_write_byte(EE_GREEN_LED_START, value_sec);
                    }
                    sp_puts_flash(FLASH_STR(" ok! Ready\r\n"));
                    value_sec = m_mode = m_mode_sec = 0;
                }
            }
            else {
                if(m_mode == 0) {
                    sp_puts_flash(FLASH_STR("LED calibration. 0 -> No change. Red LED start at RPM: \r\n"));
                    m_mode = mode;
                }
                else {
                    if(!read_our_temp_string(value, INPUT_DEC)) {
                        sp_puts_flash(s_CRLF_msg);
                        sp_puts_flash(s_err_msg);
                    }
                }
            }
            break;   

        case 'S':
        case 's':
            g_stop_tacho_func = !g_stop_tacho_func;
            sp_puts_flash(FLASH_STR("Tachometer function is "));
            if(g_stop_tacho_func) {
                sp_puts_flash(FLASH_STR("OFF"));
            }
            else {
                sp_puts_flash(FLASH_STR("ON"));
            }
            sp_puts_flash(s_CRLF_msg);
            break;   
            
        case '+':
            sp_puts_flash(FLASH_STR("New tacho value: "));
            io_set_duty_PWM_tacho(io_get_duty_PWM_tacho() + 1);
            put_int_to_dec_and_hex_str(io_get_duty_PWM_tacho());
            sp_puts_flash(s_CRLF_msg);
            break;                              
            
        case '-':
            sp_puts_flash(FLASH_STR("New tacho value: "));
            tmp_int = io_get_duty_PWM_tacho() - 1;
            if(tmp_int < 0) tmp_int = 0;
            io_set_duty_PWM_tacho(tmp_int);
            put_int_to_dec_and_hex_str(io_get_duty_PWM_tacho());
            sp_puts_flash(s_CRLF_msg);
            break;
                    
        case 'I':
        case 'i':
            sp_puts_flash(FLASH_STR("\r\nRPM: "));
            put_long_to_str(tac_get_RPM());
            sp_puts_flash(s_CRLF_msg);
            sp_puts_flash(FLASH_STR("PWM Duty value for tacho: "));
            put_int_to_dec_and_hex_str(io_get_duty_PWM_tacho());
            sp_puts_flash(FLASH_STR("  Green LED: "));
            put_int_to_dec_and_hex_str(io_get_duty_PWM_LED_green());
            sp_puts_flash(FLASH_STR("  Red LED: "));
            put_int_to_dec_and_hex_str(io_get_duty_PWM_LED_red());
            sp_puts_flash(s_CRLF_msg);
            sp_puts_flash(FLASH_STR("Reset reason: "));
            put_int_to_hex_str(g_reset_reason);
            sp_puts_flash(s_CRLF_msg);
            sp_puts_flash(FLASH_STR("Tacho calib val:\r\n"));
            for (uint8_t i=0; i<EE_NBR_OF_TACHO_CALIB; i++ ) {
                put_int_to_str(i);
                sp_puts_flash(FLASH_STR(": "));
                put_int_to_dec_and_hex_str(tac_get_calib_value(i));
                if((i+1)%3 == 0) {
                    sp_puts_flash(s_CRLF_msg);
                }
                else {
                    sp_putc('\t');
                }                    
            }                
            sp_puts_flash(s_CRLF_msg);
            sp_puts_flash(FLASH_STR("Red LED RPM start (EE): "));
            put_int_to_str(ee_read_uint16(EE_RED_LED_RPM_START));
            sp_puts_flash(FLASH_STR(", Step: "));
            put_int_to_str(ee_read_uint16(EE_RED_LED_RPM_INC));
            
            sp_puts_flash(FLASH_STR("Green LED A/D min (EE): "));
            put_int_to_hex_str(ee_read_uint16(EE_RED_LED_RPM_START));
            sp_puts_flash(FLASH_STR(", Max: "));
            put_int_to_hex_str(ee_read_uint16(EE_RED_LED_RPM_INC));
            break;
                        
        case '9':
            sp_puts_flash(FLASH_STR("Log Help:\r\n"));
            sp_puts_flash(FLASH_STR("2 - 7 ADDS log type\r\n"));
            sp_puts_flash(FLASH_STR("0 -> All logging off\r\n"));
            sp_puts_flash(FLASH_STR("1 -> All log on\r\n"));
            sp_puts_flash(FLASH_STR("2 -> Log Trace\r\n"));
            sp_puts_flash(FLASH_STR("3 -> Log Report\r\n"));
            sp_puts_flash(FLASH_STR("4 -> Log Comment\r\n"));
            sp_puts_flash(FLASH_STR("5 -> Log Error\r\n"));
            sp_puts_flash(FLASH_STR("6 -> Log Tacho\r\n"));

            sp_puts_flash(FLASH_STR("9 -> This help text\r\n"));    
        break;
        
        case '6':
            sp_puts_flash(FLASH_STR("Log Tacho"));
            g_logg_level |= LOGG_TACHO;
            puts_logg_on();
            break;        
        case '5':
            sp_puts_flash(FLASH_STR("Log Error"));
            g_logg_level |= LOGG_ERROR;
            puts_logg_on();
            break;
        case '4':
            sp_puts_flash(FLASH_STR("Log Comment"));
            g_logg_level |= LOGG_COMMENT;
            puts_logg_on();
            break;
        case '3':
            sp_puts_flash(FLASH_STR("Log Report"));
            g_logg_level |= LOGG_REPORT;
            puts_logg_on();            
            break;
        case '2':
            sp_puts_flash(FLASH_STR("Log Trace"));
            g_logg_level |= LOGG_TRACE;
            puts_logg_on();
            break;
        case '1':
            sp_puts_flash(FLASH_STR("Log All"));
            g_logg_level = LOGG_ALL_ON;
            puts_logg_on();            
            break;
        case '0':
            sp_puts_flash(FLASH_STR("All log off !\r\n"));
            g_logg_level = 00;
            break;

        case 'H':
        case 'h':
            sp_puts_flash(s_CRLF_msg);
            sp_puts_flash(FLASH_STR(" ***  Tacho  Version: " ));
            sp_puts_flash(g_version);
            sp_puts_flash(FLASH_STR("  ***\r\n\r\n" ));
            help_menu_1();
            break;
            
        default:
            sp_puts_flash(s_CRLF_msg);
            help_menu_1();
            tim_wait_thenths(3);
            sp_clear_rx_fifo();
            break;

    }
    if(m_mode == 0) {
        sp_puts_flash(s_CRLF_msg);
        sp_puts_flash(s_promt_msg);
    }        
}
/**************************************************************/    
    
    

    
    
    
    
    
    
    