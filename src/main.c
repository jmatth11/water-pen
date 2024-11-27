#include "avr/builtins.h"
#include "avr/common.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include <stdio.h>
#include <util/delay.h>
#include <string.h>

#include "components/soil_sensor.h"
#include "driver/adc.h"
#include "driver/i2c.h"
#include "types/state_types.h"


static void error_loop() {
  PORTB |= _BV(PB3);
  while(1) {

  }
}

// globals
struct state control_state;


static void increment_state_count() {
  ++control_state.wakeup_count;
  if (control_state.wakeup_count >= WAKEUP_LIMIT) {
    control_state.wakeup_count = 0;
    control_state.mode = WP_READ;
  }
}

/**
 * Setup the LED pin output
 */
static void setup_led() {
  // set pin 3 as output
  DDRB |= _BV(DDB3);
  PORTB &= ~_BV(PB3);
}

/**
 * Setup the interrupt flags on the appropriate registers.
 */
static void setup_global_interrupt_flags() {
  // SET the global interrupt enable flag
  SREG |= _BV(SREG_I);
  // Enable PCINT interrupt in the general interrupt mask
  GIMSK |= _BV(PCIE);
}

/**
 * Setup a button to be an interrupt on a given pin.
 */
static void setup_button_interrupt() {
  // enable interrupt handler for PCINT1 (or PB1)
  PCMSK |= _BV(PCINT1);
  // make sure to enable pullup resistor by setting
  // the pullup-disable flag to off
  MCUCR &= ~_BV(PUD);
  // set pin 1 as input, with DDB1 to 0 and PORTB1 to 1
  DDRB &= ~_BV(DDB1);
  PORTB |= _BV(PB1);
}

static void print(char* str, uint32_t len) {
  if (!i2c_start()) {
    error_loop();
  }
  if (i2c_write_address(4, true) & 0x01) error_loop();
  for (int i = 0; i < len; ++i) {
    if (i2c_write_byte(str[i]) & 0x01) error_loop();
  }
  i2c_stop();
}

static void print_int(uint16_t val) {
  char buf[5] = {0,0,0,0,0};
  sprintf(buf, "%d", val);
  print(buf, strlen(buf));
}

int main (void) {
  // disable interrupts while we configure our interrupts
  cli();
  setup_global_interrupt_flags();
  setup_button_interrupt();
  // enable interrupts again.
  sei();

  setup_led();
  adc_init();
  i2c_init(true);
  while(1) {

    print("start", 5);

    uint16_t result = soil_sensor_read();
    print_int(result);

    _delay_ms(1000);
  }
}

// Interrupt handlers

/**
 * Interrupt handler for pin
 */
ISR(PCINT0_vect)
{
  control_state.mode = WP_CALIBRATE;
}

/**
 * Watchdog wakeup interrupt handler
 */
ISR(WDT_vect)
{
  increment_state_count();
}

