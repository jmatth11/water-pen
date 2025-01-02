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

/**
 * This is a simple program for the ATtiny85 microcontroller to monitor
 * potted plants. The form factor is meant to be small like a pen.
 *
 * The functionality includes reading the soil moisture once every hour and
 * displaying an error LED if the moisture is lower then the threshold that is
 * set by the potentiometer.
 * However, before use, this device needs to be calibrated by pressing its
 * button. Position the soil sensor in the dry air, the calibration LED will
 * display for 3 seconds then it will take an average of the air. The
 * calibration light will display again for 3 seconds, move the soil sensor to
 * a cup of water. Once the calibration LED is off it will take the average of
 * the water data. The calibration LED will do a quick flash to let you know
 * it is done.
 *
 * Pins in use:
 * - PB0 -- Error LED
 * - PB1 -- Calibration LED
 * - PB2 -- Button
 * - PB3 -- Potentiometer
 * - PB4 -- Soil Sensor
 *
 */

// pins
#define ERROR_LED PB0
#define CAL_LED PB1
#define BUTTON PB2
#define POTENTIOMETER ADC_SE3
#define SOIL_SENSOR ADC_SE2

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


// global state
static struct state control_state;

/**
 * Handle interrupt mask values atomically.
 *
 * @param[in] state The control state.
 */
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
  setup_button_interrupt(BUTTON);
  // enable interrupts again.
  sei();

  setup_led(ERROR_LED);
  setup_led(CAL_LED);
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
        control_state.calibration_info = calibrate_soil_sensor(CAL_LED, SOIL_SENSOR);
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
        uint16_t result = soil_sensor_read(SOIL_SENSOR);
        uint8_t percentage = percentage_from_value(
          result, &control_state.calibration_info);
        uint16_t threshold = potentiometer_sensor_read(POTENTIOMETER);
        bool triggered = percentage <= threshold;
        if (triggered) {
          control_state.mode = WP_TRIGGER;
        } else {
          control_state.mode = WP_SLEEP;
          // if we detect more moisture disable the error state.
          if (control_state.error_triggered) {
            disable_trigger_state(&control_state, ERROR_LED);
          }
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
        _delay_ms(1000);
        control_state.mode = WP_READ;
#else
        // sleep for longest time -- 8sec
        wd_sleep(WDTO_8S);
#endif
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

