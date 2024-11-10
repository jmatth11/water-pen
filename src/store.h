#ifndef JM_WATER_PEN_STORE_H
#define JM_WATER_PEN_STORE_H

#include <inttypes.h>

/**
 * Write a byte of data to the EEPROM.
 *
 * @param[in] addr The address to write to.
 * @param[in] data The byte to write.
 */
void eeprom_write(uint8_t addr, uint8_t data);

/**
 * Read a byte of data from the EEPROM.
 *
 * @param[in] addr The address to read from.
 * @return The byte of data.
 */
uint8_t eeprom_read(uint8_t addr);

#endif
