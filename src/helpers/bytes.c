#include "bytes.h"
#include <stdint.h>

void write_16_to_8(uint16_t value, uint8_t buf[2]) {
  buf[0] = value;
  buf[1] = (((uint16_t)value) >> 8);
}

void read_8_to_16(uint8_t buf[2], uint16_t* value) {
  *value = (((uint16_t)buf[1])<<8) | buf[0];
}
