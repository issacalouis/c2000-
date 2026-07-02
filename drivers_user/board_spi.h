/*
 * File: board_spi.h
 * Description: SPI abstraction for OLED/TFT ports.
 * Notes: Use this when migrating from I2C OLED to SPI OLED/TFT screens.
 */
#ifndef BOARD_SPI_H
#define BOARD_SPI_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void BoardSPI_Init(void);
uint16_t BoardSPI_Write(const uint8_t *data, uint16_t len);

#ifdef __cplusplus
}
#endif

#endif
