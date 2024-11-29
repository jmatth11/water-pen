#include "state.h"
#include <avr/io.h>


void enable_trigger_state(struct state* control_state, uint8_t pin) {
  PORTB |= _BV(pin);
  control_state->error_triggered = true;
}
void disable_trigger_state(struct state* control_state, uint8_t pin) {
  PORTB &= ~_BV(pin);
  control_state->error_triggered = false;
}

void wakeup_operations(struct state* control_state) {
  if (control_state->error_triggered) {
    // if error is triggered we want to READ every time we wake
    control_state->mode = WP_READ;
    return;
  }
  ++control_state->wakeup_count;
  if (control_state->wakeup_count >= WAKEUP_LIMIT) {
    control_state->wakeup_count = 0;
    control_state->mode = WP_READ;
  }
}
