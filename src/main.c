#include "avr/builtins.h"
#include "avr/common.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include <stdio.h>
#include <util/delay.h>
#include <string.h>

#include "avr/wdt.h"
#include "components/soil_sensor.h"
#include "driver/adc.h"
#include "driver/i2c.h"
#include "driver/slp.h"
#include "logic/calibration.h"
#include "logic/data.h"
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
static void print(char* str) {
  const uint32_t len = strlen(str);
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
  print(buf);
}


// globals
struct state control_state;


int main (void) {
  // disable interrupts while we configure our interrupts
  cli();
  enable_deep_sleep();
  setup_global_interrupt_flags();
  setup_button_interrupt(PB1);
  // enable interrupts again.
  sei();

  setup_led(LED_PIN);
  adc_init();

  // read the eeprom for calibration data
  if (!data_read_calibration_info(&control_state.calibration_info)) {
    // if the device hasn't been calibrated yet, put it in an error mode
    // until the user calibrates
    control_state.mode = WP_TRIGGER;
  }

  i2c_init(true);

  while(1) {
    switch (control_state.mode) {
      case WP_CALIBRATE: {
        // wait at least a second in case we are transitioning from error mode
        // to tell the difference between "error" LED and "calibration" LED
        _delay_ms(1000);
        control_state.calibration_info = calibrate_soil_sensor(LED_PIN);

        print("cal");
        print_int(control_state.calibration_info.air_threshold);
        print_int(control_state.calibration_info.water_threshold);

        data_save_calibration_info(&control_state.calibration_info);
        print("saved");
        control_state.mode = WP_SLEEP;
        break;
      }
      case WP_READ: {
        print("read");
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
        print("trigger");
        enable_trigger_state(&control_state, LED_PIN);
        // go to sleep -- the LED should stay on during power down
        control_state.mode = WP_SLEEP;
        // reset wakeup_count
        control_state.wakeup_count = 0;
        break;
      }
      case WP_SLEEP: {
        print("sleep");
        // sleep for longest time -- 8sec
        wd_sleep(WDTO_8S);
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
  control_state.mode = WP_CALIBRATE;
  // turn off LED if it was on
  disable_trigger_state(&control_state, LED_PIN);
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

