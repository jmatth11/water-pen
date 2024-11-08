#include "avr/common.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <time.h>

#define MS_DELAY 1000

ISR(PCINT0_vect)
{
  PORTB ^= _BV(PB3);
}

int main (void) {
  // set pin 3 as output
  DDRB |= _BV(DDB3);
  PORTB &= ~_BV(PB3);
  // disable interrupts while we configure our interrupts
  cli();
  // enable interrupt handler for PCINT1 (or PB1)
  PCMSK |= _BV(PCINT1);
  // SET the global interrupt enable flag
  SREG |= _BV(SREG_I);

  // Enable PCINT interrupt in the general interrupt mask
  GIMSK |= _BV(PCIE);
  // set pin 1 as input, with DDB1 to 0 and PORTB1 to 1
  DDRB &= ~_BV(DDB1);
  PORTB |= _BV(PB1);

  // make sure to enable pullup resistor by setting
  // the pullup-disable flag to off
  MCUCR &= ~_BV(PUD);

  // enable interrupts again.
  sei();
  while(1) {
    /*Wait 3000 ms */
   // _delay_ms(MS_DELAY);
  }
}
