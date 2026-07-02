/*
 * File: oled.c
 * Description: SSD1306 128x64 OLED text renderer over I2C for LAUNCHXL-F2800157.
 */

#include "oled.h"

#include <stdio.h>
#include <string.h>

#include "board_i2c.h"
#include "board_pinmap.h"

#define OLED_CONTROL_CMD    0x00u
#define OLED_CONTROL_DATA   0x40u
#define OLED_TEXT_ROWS      8u
#define OLED_TEXT_COLUMNS   21u
#define OLED_TEXT_LINE_SIZE (OLED_TEXT_COLUMNS + 1u)
#define OLED_FONT_WIDTH     5u
#define OLED_CELL_WIDTH     6u

static char g_oled_text[OLED_TEXT_ROWS][OLED_TEXT_LINE_SIZE];
static uint8_t g_oled_framebuffer[BOARD_OLED_BUF_SIZE];

static const uint8_t *OLED_Font5x7(char c)
{
    static const uint8_t blank[5] = {0x00u, 0x00u, 0x00u, 0x00u, 0x00u};
    static const uint8_t font_0[5] = {0x3Eu, 0x51u, 0x49u, 0x45u, 0x3Eu};
    static const uint8_t font_1[5] = {0x00u, 0x42u, 0x7Fu, 0x40u, 0x00u};
    static const uint8_t font_2[5] = {0x42u, 0x61u, 0x51u, 0x49u, 0x46u};
    static const uint8_t font_3[5] = {0x21u, 0x41u, 0x45u, 0x4Bu, 0x31u};
    static const uint8_t font_4[5] = {0x18u, 0x14u, 0x12u, 0x7Fu, 0x10u};
    static const uint8_t font_5[5] = {0x27u, 0x45u, 0x45u, 0x45u, 0x39u};
    static const uint8_t font_6[5] = {0x3Cu, 0x4Au, 0x49u, 0x49u, 0x30u};
    static const uint8_t font_7[5] = {0x01u, 0x71u, 0x09u, 0x05u, 0x03u};
    static const uint8_t font_8[5] = {0x36u, 0x49u, 0x49u, 0x49u, 0x36u};
    static const uint8_t font_9[5] = {0x06u, 0x49u, 0x49u, 0x29u, 0x1Eu};
    static const uint8_t font_A[5] = {0x7Eu, 0x11u, 0x11u, 0x11u, 0x7Eu};
    static const uint8_t font_B[5] = {0x7Fu, 0x49u, 0x49u, 0x49u, 0x36u};
    static const uint8_t font_C[5] = {0x3Eu, 0x41u, 0x41u, 0x41u, 0x22u};
    static const uint8_t font_D[5] = {0x7Fu, 0x41u, 0x41u, 0x22u, 0x1Cu};
    static const uint8_t font_E[5] = {0x7Fu, 0x49u, 0x49u, 0x49u, 0x41u};
    static const uint8_t font_F[5] = {0x7Fu, 0x09u, 0x09u, 0x09u, 0x01u};
    static const uint8_t font_G[5] = {0x3Eu, 0x41u, 0x49u, 0x49u, 0x7Au};
    static const uint8_t font_H[5] = {0x7Fu, 0x08u, 0x08u, 0x08u, 0x7Fu};
    static const uint8_t font_I[5] = {0x00u, 0x41u, 0x7Fu, 0x41u, 0x00u};
    static const uint8_t font_J[5] = {0x20u, 0x40u, 0x41u, 0x3Fu, 0x01u};
    static const uint8_t font_K[5] = {0x7Fu, 0x08u, 0x14u, 0x22u, 0x41u};
    static const uint8_t font_L[5] = {0x7Fu, 0x40u, 0x40u, 0x40u, 0x40u};
    static const uint8_t font_M[5] = {0x7Fu, 0x02u, 0x0Cu, 0x02u, 0x7Fu};
    static const uint8_t font_N[5] = {0x7Fu, 0x04u, 0x08u, 0x10u, 0x7Fu};
    static const uint8_t font_O[5] = {0x3Eu, 0x41u, 0x41u, 0x41u, 0x3Eu};
    static const uint8_t font_P[5] = {0x7Fu, 0x09u, 0x09u, 0x09u, 0x06u};
    static const uint8_t font_Q[5] = {0x3Eu, 0x41u, 0x51u, 0x21u, 0x5Eu};
    static const uint8_t font_R[5] = {0x7Fu, 0x09u, 0x19u, 0x29u, 0x46u};
    static const uint8_t font_S[5] = {0x46u, 0x49u, 0x49u, 0x49u, 0x31u};
    static const uint8_t font_T[5] = {0x01u, 0x01u, 0x7Fu, 0x01u, 0x01u};
    static const uint8_t font_U[5] = {0x3Fu, 0x40u, 0x40u, 0x40u, 0x3Fu};
    static const uint8_t font_V[5] = {0x1Fu, 0x20u, 0x40u, 0x20u, 0x1Fu};
    static const uint8_t font_W[5] = {0x7Fu, 0x20u, 0x18u, 0x20u, 0x7Fu};
    static const uint8_t font_X[5] = {0x63u, 0x14u, 0x08u, 0x14u, 0x63u};
    static const uint8_t font_Y[5] = {0x07u, 0x08u, 0x70u, 0x08u, 0x07u};
    static const uint8_t font_Z[5] = {0x61u, 0x51u, 0x49u, 0x45u, 0x43u};
    static const uint8_t colon[5] = {0x00u, 0x36u, 0x36u, 0x00u, 0x00u};
    static const uint8_t dot[5] = {0x00u, 0x60u, 0x60u, 0x00u, 0x00u};
    static const uint8_t slash[5] = {0x20u, 0x10u, 0x08u, 0x04u, 0x02u};
    static const uint8_t percent[5] = {0x23u, 0x13u, 0x08u, 0x64u, 0x62u};
    static const uint8_t minus[5] = {0x08u, 0x08u, 0x08u, 0x08u, 0x08u};

    if ((c >= 'a') && (c <= 'z'))
    {
        c = (char)(c - ('a' - 'A'));
    }

    switch (c)
    {
    case '0': return font_0;
    case '1': return font_1;
    case '2': return font_2;
    case '3': return font_3;
    case '4': return font_4;
    case '5': return font_5;
    case '6': return font_6;
    case '7': return font_7;
    case '8': return font_8;
    case '9': return font_9;
    case 'A': return font_A;
    case 'B': return font_B;
    case 'C': return font_C;
    case 'D': return font_D;
    case 'E': return font_E;
    case 'F': return font_F;
    case 'G': return font_G;
    case 'H': return font_H;
    case 'I': return font_I;
    case 'J': return font_J;
    case 'K': return font_K;
    case 'L': return font_L;
    case 'M': return font_M;
    case 'N': return font_N;
    case 'O': return font_O;
    case 'P': return font_P;
    case 'Q': return font_Q;
    case 'R': return font_R;
    case 'S': return font_S;
    case 'T': return font_T;
    case 'U': return font_U;
    case 'V': return font_V;
    case 'W': return font_W;
    case 'X': return font_X;
    case 'Y': return font_Y;
    case 'Z': return font_Z;
    case ':': return colon;
    case '.': return dot;
    case '/': return slash;
    case '%': return percent;
    case '-': return minus;
    default: return blank;
    }
}

