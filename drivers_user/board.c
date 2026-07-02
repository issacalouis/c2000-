/*
 * File: board.c
 * Description: Unified board initialization for LAUNCHXL-F2800157 HMI hardware.
 */

#include "board.h"

#include "board_gpio.h"
#include "board_i2c.h"
#include "key.h"
#include "oled.h"

#ifndef UNIT_TEST
#include "device.h"
#endif

void Board_Init(void)
{
#ifndef UNIT_TEST
    Device_init();
    Device_initGPIO();
#endif

    BoardGPIO_Init();
    BoardI2C_Init();
    Key_Init();
    OLED_Init();
}
