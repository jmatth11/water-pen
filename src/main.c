#include "avr/builtins.h"
#include "avr/common.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include <util/delay.h>
#include <util/atomic.h>

#include "avr/wdt.h"
#include "components/soil_sensor.h"
#include "components/potentiometer.h"
#include "driver/adc.h"
#include "driver/slp.h"
#include "logic/calibration.h"
#include "logic/data.h"
#include "types/state_types.h"
#include "logic/setup.h"
#include "logic/state.h"

#ifdef DEBUG_MODE
#include "driver/i2c.h"
#include "helpers/print.h"
#endif

#define ERROR_LED PB3
// right now it is the same as error, once using rgb led it will change
#define CAL_LED PB3
// flags for custom interrupt mask
/* To signal an interrupt for calibration */
#define I_CAL 0
/* To signal an interrupt from a wakeup */
#define I_WAKE 1
/**
 * Set bit to 0 to signal a handle event, set to 1 for the handler to
 * accept new handle requests.
 */
#define I_HANDLE 2
// volatile shared mask between interrupts and main thread
static volatile uint8_t interrupt_mask = 0;


// globals
static struct state control_state;

static void handle_interrupt_values(struct state* state) {
#ifdef DEBUG_MODE
  print("handler");
  print_int(interrupt_mask);
#endif
  // if the handle flag is set it means no request has come in
  if (interrupt_mask & _BV(I_HANDLE)) {
    return;
  }
  // create an atomic block for these operations
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
  {
    if (interrupt_mask & _BV(I_CAL)) {
      // turn off LED if it was on
      disable_trigger_state(state, ERROR_LED);
      state->mode = WP_CALIBRATE;
      // we don't clear the I_CAL bit in here we wait until the calibration is done.
      // this gives enough time for extra interrupts from the button to finish out
    } else if (interrupt_mask & _BV(I_WAKE)) {
      wakeup_operations(state);
    }
    // signal interrupt handler can accept requests again by reseting mask
    interrupt_mask = _BV(I_HANDLE);
  }
#ifdef DEBUG_MODE
  print_int(interrupt_mask);
#endif
}


int main (void) {
  // disable interrupts while we configure our interrupts
  cli();
  enable_deep_sleep();
  setup_global_interrupt_flags();
  setup_button_interrupt(PB1);
  // enable interrupts again.
  sei();

  setup_led(ERROR_LED);
  // TODO uncomment when using rgb led
  //setup_led(CAL_LED);
  adc_init();

  // start off with at least one sleep
  control_state.wakeup_count = WAKEUP_LIMIT;
  control_state.mode = WP_SLEEP;
  // read the eeprom for calibration data
  if (!data_read_calibration_info(&control_state.calibration_info)) {
    // if the device hasn't been calibrated yet, put it in an error mode
    // until the user calibrates
    control_state.mode = WP_TRIGGER;
  }

#ifdef DEBUG_MODE
  i2c_init(true);
#endif

  // set interrupt_mask to default value
  interrupt_mask = _BV(I_HANDLE);
  while(1) {

    handle_interrupt_values(&control_state);

    switch (control_state.mode) {
      case WP_CALIBRATE: {
#ifdef DEBUG_MODE
        print("cal");
#endif
        // wait at least a second in case we are transitioning from error mode
        // to tell the difference between "error" LED and "calibration" LED
        _delay_ms(1000);
        control_state.calibration_info = calibrate_soil_sensor(CAL_LED);
        data_save_calibration_info(&control_state.calibration_info);
#ifdef DEBUG_MODE
        print_int(control_state.calibration_info.air_threshold);
        print_int(control_state.calibration_info.water_threshold);
        print("saved");
#endif
        // set wakeup count to the max so it reads the next time we wake.
        control_state.wakeup_count = WAKEUP_LIMIT;
        control_state.mode = WP_SLEEP;
        // safe to finally clear bit here
        interrupt_mask &= ~_BV(I_CAL);
        break;
      }
      case WP_READ: {
#ifdef DEBUG_MODE
        print("read");
#endif
        // if the values are default we want to trigger the error state
        // to tell the user to calibrate
        if (data_are_default_values(&control_state.calibration_info)) {
          control_state.mode = WP_TRIGGER;
          break;
        }
        uint16_t result = soil_sensor_read();
        uint8_t percentage = percentage_from_value(
          result, &control_state.calibration_info);
        uint16_t threshold = potentiometer_sensor_read();
        control_state.error_triggered = percentage <= threshold;
        if (control_state.error_triggered) {
          control_state.mode = WP_TRIGGER;
        } else {
          control_state.mode = WP_SLEEP;
        }
#ifdef DEBUG_MODE
        print_int(result);
        print_int(percentage);
        print_int(threshold);
#endif
        break;
      }
      case WP_TRIGGER: {
#ifdef DEBUG_MODE
        print("trigger");
#endif
        enable_trigger_state(&control_state, ERROR_LED);
        // go to sleep -- the LED should stay on during power down
        control_state.mode = WP_SLEEP;
        // reset wakeup_count
        control_state.wakeup_count = 0;
        break;
      }
      case WP_SLEEP: {
#ifdef DEBUG_MODE
        print("sleep");
        print_int(control_state.calibration_info.air_threshold);
        print_int(control_state.calibration_info.water_threshold);
        print_int(control_state.wakeup_count);
#endif
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
  if (interrupt_mask & _BV(I_HANDLE)) {
    interrupt_mask &= ~_BV(I_HANDLE);
    interrupt_mask |= _BV(I_CAL);
  }
}

/**
 * Watchdog wakeup interrupt handler
 */
ISR(WDT_vect)
{
  if (interrupt_mask & _BV(I_HANDLE)) {
    interrupt_mask &= ~_BV(I_HANDLE);
    interrupt_mask |= _BV(I_WAKE);
  }
}

