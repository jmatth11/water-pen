#include "setup.h"
#include <avr/io.h>

/**
 * Setup the LED pin output
 */
void setup_led(uint8_t pin) {
  // set pin 3 as output
  DDRB |= _BV(pin);
  PORTB &= ~_BV(pin);
}

/**
 * Setup the interrupt flags on the appropriate registers.
 */
void setup_global_interrupt_flags() {
  // SET the global interrupt enable flag
  SREG |= _BV(SREG_I);
  // Enable PCINT interrupt in the general interrupt mask
  GIMSK |= _BV(PCIE);
}

/**
 * Setup a button to be an interrupt on a given pin.
 */
void setup_button_interrupt(uint8_t pin) {
  // enable interrupt handler for PCINT1 (or PB1)
  PCMSK |= _BV(pin);
  // set pin interrupt to trigger on rising edge
  MCUCR |= (
    _BV(ISC00) | _BV(ISC01)
  );
  // set pin 1 as input, with DDB1 to 0
  DDRB &= ~_BV(pin);
}
