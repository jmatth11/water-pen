#ifndef WATER_PEN_STATE_TYPES_H
#define WATER_PEN_STATE_TYPES_H

enum mode_t {
  WP_SLEEP,
  WP_CALIBRATE,
  WP_READ,
};

struct state {
  enum mode_t mode;
};

#endif
