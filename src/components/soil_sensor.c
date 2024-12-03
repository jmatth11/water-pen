#include "soil_sensor.h"
#include <avr/io.h>
#include "driver/adc.h"

uint16_t soil_sensor_read(enum adc_input_channel channel) {
  return adc_read_once(channel);
}
