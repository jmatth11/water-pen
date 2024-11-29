#ifndef WATER_PEN_STATE_TYPES_H
#define WATER_PEN_STATE_TYPES_H

#include "types/calibration_types.h"
#include <stdbool.h>
#include <stdint.h>

/**
 * If sleeping for 8 sec everytime
 * 450 wakeups equals 1 hour.
 */
#define WAKEUP_LIMIT 450

enum mode_t {
  // sleep mode
  WP_SLEEP,
  // start calibration of soil sensor
  WP_CALIBRATE,
  // read the soil sensor and check against potentiometer
  WP_READ,
  // trigger warning LED because moisture is low
  WP_TRIGGER,
};

struct state {
  /* the current mode the state is in. */
  enum mode_t mode;
  /* The wakeup count from watchdog.
   * 450 is 1 hour if sleeping for 8 seconds */
  uint16_t wakeup_count;
  /* Boolean flag for threshold triggered. This is flipped in WP_TRIGGER mode. */
  bool error_triggered;
  /* the calibrated threshold info */
  struct calibration_info calibration_info;
};

#endif
