#include <avr/io.h>

#include "eeprom.h"

/**
 * Write a byte of data to the EEPROM.
 *
 * @param[in] addr The address to write to.
 * @param[in] data The byte to write.
 */
void eeprom_write(uint16_t addr, uint8_t data) {

  /* Wait for completion of previous write */
  while(EECR & _BV(EEPE));

  /* Set Programming mode */
  EECR = (0<<EEPM1)|(0<<EEPM0);
  /* Set up address and data registers */
  // address can't be bigger than 511
  EEAR = addr;
  EEDR = data;
  /* Write logical one to EEMPE */
  EECR |= _BV(EEMPE);
  /* Start eeprom write by setting EEPE */
  EECR |= _BV(EEPE);
}

/**
 * Read a byte of data from the EEPROM.
 *
 * @param[in] addr The address to read from.
 * @return The byte of data.
 */
uint8_t eeprom_read(uint16_t addr) {

  /* Wait for completion of previous write */
  while(EECR & (1<<EEPE));

  /* Set up address register */
  // address can't be bigger than 511
  EEAR = addr;
  /* Start eeprom read by writing EERE */
  EECR |= (1<<EERE);
  /* Return data from data register */
  return EEDR;
}
