/*
 * File: board_gpio.h
 * Description: GPIO abstraction for keys and matrix keyboard lines.
 * Notes: Replace board_gpio.c with real C2000Ware GPIO access for target boards.
 */
#ifndef BOARD_GPIO_H
#define BOARD_GPIO_H

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

void BoardGPIO_SetMatrixRow(uint16_t row, uint16_t active);
uint16_t BoardGPIO_ReadMatrixColumn(uint16_t col);

#if defined(UNIT_TEST)
void BoardGPIO_MockSetIndependent(uint16_t up,
                                  uint16_t down,
                                  uint16_t left,
                                  uint16_t right,
                                  uint16_t ok,
                                  uint16_t back,
                                  uint16_t run);
void BoardGPIO_MockSetMatrix(uint16_t row, uint16_t col, uint16_t pressed);
#endif

#ifdef __cplusplus
}
#endif

#endif
