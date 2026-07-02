#ifndef OLED_H
#define OLED_H

/*
 * 文件: oled.h
 * 说明: 供 HMI 显示渲染使用的 SSD1306 风格 OLED 抽象。
 * 备注: 默认目标为通过 I2C 连接的 128x64 OLED，传输细节保留在 board_i2c 中。
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
