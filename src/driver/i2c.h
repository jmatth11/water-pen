#ifndef JM_I2C_H
#define JM_I2C_H

#include <avr/io.h>
#include <stdbool.h>

#define I2C_ACK 0x00
#define I2C_NACK 0xFF

// These numbers are from a calculation chart
// from I2C spec -- NXP I2C Timing Specification
#ifdef I2C_FAST_MODE // 400kHz fast
  // low period of SCL
  #define T2_TWI 2 // >1.3microseconds
  // high period of SCL
  #define T4_TWI 1 // >0.6microseconds
#else // 100kHz standard
  // low period of SCL
  #define T2_TWI 5 // >4.7microseconds
  // high period of SCL
  #define T4_TWI 4 // >4.0microseconds
#endif

#ifndef i2c_ddr
  #define i2c_ddr DDRB
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
 *
 * @param[in] internal_pullups Flag for using internal pullup resistors,
 *  False for external.
 */
void i2c_init(bool internal_pullups);

/**
 * Send start command.
 *
 * @return True if the start successfully initiated, False otherwise.
 */
bool i2c_start();

/**
 * Send the stop command.
 */
void i2c_stop();

/**
 * Write the given byte.
 *
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
