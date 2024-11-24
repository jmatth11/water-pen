#ifndef WATER_PEN_CALIBRATION_TYPES_H
#define WATER_PEN_CALIBRATION_TYPES_H

#include <inttypes.h>

/**
 * Structure to contain the thresholds for our soil sensor.
 */
struct calibration_info {
  /* This is the "high" level of our sensor range. */
  uint16_t water_threshold;
  /* This is the "low" level of our sensor range. */
  uint16_t air_threshold;
};

#endif