static void OLED_WriteCommandBytes(const uint8_t *cmd, uint16_t len)
{
    uint8_t tx[32];
    uint16_t index;
    uint16_t chunk;
    uint16_t offset;

    if ((cmd == 0) || (len == 0u))
    {
        return;
    }

    offset = 0u;
    while (offset < len)
    {
        chunk = (uint16_t)(len - offset);
        if (chunk > 15u)
        {
            chunk = 15u;
        }

        tx[0] = OLED_CONTROL_CMD;
        for (index = 0u; index < chunk; index++)
        {
            tx[index + 1u] = cmd[offset + index];
        }

        BoardI2C_Write(BOARD_OLED_ADDR_7BIT, tx, (uint16_t)(chunk + 1u));
        offset = (uint16_t)(offset + chunk);
    }
}

static void OLED_RenderTextToFramebuffer(void)
{
    uint8_t row;
    uint8_t column;
    uint8_t font_col;
    uint16_t x;
    uint16_t page_offset;
    const uint8_t *glyph;

    memset(g_oled_framebuffer, 0, sizeof(g_oled_framebuffer));

    for (row = 0u; row < OLED_TEXT_ROWS; row++)
    {
        page_offset = (uint16_t)row * BOARD_OLED_WIDTH;
        for (column = 0u; column < OLED_TEXT_COLUMNS; column++)
        {
            x = (uint16_t)column * OLED_CELL_WIDTH;
            if ((x + OLED_FONT_WIDTH) > BOARD_OLED_WIDTH)
            {
                break;
            }

            glyph = OLED_Font5x7(g_oled_text[row][column]);
            for (font_col = 0u; font_col < OLED_FONT_WIDTH; font_col++)
            {
                g_oled_framebuffer[page_offset + x + font_col] = glyph[font_col];
            }
        }
    }
}

