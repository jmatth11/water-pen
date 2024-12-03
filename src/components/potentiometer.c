#include "potentiometer.h"
#include <avr/io.h>
#include "helpers/math.h"

uint16_t potentiometer_sensor_read(enum adc_input_channel channel) {
  uint16_t result = adc_read_once(channel);
  result = map(result, 0, 1023, 0, 100);
  return result;
}
