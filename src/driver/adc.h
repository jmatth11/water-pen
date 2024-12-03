#ifndef WATER_PEN_ADC_H
#define WATER_PEN_ADC_H

#include <avr/io.h>
#include <stdint.h>

/**
 * Enumeration of ADC input channels.
 */
enum adc_input_channel {
  // ADC Single Input options
  /* PB5 */
  ADC_SE0 = 0,
  /* PB2 */
  ADC_SE1,
  /* PB4 */
  ADC_SE2,
  /* PB3 */
  ADC_SE3,
};

/**
 * Initialize the registers for ADC module.
 */
void adc_init();

/**
 * Enable the given input channel for continuous mode.
 * You must call adc_disable to switch input channels
 * or stop the continuous mode.
 *
 * @param[in] input The input channel.
 */
void adc_enable_input_channel(enum adc_input_channel input);

/**
 * Read the value from ADC registers.
 * This functions relies on adc_enable_pin to be called first.
 *
 * @return The uint16 value.
 */
uint16_t adc_read();

/**
 * This function can be called standalone and enables the input channel for
 * single input mode and then grabs the value.
 *
 * @param[in] input The input channel.
 * @return The uint16 value.
 */
uint16_t adc_read_once(enum adc_input_channel input);

/**
 * This function disables and stops continuous updates.
 */
void adc_disable();

#endif