static void OLED_WriteText(uint8_t x, uint8_t y, const char *str)
{
    uint8_t column;
    uint8_t index;

    if ((y >= OLED_TEXT_ROWS) || (str == 0))
    {
        return;
    }

    column = (uint8_t)(x / OLED_CELL_WIDTH);
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

void OLED_Init(void)
{
    static const uint8_t init_cmd[] =
    {
        0xAEu, 0x20u, 0x00u, 0xB0u, 0xC8u, 0x00u, 0x10u, 0x40u,
        0x81u, 0x7Fu, 0xA1u, 0xA6u, 0xA8u, 0x3Fu, 0xA4u, 0xD3u,
        0x00u, 0xD5u, 0x80u, 0xD9u, 0xF1u, 0xDAu, 0x12u, 0xDBu,
        0x40u, 0x8Du, 0x14u, 0xAFu
    };

    OLED_Clear();
    memset(g_oled_framebuffer, 0, sizeof(g_oled_framebuffer));
    OLED_WriteCommandBytes(init_cmd, (uint16_t)sizeof(init_cmd));
    OLED_Update();
}

void OLED_Clear(void)
{
    uint8_t row;

    for (row = 0u; row < OLED_TEXT_ROWS; row++)
    {
        memset(g_oled_text[row], ' ', OLED_TEXT_COLUMNS);
        g_oled_text[row][OLED_TEXT_COLUMNS] = '\0';
    }
}

void OLED_Update(void)
{
    uint8_t page;
    uint8_t chunk;
    uint8_t offset;
    uint8_t cmd[3];
    uint8_t tx[16];

    OLED_RenderTextToFramebuffer();

    for (page = 0u; page < BOARD_OLED_PAGE_NUM; page++)
    {
        cmd[0] = (uint8_t)(0xB0u + page);
        cmd[1] = 0x00u;
        cmd[2] = 0x10u;
        OLED_WriteCommandBytes(cmd, (uint16_t)sizeof(cmd));

        offset = 0u;
        while (offset < BOARD_OLED_WIDTH)
        {
            chunk = (uint8_t)(BOARD_OLED_WIDTH - offset);
            if (chunk > 15u)
            {
                chunk = 15u;
            }

            tx[0] = OLED_CONTROL_DATA;
            memcpy(&tx[1],
                   &g_oled_framebuffer[((uint16_t)page * BOARD_OLED_WIDTH) + offset],
                   chunk);
            BoardI2C_Write(BOARD_OLED_ADDR_7BIT, tx, (uint16_t)(chunk + 1u));
            offset = (uint8_t)(offset + chunk);
        }
    }
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
