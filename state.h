#ifndef JM_WATER_PEN_STATE_H
#define JM_WATER_PEN_STATE_H

enum mode_t {
  WP_SLEEP,
  WP_CALIBRATE,
  WP_READ,
};

struct state {
  enum mode_t mode;
};

#endif
