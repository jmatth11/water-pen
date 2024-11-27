#include "soil_sensor.h"
#include "driver/adc.h"

uint16_t soil_sensor_read() {
  return adc_read_once(ADC_SE2);
}
