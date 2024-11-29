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
#include "logic/calibration.h"
#include "types/state_types.h"
#include "logic/setup.h"
#include "logic/state.h"

#define LED_PIN PB3

// DEBUG FUNCTIONS
static void error_loop() {
  PORTB |= _BV(PB3);
  while(1) {

  }
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


// globals
struct state control_state;


int main (void) {
  // disable interrupts while we configure our interrupts
  cli();
  setup_global_interrupt_flags();
  setup_button_interrupt(PB1);
  // enable interrupts again.
  sei();

  setup_led(LED_PIN);
  adc_init();
  // TODO check if we have calibration info stored in the eeprom
  i2c_init(true);
  while(1) {
    switch (control_state.mode) {
      case WP_CALIBRATE: {
        control_state.calibration_info = calibrate_soil_sensor(LED_PIN);
        // TODO save result in EEPROM
        control_state.mode = WP_SLEEP;
        break;
      }
      case WP_READ: {
        uint16_t result = soil_sensor_read();
        print_int(result);
        uint8_t percentage = percentage_from_value(
          result, &control_state.calibration_info);
        print_int(percentage);
        // TODO pull potentiometer data to check if value is
        // below percentage threshold.
        //
        control_state.mode = WP_SLEEP;
        break;
      }
      case WP_TRIGGER: {
        enable_trigger_state(&control_state, LED_PIN);
        // go to sleep -- the LED should stay on during power down
        control_state.mode = WP_SLEEP;
        // reset wakeup_count
        control_state.wakeup_count = 0;
        break;
      }
      case WP_SLEEP: {
        // TODO use sleep method here
        break;
      }
    }
  }
}

// Interrupt handlers

/**
 * Interrupt handler for pin
 */
ISR(PCINT0_vect)
{
  // turn off LED if it was on
  disable_trigger_state(&control_state, LED_PIN);
  control_state.mode = WP_CALIBRATE;
  // set wakeup count to the max so it reads the next time we wake.
  control_state.wakeup_count = WAKEUP_LIMIT;
}

/**
 * Watchdog wakeup interrupt handler
 */
ISR(WDT_vect)
{
  wakeup_operations(&control_state);
}

