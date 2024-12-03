#include "potentiometer.h"
#include <avr/io.h>
#include "driver/adc.h"
#include "helpers/math.h"

uint16_t potentiometer_sensor_read() {
  uint16_t result = adc_read_once(ADC_SE1);
  result = map(result, 0, 1023, 0, 100);
  return result;
}
