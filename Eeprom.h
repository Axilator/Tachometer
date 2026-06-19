/*
 * Eeprom.h
 * Handle EEPROM read/write
 *
 * Created: 2018-01-25 12:21:14
 * Author: Bengt Palmkvist
 */ 


#ifndef EEPROM_H_
#define EEPROM_H_

#include <stdint.h>

//////////////////////////////////////////////////////////////////////////
//    Eeprom address defines
//////////////////////////////////////////////////////////////////////////
#define EE_START                        0
#define EEPROM_SIZE                     1024

#define EE_TACHO_CALIB_START         EE_START
#define EE_TACHO_CALIB_05            EE_TACHO_CALIB_START
#define EE_TACHO_CALIB_1            (EE_TACHO_CALIB_05+2)
#define EE_TACHO_CALIB_2            (EE_TACHO_CALIB_1+2)
#define EE_TACHO_CALIB_3            (EE_TACHO_CALIB_2+2)
#define EE_TACHO_CALIB_4            (EE_TACHO_CALIB_3+2)
#define EE_TACHO_CALIB_5            (EE_TACHO_CALIB_4+2)
#define EE_TACHO_CALIB_6            (EE_TACHO_CALIB_5+2)
#define EE_TACHO_CALIB_7            (EE_TACHO_CALIB_6+2)
#define EE_TACHO_CALIB_8            (EE_TACHO_CALIB_7+2)
#define EE_TACHO_CALIB_9            (EE_TACHO_CALIB_8+2)
#define EE_TACHO_CALIB_10           (EE_TACHO_CALIB_9+2)
#define EE_TACHO_CALIB_END          (EE_TACHO_CALIB_10+2)
#define EE_NBR_OF_TACHO_CALIB       ((EE_TACHO_CALIB_END - EE_TACHO_CALIB_START) / 2)
#define EE_TACHO_CALIB_NBR(address) ((address - EE_TACHO_CALIB_START)/2)

#define EE_RED_LED_RPM_START        EE_TACHO_CALIB_END
#define EE_RED_LED_RPM_INC          (EE_RED_LED_RPM_START+2)

#define EE_GREEN_LED_START          (EE_RED_LED_RPM_INC+2)
#define EE_GREEN_LED_MAX            (EE_GREEN_LED_START+1)

extern uint8_t ee_read_byte(uint16_t address);
extern void ee_write_byte(uint16_t address, uint8_t data);
extern uint16_t ee_read_uint16(uint16_t address);
extern void ee_write_uint16(uint16_t address, uint16_t data);

#endif /* EEPROM_H_ */
