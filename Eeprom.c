/*
 * Eeprom.c
 * Handle EEPROM read/write
 *
 * Created: 2018-01-25 12:20:55
 *  Author: Bengt Palmkvist
 */ 

#include "General.h"
#include <avr/eeprom.h>

uint8_t ee_read_byte(uint16_t address)
{
    return  eeprom_read_byte((uint8_t *)address);
}


void ee_write_byte(uint16_t address, uint8_t data)
{

  if (eeprom_read_byte((uint8_t *)address) != data) {
    AVR_ENTER_CRITICAL_REGION;                        
    eeprom_write_byte((uint8_t *)address, data);
    AVR_LEAVE_CRITICAL_REGION
  }
}


uint16_t ee_read_uint16(uint16_t address)
{
  uint16_t data = ee_read_byte(address) << 8;  
  data |= ee_read_byte(address + 1);   

  return data;
}


void ee_write_uint16(uint16_t address, uint16_t data)
{
  ee_write_byte(address, data >> 8);
  ee_write_byte(address + 1, data);  
}

