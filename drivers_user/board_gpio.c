/*
 * File: board_gpio.c
 * Description: Portable GPIO stubs for keys and matrix scanning.
 * Notes: TODO map these functions to the selected C2000 GPIO pins.
 */
#include "board_gpio.h"
#include "app_config.h"

static uint16_t g_board_key_up = BOARD_KEY_RELEASED;
static uint16_t g_board_key_down = BOARD_KEY_RELEASED;
static uint16_t g_board_key_left = BOARD_KEY_RELEASED;
static uint16_t g_board_key_right = BOARD_KEY_RELEASED;
static uint16_t g_board_key_ok = BOARD_KEY_RELEASED;
static uint16_t g_board_key_back = BOARD_KEY_RELEASED;
static uint16_t g_board_key_run = BOARD_KEY_RELEASED;
static uint16_t g_board_matrix[4][4];
static uint16_t g_board_active_row = 0u;

void BoardGPIO_Init(void)
{
    /* TODO: Configure target C2000 GPIO direction, pullups, and qualification. */
}

uint16_t BoardGPIO_ReadKeyUp(void)
{
    return g_board_key_up;
}

uint16_t BoardGPIO_ReadKeyDown(void)
{
    return g_board_key_down;
}

uint16_t BoardGPIO_ReadKeyLeft(void)
{
    return g_board_key_left;
}

uint16_t BoardGPIO_ReadKeyRight(void)
{
    return g_board_key_right;
}

uint16_t BoardGPIO_ReadKeyOk(void)
{
    return g_board_key_ok;
}

uint16_t BoardGPIO_ReadKeyBack(void)
{
    return g_board_key_back;
}

uint16_t BoardGPIO_ReadKeyRun(void)
{
    return g_board_key_run;
}

void BoardGPIO_SetMatrixRow(uint16_t row, uint16_t active)
{
    if (row < 4u)
    {
        g_board_active_row = active != 0u ? row : 0u;
    }
}

uint16_t BoardGPIO_ReadMatrixColumn(uint16_t col)
{
    if (col >= 4u)
    {
        return BOARD_KEY_RELEASED;
    }

    return g_board_matrix[g_board_active_row][col];
}

#if defined(UNIT_TEST)
void BoardGPIO_MockSetIndependent(uint16_t up,
                                  uint16_t down,
                                  uint16_t left,
                                  uint16_t right,
                                  uint16_t ok,
                                  uint16_t back,
                                  uint16_t run)
{
    g_board_key_up = up;
    g_board_key_down = down;
    g_board_key_left = left;
    g_board_key_right = right;
    g_board_key_ok = ok;
    g_board_key_back = back;
    g_board_key_run = run;
}

void BoardGPIO_MockSetMatrix(uint16_t row, uint16_t col, uint16_t pressed)
{
    if ((row < 4u) && (col < 4u))
    {
        g_board_matrix[row][col] = pressed;
    }
}
#endif
