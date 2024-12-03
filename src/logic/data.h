#ifndef WATER_PEN_DATA_H
#define WATER_PEN_DATA_H

#include <stdbool.h>

// forward declare
struct calibration_info;

/**
 * Save the given calibration info to the EEPROM.
 *
 * @param[in] info The calibration info to save.
 */
void data_save_calibration_info(const struct calibration_info *info);

/**
 * Read and store the calibration info from the EEPROM into the given structure.
 *
 * @param[out] info The calibration info to read into.
 * @return True if valid values were store, False otherwise.
 */
bool data_read_calibration_info(struct calibration_info *info);

/**
 * Check if the values in the given calibration info structure are the default values.
 *
 * @param[in] info The calibration info.
 * @return True if they are the default values, false otherwise.
 */
bool data_are_default_values(struct calibration_info *info);

#endif
