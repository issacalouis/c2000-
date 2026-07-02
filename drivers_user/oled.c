/*
 * File: oled.c
 * Description: SSD1306-compatible OLED abstraction with portable text buffer.
 * Notes: TODO replace OLED_Update with actual I2C/SPI page transfer.
 */
#include "oled.h"
#include "board_i2c.h"
#include <stdio.h>
#include <string.h>

#define OLED_TEXT_ROWS 8u
#define OLED_TEXT_COLS 21u
#define OLED_I2C_ADDR  0x3Cu

static char g_oled_text[OLED_TEXT_ROWS][OLED_TEXT_COLS + 1u];

static void OLED_PutText(uint8_t x, uint8_t y, const char *str)
{
    uint8_t col;
    uint8_t idx = 0u;

    if ((y >= OLED_TEXT_ROWS) || (str == (const char *)0))
    {
        return;
    }

    col = (uint8_t)(x / 6u);
    while ((col < OLED_TEXT_COLS) && (str[idx] != '\0'))
    {
        g_oled_text[y][col] = str[idx];
        col++;
        idx++;
    }
}

void OLED_Init(void)
{
    BoardI2C_Init();
    OLED_Clear();
}

void OLED_Clear(void)
{
    uint8_t row;
    uint8_t col;

    for (row = 0u; row < OLED_TEXT_ROWS; row++)
    {
        for (col = 0u; col < OLED_TEXT_COLS; col++)
        {
            g_oled_text[row][col] = ' ';
        }
        g_oled_text[row][OLED_TEXT_COLS] = '\0';
    }
}

void OLED_Update(void)
{
    /*
     * TODO: Convert g_oled_text or a graphic framebuffer into SSD1306 pages
     * and send them through BoardI2C_Write(OLED_I2C_ADDR, data, len).
     */
    (void)OLED_I2C_ADDR;
}

void OLED_ShowString(uint8_t x, uint8_t y, const char *str)
{
    OLED_PutText(x, y, str);
}

void OLED_ShowInt(uint8_t x, uint8_t y, int32_t value)
{
    char text[16];

    (void)snprintf(text, sizeof(text), "%ld", (long)value);
    OLED_PutText(x, y, text);
}

void OLED_ShowFloat(uint8_t x, uint8_t y, float value, uint8_t decimals)
{
    char text[20];

    if (decimals > 3u)
    {
        decimals = 3u;
    }

    (void)snprintf(text, sizeof(text), "%.*f", (int)decimals, (double)value);
    OLED_PutText(x, y, text);
}
