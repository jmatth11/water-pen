#ifndef WATER_PEN_SLEEP_H
#define WATER_PEN_SLEEP_H

#include <avr/sleep.h>
#include <avr/wdt.h>
#include <stdint.h>

/**
 * Enables sleep flag and sets it to power-down on sleep.
 */
void enable_deep_sleep();

/**
 * Use watchdog sleep to sleep by the specified prescaler.
 * Handles disabling ADC and setting interrupt flags,
 * then renabling ADC after sleep.
 *
 * @param[in] prescaler The watchdog prescaler value.
 *  \reference WDTO_x for prescaler values
 */
void wd_sleep(uint8_t prescaler);

#endif
