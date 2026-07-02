/*
 * File: board_spi.c
 * Description: Portable SPI write stub.
 * Notes: TODO map to C2000 SPI peripheral once the display is fixed.
 */
#include "board_spi.h"

void BoardSPI_Init(void)
{
    /* TODO: Configure target C2000 SPI peripheral. */
}

uint16_t BoardSPI_Write(const uint8_t *data, uint16_t len)
{
    (void)data;
    (void)len;

    /* Stub succeeds so HMI can be compiled before hardware is selected. */
    return 0u;
}
