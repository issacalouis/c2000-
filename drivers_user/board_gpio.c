/*
 * File: board_gpio.c
 * Description: Hardware isolation layer for C2000 GPIO key inputs.
 * Notes: Default implementation is a safe stub. Fill TODOs with F280015x pin
 *        mapping and C2000Ware GPIO_readPin/GPIO_writePin calls.
 */

#include "board_gpio.h"

#define BOARD_KEY_UP_MASK    0x0001u
#define BOARD_KEY_DOWN_MASK  0x0002u
#define BOARD_KEY_LEFT_MASK  0x0004u
#define BOARD_KEY_RIGHT_MASK 0x0008u
#define BOARD_KEY_OK_MASK    0x0010u
#define BOARD_KEY_BACK_MASK  0x0020u
#define BOARD_KEY_RUN_MASK   0x0040u

static uint16_t g_board_gpio_mock_independent_mask;
static uint16_t g_board_gpio_mock_matrix_mask;
static uint8_t g_board_gpio_active_matrix_row;

/*
 * Function: BoardGPIO_Init
 * Call period: once before key scanning starts.
 * ISR: no.
 * Blocking: no.
 */
void BoardGPIO_Init(void)
{
    /*
     * TODO(F280015x): configure independent key GPIOs as inputs with pull-ups,
     * and configure matrix rows as outputs / columns as inputs.
     */
    g_board_gpio_active_matrix_row = 0u;
}

uint16_t BoardGPIO_ReadKeyUp(void)
{
    return (g_board_gpio_mock_independent_mask & BOARD_KEY_UP_MASK) != 0u;
}

uint16_t BoardGPIO_ReadKeyDown(void)
{
    return (g_board_gpio_mock_independent_mask & BOARD_KEY_DOWN_MASK) != 0u;
}

uint16_t BoardGPIO_ReadKeyLeft(void)
{
    return (g_board_gpio_mock_independent_mask & BOARD_KEY_LEFT_MASK) != 0u;
}

uint16_t BoardGPIO_ReadKeyRight(void)
{
    return (g_board_gpio_mock_independent_mask & BOARD_KEY_RIGHT_MASK) != 0u;
}

uint16_t BoardGPIO_ReadKeyOk(void)
{
    return (g_board_gpio_mock_independent_mask & BOARD_KEY_OK_MASK) != 0u;
}

uint16_t BoardGPIO_ReadKeyBack(void)
{
    return (g_board_gpio_mock_independent_mask & BOARD_KEY_BACK_MASK) != 0u;
}

uint16_t BoardGPIO_ReadKeyRun(void)
{
    return (g_board_gpio_mock_independent_mask & BOARD_KEY_RUN_MASK) != 0u;
}

/*
 * Function: BoardGPIO_SetMatrixRow
 * Call period: inside Key_Task_10ms().
 * ISR: no.
 * Blocking: no; do not add delay here.
 */
void BoardGPIO_SetMatrixRow(uint8_t row)
{
    g_board_gpio_active_matrix_row = row;
    /* TODO(F280015x): drive selected row active and other rows inactive. */
}

uint16_t BoardGPIO_ReadMatrixColumn(uint8_t column)
{
    uint8_t bit_index;

    bit_index = (uint8_t)((g_board_gpio_active_matrix_row * 4u) + column);
    if (bit_index >= 16u)
    {
        return 0u;
    }

    return (g_board_gpio_mock_matrix_mask & (uint16_t)(1u << bit_index)) != 0u;
}

#ifdef UNIT_TEST
void BoardGPIO_MockSetIndependentMask(uint16_t mask)
{
    g_board_gpio_mock_independent_mask = mask;
}

void BoardGPIO_MockSetMatrixMask(uint16_t mask)
{
    g_board_gpio_mock_matrix_mask = mask;
}
#endif
