#include "avr/builtins.h"
#include "avr/common.h"
#include <avr/io.h>
#include <avr/interrupt.h>

/**
 * Interrupt handler for pin
 */
ISR(PCINT0_vect)
{
  PORTB ^= _BV(PB3);
}

void deep_sleep() {
  MCUCR |= _BV(SE);
  __builtin_avr_sleep();
}

/**
 * Setup the LED pin output
 */
void setup_led() {
  // set pin 3 as output
  DDRB |= _BV(DDB3);
  PORTB &= ~_BV(PB3);
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
void setup_button_interrupt() {
  // enable interrupt handler for PCINT1 (or PB1)
  PCMSK |= _BV(PCINT1);
  // make sure to enable pullup resistor by setting
  // the pullup-disable flag to off
  MCUCR &= ~_BV(PUD);
  // set pin 1 as input, with DDB1 to 0 and PORTB1 to 1
  DDRB &= ~_BV(DDB1);
  PORTB |= _BV(PB1);
}

int main (void) {
  setup_led();
  // disable interrupts while we configure our interrupts
  cli();
  setup_global_interrupt_flags();
  setup_button_interrupt();
  // enable interrupts again.
  sei();
  while(1) {}
}
