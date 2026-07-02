#ifndef BOARD_SPI_H
#define BOARD_SPI_H

/*
 * 文件: board_spi.h
 * 说明: 预留给 TFT/OLED SPI 变体的板级 SPI 抽象。
 * 备注: 当前默认显示设备为通过 I2C 连接的 SSD1306。
 */

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void BoardSPI_Init(void);
void BoardSPI_Write(const uint8_t *data, uint16_t len);

#ifdef __cplusplus
}
#endif

#endif
