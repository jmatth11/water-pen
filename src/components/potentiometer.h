#ifndef WATER_PEN_POTENTIOMETER_H
#define WATER_PEN_POTENTIOMETER_H

#include <stdint.h>
#include "driver/adc.h"

/**
 * Read the potentiometer sensor value on the given channel.
 *
 * @param[in] channel The ADC channel.
 * \reference ADC_SE-
 * @return The ADC value.
 */
uint16_t potentiometer_sensor_read(enum adc_input_channel channel);

#endif
