#include "slp.h"
#include "avr/interrupt.h"
#include "avr/wdt.h"

void enable_deep_sleep() {
  sleep_enable();
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
}

void wd_sleep(uint8_t prescaler) {
  // reset the "reset trigger" flags
  MCUSR = 0;
  // turn of the ADC to save more on power consumption.
  ADCSRA &= ~_BV(ADEN);
  cli();
  // enable watchdog timer with the given prescaler.
  wdt_enable(prescaler);
  WDTCR |= (
    // enable wd interrupt
    _BV(WDIE) |
    // clear out any interrupt flag
    _BV(WDIF)
  );
  sei();
  // put microcontroller to sleep.
  // The watchdog interrupt will wake us up back here.
  sleep_cpu();
  // disable because next interrupt will lead to a reset.
  wdt_disable();
  // turn ADC back on
  ADCSRA |= _BV(ADEN);
}
