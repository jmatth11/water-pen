#ifndef JM_I2C_H
#define JM_I2C_H

#include <avr/io.h>
#include <stdbool.h>

#ifndef i2c_bus
  #define i2c_bus DDRB
#endif
#ifndef i2c_port
  #define i2c_port PORTB
#endif
#ifndef i2c_pin
  #define i2c_pin PINB
#endif
#ifndef i2c_scl
  #define i2c_scl PORTB2
#endif
#ifndef i2c_sda
  #define i2c_sda PORTB0
#endif
#ifndef i2c_data
  #define i2c_data USIDR
#endif
#ifndef i2c_status
  #define i2c_status USISR
#endif
#ifndef i2c_control
  #define i2c_control USICR
#endif

/**
 * Initialize I2C registers and ports.
 */
void i2c_init();

/**
 * Send start command.
 */
bool i2c_start();

/**
 * Send the stop command.
 */
void i2c_stop();

/**
 * Write the given byte.
 * @param[in] data The byte to send.
 * @return The N/ACK byte.
 */
unsigned char i2c_write_byte(unsigned char data);

/**
 * Read the next byte.
 * @param[in] nack True for reading more, false otherwise.
 * @return The read byte.
 */
unsigned char i2c_read_byte(bool nack);

/**
 * Write the target's address out.
 *
 * @param[in] address The target address.
 * @param[in] write Flag for Write or Read bit.
 * @return The N/ACK byte.
 */
unsigned char i2c_write_address(unsigned char address, bool write);

#endif
