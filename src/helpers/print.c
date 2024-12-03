#include "print.h"

#include <stdio.h>
#include <string.h>
#include "driver/i2c.h"

// DEBUG FUNCTIONS
static void error_loop() {
  PORTB |= _BV(PRINT_ERROR_PIN);
  while(1) {

  }
}

void print(const char* str) {
  const uint32_t len = strlen(str);
  if (!i2c_start()) {
    error_loop();
  }
  if (i2c_write_address(4, true) & 0x01) error_loop();
  for (int i = 0; i < len; ++i) {
    if (i2c_write_byte(str[i]) & 0x01) error_loop();
  }
  i2c_stop();
}

void print_int(const uint16_t val) {
  char buf[5] = {0,0,0,0,0};
  sprintf(buf, "%u", val);
  print(buf);
}
