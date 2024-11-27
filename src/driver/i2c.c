#include "i2c.h"
#include <util/delay.h>

// data sheet says sampling time is between 50 and 300 nanoseconds
// this is 0.2 microseconds which is 200 nanoseconds
#define SAMPLING_WAIT 0.2

#define STATUS_CLOCK_8_BITS (_BV(USISIF)|_BV(USIOIF)|_BV(USIPF)|_BV(USIDC) | \
    (0x0 << USICNT0)) // reset clock to allow for full 8 bit transfer

#define STATUS_CLOCK_1_BIT (_BV(USISIF) | \
   _BV(USIOIF) | \
   _BV(USIPF) | \
   _BV(USIDC) | \
   (0xE << USICNT0))// we set the clock to 1110 so it overflows after 1 exchange

void i2c_init(bool internal_pullups) {
  // preload data register with default HIGH
  i2c_data = 0xff;

  // setup for master
  i2c_control = (
    // disable Start condition interrupt
    (0 << USISIE) |
    // disable overflow interrupt
    (0 << USIOIE) |
    // only set WM1 to high for normal 2wire mode
    _BV(USIWM1) | (0<<USIWM0) |
    // set CS1 and CLK to high to use external clock source
    // with positive edge. Software Clock Strobe (with USITC register)
    _BV(USICS1) | (0<<USICS0) | _BV(USICLK) |
    (0<<USITC)
  );

  i2c_status = (
    // clear all flags
    _BV(USISIF) | _BV(USIOIF) | _BV(USIPF) | _BV(USIDC) |
    // reset overflow counter
    (0x0 << USICNT0)
  );

  if (internal_pullups) {
    // flip the ports to input mode so we can enable pullup resistors on the next line
    i2c_ddr &= ~_BV(i2c_sda);
    i2c_ddr &= ~_BV(i2c_scl);

    // set both pins to HIGH to enable pullup.
    i2c_port |= (_BV(i2c_sda) | _BV(i2c_scl));
  }

  // flip the ports to output mode
  i2c_ddr |= (_BV(i2c_sda) | _BV(i2c_scl));
}

bool i2c_start() {
  // ensure both lines are high
  i2c_port |= (_BV(i2c_sda) | _BV(i2c_scl));
  // wait till clock pin is high
  while (!(i2c_port & _BV(i2c_scl)));
  _delay_us(T2_TWI);

  // pull data line low
  i2c_port &= ~_BV(i2c_sda);
  // this is sampling time for the attiny85 to recognize the start command
  _delay_us(SAMPLING_WAIT);
  // pull clock line low
  i2c_port &= ~_BV(i2c_scl);

  // release data line to high
  i2c_port |= _BV(i2c_sda);

  // check for valid start
  return (i2c_status & _BV(USISIF));
}

void i2c_stop() {

  // ensure data line is low
  i2c_port &= ~_BV(i2c_sda);

  // relase clock line to high
  i2c_port |= _BV(i2c_scl);
  // wait for clock pin to read high
  while (!(i2c_pin & _BV(i2c_scl)));
  _delay_us(T4_TWI);

  // relase data line to high
  i2c_port |= _BV(i2c_sda);
  _delay_us(T2_TWI);
}

unsigned char transfer(unsigned char mask) {
  // ensure clock line is low
  i2c_port &= ~_BV(i2c_scl);
  i2c_status = mask;

  do {
    // wait a little bit
    _delay_us(T2_TWI);
    // toggle clock
    i2c_control |= _BV(USITC);
    // wait for SCL to go high
    while (! (i2c_pin & _BV(i2c_scl)));
    // wait short
    _delay_us(T4_TWI);
    // toggle clock again
    i2c_control |= _BV(USITC);

  } while (!(i2c_status & _BV(USIOIF)));
  _delay_us(T2_TWI);

  // clear counter overflow status
  i2c_status |= _BV(USIOIF);

  unsigned char data = i2c_data;
  i2c_data = 0xff;
  return data;
}

unsigned char i2c_write_byte(unsigned char data) {
  i2c_data = data;
  transfer(STATUS_CLOCK_8_BITS);

  // change data pin to input
  i2c_ddr &= ~_BV(i2c_sda);
  unsigned char nack = transfer(STATUS_CLOCK_1_BIT);
  // change back to output
  i2c_ddr |= _BV(i2c_sda);
  return nack;
}

unsigned char i2c_read_byte(bool ack) {
  // HIGH value means stop sending
  unsigned char response = I2C_NACK;
  if (ack) {
    // LOW means read another byte
    response = I2C_ACK;
  }
  // change data pin to input
  i2c_ddr &= ~_BV(i2c_sda);
  unsigned char data = transfer(STATUS_CLOCK_8_BITS);
  // change back to output
  i2c_ddr |= _BV(i2c_sda);
  // send n/ack
  i2c_data = response;
  transfer(STATUS_CLOCK_1_BIT);
  return data;
}

unsigned char i2c_write_address(unsigned char address, bool write) {
  unsigned char rw_flag = 1;
  if (write) rw_flag = 0;
  // shift address over 1 because Least sig bit is RW flag
  return i2c_write_byte(((address << 1) | rw_flag));
}
