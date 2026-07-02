#ifndef OLED_FONT_H
#define OLED_FONT_H

/*
 * 文件: oled_font.h
 * 说明: 供 SSD1306 像素渲染使用的占位字模表接口。
 * 备注: 当前 OLED 模板使用文本缓存；后续实现真实像素渲染时可使用该字模表。
 */

#include <stdint.h>

extern const uint8_t OLED_Font6x8[1][6];

#endif
