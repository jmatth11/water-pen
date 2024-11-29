#include "data.h"
#include "driver/eeprom.h"
#include <stdint.h>
#include "helpers/bytes.h"
#include "types/calibration_types.h"

#define ADDR_AIR_LOW 0
#define ADDR_AIR_HIGH 1
#define ADDR_WATER_LOW 2
#define ADDR_WATER_HIGH 3

void data_save_calibration_info(const struct calibration_info *info) {
  uint8_t buf[2];

  write_16_to_8(info->air_threshold, buf);
  // store in little endian format
  eeprom_write(ADDR_AIR_LOW, buf[0]);
  eeprom_write(ADDR_AIR_HIGH, buf[1]);

  write_16_to_8(info->water_threshold, buf);
  // store in little endian format
  eeprom_write(ADDR_WATER_LOW, buf[0]);
  eeprom_write(ADDR_WATER_HIGH, buf[1]);
}

bool data_read_calibration_info(struct calibration_info *info) {
  uint8_t buf[2];
  // read in little endian format
  buf[0] = eeprom_read(ADDR_AIR_LOW);
  buf[1] = eeprom_read(ADDR_AIR_HIGH);
  read_8_to_16(buf, &info->air_threshold);

  // read in little endian format
  buf[0] = eeprom_read(ADDR_WATER_LOW);
  buf[1] = eeprom_read(ADDR_WATER_HIGH);
  read_8_to_16(buf, &info->water_threshold);

  // 0xFF is the default value in the EEPROM.
  // Since we are grabbing a uint16_t from the EEPROM we need to check for 0xFFFF
  return info->air_threshold != 0xFFFF && info->water_threshold != 0xFFFF;
}
