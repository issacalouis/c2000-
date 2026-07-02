/*
 * File: board_i2c.h
 * Description: I2C abstraction used by OLED or other display drivers.
 * Notes: Keep C2000Ware I2C register code inside board_i2c.c.
 */
#ifndef BOARD_I2C_H
#define BOARD_I2C_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void BoardI2C_Init(void);
uint16_t BoardI2C_Write(uint8_t dev_addr, const uint8_t *data, uint16_t len);

#ifdef __cplusplus
}
#endif

#endif
