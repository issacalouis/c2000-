#ifndef OLED_H
#define OLED_H

/*
 * File: oled.h
 * Description: SSD1306-style OLED abstraction used by HMI display rendering.
 * Notes: Default target is 128x64 over I2C. Transport details stay in board_i2c.
 */

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void OLED_Init(void);
void OLED_Clear(void);
void OLED_Update(void);

void OLED_ShowString(uint8_t x, uint8_t y, const char *str);
void OLED_ShowInt(uint8_t x, uint8_t y, int32_t value);
void OLED_ShowFloat(uint8_t x, uint8_t y, float value, uint8_t decimals);

#ifdef UNIT_TEST
const char *OLED_MockGetLine(uint8_t y);
#endif

#ifdef __cplusplus
}
#endif

#endif
