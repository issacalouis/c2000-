/*
 * 文件: oled.c
 * 说明: 带 SSD1306 I2C 传输钩子的文本模式 OLED 抽象。
 * 备注: 本模板维护 8 行文本缓存。待显示硬件方案确定后，
 *       请将 OLED_Update() 替换为真实的帧缓冲传输实现。
 */

#include "oled.h"

#include <stdio.h>
#include <string.h>

#include "board_i2c.h"

#define OLED_I2C_ADDR       0x3Cu
#define OLED_TEXT_ROWS      8u
#define OLED_TEXT_COLUMNS   21u
#define OLED_TEXT_LINE_SIZE (OLED_TEXT_COLUMNS + 1u)

static char g_oled_text[OLED_TEXT_ROWS][OLED_TEXT_LINE_SIZE];

static void OLED_WriteText(uint8_t x, uint8_t y, const char *str)
{
    uint8_t column;
    uint8_t index;

    if ((y >= OLED_TEXT_ROWS) || (str == 0))
    {
        return;
    }

    column = (uint8_t)(x / 6u);
    if (column >= OLED_TEXT_COLUMNS)
    {
        return;
    }

    index = 0u;
    while ((str[index] != '\0') && ((uint8_t)(column + index) < OLED_TEXT_COLUMNS))
    {
        g_oled_text[y][column + index] = str[index];
        index++;
    }
}

/*
 * 函数: OLED_Init
 * 调用周期: 在 HMI_Init() 中调用一次。
 * ISR: 否。
 * 阻塞: 在本模板中为否。
 */
void OLED_Init(void)
{
    BoardI2C_Init();
    OLED_Clear();
    /*
     * TODO(SSD1306): 通过 I2C 发送 128x64 显示器初始化命令序列。
     * 该流程应放在控制 ISR 之外，且任何等待时间都必须有上界。
     */
}

/*
 * 函数: OLED_Clear
 * 调用周期: 在渲染页面前调用。
 * ISR: 否。
 * 阻塞: 否。
 */
void OLED_Clear(void)
{
    uint8_t row;

    for (row = 0u; row < OLED_TEXT_ROWS; row++)
    {
        memset(g_oled_text[row], ' ', OLED_TEXT_COLUMNS);
        g_oled_text[row][OLED_TEXT_COLUMNS] = '\0';
    }
}

/*
 * 函数: OLED_Update
 * 调用周期: 100 ms 显示任务中调用。
 * ISR: 否。
 * 阻塞: 目标实现应为有界执行时间或异步方式。
 */
void OLED_Update(void)
{
    /*
     * TODO(SSD1306): 将文本缓存转换为像素数据，并调用 BoardI2C_Write()。
     * 下面的调用只是无害的占位实现，用于在早期集成构建中保留传输符号链接。
     */
    BoardI2C_Write(OLED_I2C_ADDR, (const uint8_t *)0, 0u);
}

void OLED_ShowString(uint8_t x, uint8_t y, const char *str)
{
    OLED_WriteText(x, y, str);
}

void OLED_ShowInt(uint8_t x, uint8_t y, int32_t value)
{
    char text[16];

    (void)snprintf(text, sizeof(text), "%ld", (long)value);
    OLED_WriteText(x, y, text);
}

void OLED_ShowFloat(uint8_t x, uint8_t y, float value, uint8_t decimals)
{
    char text[20];

    if (decimals > 3u)
    {
        decimals = 3u;
    }

    (void)snprintf(text, sizeof(text), "%.*f", (int)decimals, (double)value);
    OLED_WriteText(x, y, text);
}

#ifdef UNIT_TEST
const char *OLED_MockGetLine(uint8_t y)
{
    if (y >= OLED_TEXT_ROWS)
    {
        return "";
    }

    return g_oled_text[y];
}
#endif
