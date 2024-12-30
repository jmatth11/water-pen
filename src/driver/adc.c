#include "adc.h"

static void enable_inputs(enum adc_input_channel input) {
  ADMUX = (
    // set refs to (0,0,0) to use Vcc as Voltage Reference
    (0 << REFS1) | (0 << REFS0) | (0 << REFS2) |
    // Set 0 to read bit offset 9 and 8 in ADCH and the rest (0-7) in ADCL
    (0 << ADLAR) |
    // set the bottom 4 bits to configure PBx as ADC input
    (0b00001111 & input)
  );
  // set everything to zero.
  // The bottom three bits to zero means Free Running Mode
  ADCSRB = 0x0;
}

void adc_init() {
  // enable ADC module
  ADCSRA |= _BV(ADEN);
}

void adc_enable_input_channel(enum adc_input_channel input) {
  enable_inputs(input);
  ADCSRA |= (
    // Start conversion
    _BV(ADSC) |
    // Auto Trigger Enabled
    _BV(ADATE)
  );
}

uint16_t adc_read() {
  // wait until conversion is done
  while(!(ADCSRA & _BV(ADIF)));
  // read low byte first
  uint16_t adc_l = ADCL;
  // then combine with high byte
  uint16_t result = (ADCH<<8)|adc_l;
  // clear the interrupt flag
  ADCSRA |= _BV(ADIF);
  return result;
}

uint16_t adc_read_once(enum adc_input_channel input) {
  enable_inputs(input);
  // Start conversion
  // With auto trigger disabled this bit will flip back to off
  // on it's own after one conversion.
  ADCSRA |= _BV(ADSC);
  return adc_read();
}

void adc_disable() {
  // clear start and auto toggle
  ADCSRA &= ~_BV(ADSC);
  ADCSRA &= ~_BV(ADATE);
}
