#ifndef WATER_PEN_LOGIC_CALIBRATION_H
#define WATER_PEN_LOGIC_CALIBRATION_H

#include "types/calibration_types.h"
#include "driver/adc.h"
#include <stdint.h>

/**
 * Calibrate sensor soil by averaging values over a short time
 * for an air and water threshold.
 * We use the given LED pin to instruct when to start and switch
 * between air and water measureing.
 *
 * @param[in] led_pin The LED pin to notify the user of changing state.
 * @param[in] soil_channel The ADC channel the soil sensor is on.
 * @return Calibration info, water and air threshold values.
 */
struct calibration_info calibrate_soil_sensor(uint8_t led_pin, enum adc_input_channel soil_channel);

/**
 * Get the percentage of the target value given the calibration info.
 *
 * @param[in] value The target value.
 * @param[in] info The calibration info of the soil sensor.
 * @return The percentage value (0-100).
 */
uint8_t percentage_from_value(const uint16_t value, const struct calibration_info *info);

#endif

