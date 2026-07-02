/*
 * File: oled.h
 * Description: Abstract SSD1306-style OLED drawing API.
 * Notes: OLED_Update is intentionally separated from drawing calls.
 */
#ifndef OLED_H
#define OLED_H

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

#ifdef __cplusplus
}
#endif

#endif
