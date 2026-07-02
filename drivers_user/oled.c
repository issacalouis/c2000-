/*
 * File: oled.c
 * Description: Text-mode OLED abstraction with SSD1306 I2C transport hook.
 * Notes: This template stores 8 text rows. Replace OLED_Update() with a real
 *        framebuffer transfer when the display hardware is finalized.
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
 * Function: OLED_Init
 * Call period: once from HMI_Init().
 * ISR: no.
 * Blocking: no in this template.
 */
void OLED_Init(void)
{
    BoardI2C_Init();
    OLED_Clear();
    /*
     * TODO(SSD1306): send init command sequence for 128x64 display over I2C.
     * Keep this outside the control ISR and keep any wait bounded.
     */
}

/*
 * Function: OLED_Clear
 * Call period: before rendering a page.
 * ISR: no.
 * Blocking: no.
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
 * Function: OLED_Update
 * Call period: 100 ms display task.
 * ISR: no.
 * Blocking: target implementation should be bounded or asynchronous.
 */
void OLED_Update(void)
{
    /*
     * TODO(SSD1306): convert text buffer to pixels and call BoardI2C_Write().
     * The call below is a harmless placeholder so the transport symbol remains
     * linked in early integration builds.
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
