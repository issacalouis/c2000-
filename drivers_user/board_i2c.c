/*
 * File: board_i2c.c
 * Description: Board-level I2C transport placeholder.
 * Notes: Replace stubs with non-blocking or bounded-time C2000Ware I2C code.
 */

#include "board_i2c.h"

/*
 * Function: BoardI2C_Init
 * Call period: once during OLED_Init().
 * ISR: no.
 * Blocking: no in this template.
 */
void BoardI2C_Init(void)
{
    /* TODO(F280015x): configure I2C module, pins, bitrate, and interrupts/DMA if used. */
}

/*
 * Function: BoardI2C_Write
 * Call period: OLED_Update() in the 100 ms display task.
 * ISR: no.
 * Blocking: target implementation should be bounded and must not run in control ISR.
 */
void BoardI2C_Write(uint8_t dev_addr, const uint8_t *data, uint16_t len)
{
    (void)dev_addr;
    (void)data;
    (void)len;
    /* TODO(F280015x): write bytes to I2C FIFO or queue an async transaction. */
}
