#ifndef BOARD_I2C_H
#define BOARD_I2C_H

/*
 * File: board_i2c.h
 * Description: Board-level I2C write abstraction for OLED or other peripherals.
 * Notes: The HMI layer must not call C2000 I2C registers directly.
 */

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void BoardI2C_Init(void);
void BoardI2C_Write(uint8_t dev_addr, const uint8_t *data, uint16_t len);

#ifdef __cplusplus
}
#endif

#endif
