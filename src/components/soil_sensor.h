#ifndef WATER_PEN_SOIl_SENSOR_H
#define WATER_PEN_SOIl_SENSOR_H

#include <stdint.h>
#include "driver/adc.h"

/**
 * Read the soil sensor on the given channel.
 *
 * @param[in] channel The ADC input channel.
 * \reference ADC_SE-
 * @return Raw soil sensor data.
 */
uint16_t soil_sensor_read(enum adc_input_channel channel);

#endif
