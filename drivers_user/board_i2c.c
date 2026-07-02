/*
 * File: board_i2c.c
 * Description: Portable I2C write stub.
 * Notes: TODO map to C2000 I2C peripheral once the device and pins are fixed.
 */
#include "board_i2c.h"

void BoardI2C_Init(void)
{
    /* TODO: Configure target C2000 I2C peripheral. */
}

uint16_t BoardI2C_Write(uint8_t dev_addr, const uint8_t *data, uint16_t len)
{
    (void)dev_addr;
    (void)data;
    (void)len;

    /* Stub succeeds so HMI can be compiled before hardware is selected. */
    return 0u;
}
