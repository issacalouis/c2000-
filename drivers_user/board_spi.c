/*
 * File: board_spi.c
 * Description: Board-level SPI transport placeholder.
 * Notes: Kept separate so OLED/TFT drivers remain portable across C2000 boards.
 */

#include "board_spi.h"

/*
 * Function: BoardSPI_Init
 * Call period: once if an SPI display is selected.
 * ISR: no.
 * Blocking: no in this template.
 */
void BoardSPI_Init(void)
{
    /* TODO(F280015x): configure SPI pins, FIFO, clock polarity, and baud rate. */
}

/*
 * Function: BoardSPI_Write
 * Call period: low-speed display task only.
 * ISR: no.
 * Blocking: target implementation should be bounded and never called by control ISR.
 */
void BoardSPI_Write(const uint8_t *data, uint16_t len)
{
    (void)data;
    (void)len;
    /* TODO(F280015x): write bytes to SPI FIFO or queue an async transaction. */
}
