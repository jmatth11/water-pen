#ifndef WATER_PEN_STATE_H
#define WATER_PEN_STATE_H

#include "types/state_types.h"
#include <stdint.h>

/**
 * Enables the trigger state.
 * This function turns on the LED pin and sets the error trigger flag to true.
 *
 * @param[in] control_state The main control state.
 * @param[in] pin The LED pin.
 */
void enable_trigger_state(struct state* control_state, uint8_t pin);
/**
 * Disables the trigger state.
 * This function turns off the LED pin and sets the error trigger flag to false.
 *
 * @param[in] control_state The main control state.
 * @param[in] pin The LED pin.
 */
void disable_trigger_state(struct state* control_state, uint8_t pin);

/**
 * Executes operations on wakeup.
 * Checks error state to see if we need to continually read.
 * If not in error state, we increment wakeup counter until the max wait time
 *  and then puts the controller in READ state.
 *
 * @param[in] control_state The main control state.
 */
void wakeup_operations(struct state* control_state);

#endif
