#ifndef JM_PRINT_H
#define JM_PRINT_H

#include <stdint.h>
#include <avr/io.h>

#ifndef ERROR_PIN
#define ERROR_PIN PB3
#endif

/**
 * Prints the given string to the I2C bus.
 * @param[in] str The given string.
 */
void print(const char* str);
/**
 * Prints the given number value to the I2C bus.
 * @param[in] val The given number.
 */
void print_int(const uint16_t val);


#endif
