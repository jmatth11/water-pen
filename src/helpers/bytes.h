#ifndef JM_AVR_BYTES_H
#define JM_AVR_BYTES_H

#include <stdint.h>

/**
 * Write a uint16_t out to a uint8_t array in little endian format.
 *
 * @param[in] value The target value.
 * @param[out] buf The array to populate with the broken up target value.
 */
void write_16_to_8(uint16_t value, uint8_t buf[2]);

/**
 * Read a uint16_t from a uint8_t array. Expects little endian format.
 *
 * @param[in] buf The array to construct the target value from.
 * @param[out] value The target value to populate.
 */
void read_8_to_16(uint8_t buf[2], uint16_t* value);

#endif
