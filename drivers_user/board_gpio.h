#ifndef BOARD_GPIO_H
#define BOARD_GPIO_H

/*
 * File: board_gpio.h
 * Description: Board-level GPIO abstraction for keys and matrix keyboard rows.
 * Notes: Replace the TODO stubs in board_gpio.c with C2000Ware GPIO calls.
 */

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void BoardGPIO_Init(void);

uint16_t BoardGPIO_ReadKeyUp(void);
uint16_t BoardGPIO_ReadKeyDown(void);
uint16_t BoardGPIO_ReadKeyLeft(void);
uint16_t BoardGPIO_ReadKeyRight(void);
uint16_t BoardGPIO_ReadKeyOk(void);
uint16_t BoardGPIO_ReadKeyBack(void);
uint16_t BoardGPIO_ReadKeyRun(void);

void BoardGPIO_SetMatrixRow(uint8_t row);
uint16_t BoardGPIO_ReadMatrixColumn(uint8_t column);

#ifdef UNIT_TEST
void BoardGPIO_MockSetIndependentMask(uint16_t mask);
void BoardGPIO_MockSetMatrixMask(uint16_t mask);
#endif

#ifdef __cplusplus
}
#endif

#endif
