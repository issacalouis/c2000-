#ifndef BOARD_SPI_H
#define BOARD_SPI_H

/*
 * File: board_spi.h
 * Description: Board-level SPI abstraction reserved for TFT/OLED SPI variants.
 * Notes: Current default display is SSD1306 over I2C.
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
