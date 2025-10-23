#ifndef I2C_H
#define I2C_H

#include <avr/io.h>

#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#define I2C_FREQ 100000UL // 100 kHz

void i2c_init(void);
uint8_t i2c_start(void);
void i2c_stop(void);
uint8_t i2c_write(uint8_t data);
uint8_t i2c_read_ack(void);
uint8_t i2c_read_nack(void);

#endif