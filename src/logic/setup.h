#ifndef WATER_PEN_SETUP_H
#define WATER_PEN_SETUP_H

#include <stdint.h>

/**
 * Setup the LED pin output
 *
 * @param[in] pin The pin to setup.
 */
void setup_led(uint8_t pin);

/**
 * Setup the interrupt flags on the appropriate registers.
 */
void setup_global_interrupt_flags();

/**
 * Setup a button to be an interrupt on a given pin.
 *
 * @param[in] pin The pin to setup.
 */
void setup_button_interrupt(uint8_t pin);

#endif
