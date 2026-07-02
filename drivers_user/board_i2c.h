#ifndef BOARD_I2C_H
#define BOARD_I2C_H

/*
 * 文件: board_i2c.h
 * 说明: 面向 OLED 或其他外设的板级 I2C 写入抽象。
 * 备注: HMI 层不应直接访问 C2000 的 I2C 寄存器。
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
