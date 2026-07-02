#ifndef BOARD_GPIO_H
#define BOARD_GPIO_H

/*
 * 文件: board_gpio.h
 * 说明: 面向按键与矩阵键盘行线的板级 GPIO 抽象。
 * 备注: 需要用 C2000Ware 的 GPIO 调用替换 board_gpio.c 中的 TODO 桩代码。
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
