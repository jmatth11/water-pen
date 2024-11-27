#ifndef WATER_PEN_STATE_TYPES_H
#define WATER_PEN_STATE_TYPES_H

#include <stdint.h>

/**
 * If sleeping for 8 sec everytime
 * 450 wakeups equals 1 hour.
 */
#define WAKEUP_LIMIT 450

enum mode_t {
  WP_SLEEP,
  WP_CALIBRATE,
  WP_READ,
};

struct state {
  /* the current mode the state is in. */
  enum mode_t mode;
  /* The wakeup count from watchdog.
   * 450 is 1 hour if sleeping for 8 seconds */
  uint16_t wakeup_count;
};

#endif
