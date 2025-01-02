#include "calibration.h"
#include <avr/io.h>
#include "components/soil_sensor.h"
#include "driver/adc.h"
#include "helpers/math.h"
#include "util/delay.h"
#include <stdint.h>

static uint16_t average(enum adc_input_channel soil_channel, uint8_t cap) {
  uint32_t sum = 0;
  for (int i = 0; i < cap; ++i) {
    sum += soil_sensor_read(soil_channel);
    _delay_ms(500);
  }
  return sum/cap;
}

struct calibration_info calibrate_soil_sensor(uint8_t led_pin, enum adc_input_channel soil_channel) {
  struct calibration_info result;
  PORTB |= _BV(led_pin);
  _delay_ms(3000);
  PORTB &= ~_BV(led_pin);

  result.air_threshold = average(soil_channel, 5);

  PORTB |= _BV(led_pin);
  _delay_ms(3000);
  PORTB &= ~_BV(led_pin);

  result.water_threshold = average(soil_channel, 5);

  // quickly flash the LED to signal the end
  PORTB |= _BV(led_pin);
  _delay_ms(500);
  PORTB &= ~_BV(led_pin);
  return result;
}

uint8_t percentage_from_value(const uint16_t value, const struct calibration_info *info) {
  return map(
    value,
    info->air_threshold,
    info->water_threshold,
    0,
    100
  );
}
