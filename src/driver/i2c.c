#include "i2c.h"
#include <util/delay.h>

// data sheet says between 50 and 300 nanoseconds
// this is 0.2 microseconds which is 200 nanoseconds
#define WAIT 0.2

#define STATUS_CLOCK_8_BITS (_BV(USISIF)|_BV(USIOIF)|_BV(USIPF)|_BV(USIDC) | \
    (0x0 << USICNT0)) // reset clock to allow for full 8 byte transfer

#define STATUS_CLOCK_1_BIT (_BV(USISIF) | \
   _BV(USIOIF) | \
   _BV(USIPF) | \
   _BV(USIDC) | \
   (0xE << USICNT0))// we set the clock to 1110 so it overflows after 1 exchange

/**
 * Initialize I2C registers and ports.
 */
void i2c_init() {
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
    _BV(USICS1) | (0<<USICS0) |
    _BV(USICLK) |
    (0<<USITC)
  );

  i2c_status = (
    // clear all flags
    _BV(USISIF) |
    _BV(USIOIF) |
    _BV(USIPF) |
    _BV(USIDC) |
    // reset overflow counter
    (0x0 << USICNT0)
  );
  // flip the ports to input mode so we can enable pullup resistors on the next line
  i2c_bus &= ~_BV(i2c_sda);
  i2c_bus &= ~_BV(i2c_scl);

  // set both pins to HIGH to enable pullup.
  i2c_port |= _BV(i2c_sda);
  i2c_port |= _BV(i2c_scl);

  // flip the ports to output mode
  i2c_bus |= _BV(i2c_sda);
  i2c_bus |= _BV(i2c_scl);
}

/**
 * Send start command.
 */
bool i2c_start() {
  // ensure both lines are high
  i2c_port |= _BV(i2c_sda);
  i2c_port |= _BV(i2c_scl);
  // wait till clock pin is high
  while (!(i2c_port & _BV(i2c_scl)));
  _delay_us(WAIT);

  // pull data line low
  i2c_port &= ~_BV(i2c_sda);
  // wait some time
  _delay_us(WAIT);
  // pull clock line low
  i2c_port &= ~_BV(i2c_scl);

  // release data line to high
  i2c_port |= _BV(i2c_sda);

  // check for valid start
  if (! (i2c_status & _BV(USISIF))) {
    return false;
  }
  return true;
}

/**
 * Send the stop command.
 */
void i2c_stop() {

  // ensure data line is low
  i2c_port &= ~_BV(i2c_sda);

  // relase clock line to high
  i2c_port |= _BV(i2c_scl);
  // wait for clock pin to read high
  while (!(i2c_pin & _BV(i2c_scl)));
  _delay_us(WAIT);

  // relase data line to high
  i2c_port |= _BV(i2c_sda);
  _delay_us(WAIT);
}

unsigned char transfer(unsigned char mask) {
  i2c_port &= ~_BV(i2c_scl);
  i2c_status = mask;

  unsigned char temp = (0<<USISIE)|(0<<USIOIE)| // Interrupts disabled
           (1<<USIWM1)|(0<<USIWM0)|             // Set USI in Two-wire mode.
           (1<<USICS1)|(0<<USICS0)|(1<<USICLK)| // Software clock strobe as source.
           (1<<USITC); // toggle clock
  do {
    // wait a little bit
    _delay_us(WAIT);
    // toggle clock
    i2c_control = temp;
    // wait for SCL to go high
    while (! (i2c_pin & _BV(i2c_scl)));
    // wait short
    _delay_us(WAIT);
    // toggle clock again
    i2c_control = temp;

  } while (!(i2c_status & _BV(USIOIF)));
  _delay_us(WAIT);

  // clear status
  i2c_status |= _BV(USIOIF);

  unsigned char data = i2c_data;
  i2c_data = 0xff;
  return data;
}

/**
 * Write the given byte.
 *
 * @param[in] data The byte to send.
 * @return The N/ACK byte.
 */
unsigned char i2c_write_byte(unsigned char data) {
  i2c_data = data;
  transfer(STATUS_CLOCK_8_BITS);

  // change data pin to input
  i2c_bus &= ~_BV(i2c_sda);
  unsigned char nack = transfer(STATUS_CLOCK_1_BIT);
  // change back to output
  i2c_bus |= _BV(i2c_sda);
  return nack;
}

/**
 * Read the next byte.
 *
 * @param[in] ack True for reading more, false otherwise.
 * @return The read byte.
 */
unsigned char i2c_read_byte(bool ack) {
  // change data pin to input
  i2c_bus &= ~_BV(i2c_sda);
  unsigned char data = transfer(STATUS_CLOCK_8_BITS);
  // change back to output
  i2c_bus |= _BV(i2c_sda);
  if (ack) {
    // LOW means read another byte
    i2c_data = 0x00;
  } else {
    // HIGH means stop sending
    i2c_data = 0xff;
  }
  // send n/ack
  transfer(STATUS_CLOCK_1_BIT);
  return data;
}

/**
 * Write the target's address out.
 *
 * @param[in] address The target address.
 * @param[in] write Flag for Write or Read bit.
 * @return The N/ACK byte.
 */
unsigned char i2c_write_address(unsigned char address, bool write) {
  unsigned char rw_flag = 1;
  if (write) rw_flag = 0;
  // shift address over 1 because Least sig byte is RW flag
  return i2c_write_byte(((address << 1) | rw_flag));
}
