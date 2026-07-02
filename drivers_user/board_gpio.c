/*
 * File: board_gpio.c
 * Description: LAUNCHXL-F2800157 GPIO backend for the 4x4 matrix keypad.
 * Notes: Matrix columns are low-active. Public read APIs return logical key
 *        state: GPIO 0 returns 1 (pressed), GPIO 1 returns 0 (released).
 */

#include "board_gpio.h"

#include "board_pinmap.h"

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

#ifndef UNIT_TEST
static const uint32_t g_key_row_gpio[KEY_ROW_NUM] =
{
    KEY_ROW0_GPIO,
    KEY_ROW1_GPIO,
    KEY_ROW2_GPIO,
    KEY_ROW3_GPIO
};

static const uint32_t g_key_col_gpio[KEY_COL_NUM] =
{
    KEY_COL0_GPIO,
    KEY_COL1_GPIO,
    KEY_COL2_GPIO,
    KEY_COL3_GPIO
};

static void BoardGPIO_SetPinMuxToGpio(uint32_t pin)
{
    switch (pin)
    {
#ifdef GPIO_25_GPIO25
    case 25U:
        GPIO_setPinConfig(GPIO_25_GPIO25);
        break;
#endif
#ifdef GPIO_26_GPIO26
    case 26U:
        GPIO_setPinConfig(GPIO_26_GPIO26);
        break;
#endif
#ifdef GPIO_27_GPIO27
    case 27U:
        GPIO_setPinConfig(GPIO_27_GPIO27);
        break;
#endif
#ifdef GPIO_33_GPIO33
    case 33U:
        GPIO_setPinConfig(GPIO_33_GPIO33);
        break;
#endif
#ifdef GPIO_44_GPIO44
    case 44U:
        GPIO_setPinConfig(GPIO_44_GPIO44);
        break;
#endif
#ifdef GPIO_45_GPIO45
    case 45U:
        GPIO_setPinConfig(GPIO_45_GPIO45);
        break;
#endif
#ifdef GPIO_46_GPIO46
    case 46U:
        GPIO_setPinConfig(GPIO_46_GPIO46);
        break;
#endif
#ifdef GPIO_48_GPIO48
    case 48U:
        GPIO_setPinConfig(GPIO_48_GPIO48);
        break;
#endif
    default:
        break;
    }
}
#endif

void BoardGPIO_Init(void)
{
#ifndef UNIT_TEST
    uint16_t index;

    for (index = 0u; index < KEY_ROW_NUM; index++)
    {
        BoardGPIO_SetPinMuxToGpio(g_key_row_gpio[index]);
        GPIO_setPadConfig(g_key_row_gpio[index], GPIO_PIN_TYPE_STD);
        GPIO_setDirectionMode(g_key_row_gpio[index], GPIO_DIR_MODE_OUT);
        GPIO_writePin(g_key_row_gpio[index], KEY_INACTIVE_LEVEL);
    }

    for (index = 0u; index < KEY_COL_NUM; index++)
    {
        BoardGPIO_SetPinMuxToGpio(g_key_col_gpio[index]);
        GPIO_setPadConfig(g_key_col_gpio[index], GPIO_PIN_TYPE_PULLUP);
        GPIO_setDirectionMode(g_key_col_gpio[index], GPIO_DIR_MODE_IN);
        GPIO_setQualificationMode(g_key_col_gpio[index], GPIO_QUAL_SYNC);
    }
#endif

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

void BoardGPIO_SetMatrixRow(uint8_t row)
{
#ifndef UNIT_TEST
    uint16_t index;

    for (index = 0u; index < KEY_ROW_NUM; index++)
    {
        GPIO_writePin(g_key_row_gpio[index],
                      (index == row) ? KEY_ACTIVE_LEVEL : KEY_INACTIVE_LEVEL);
    }
#endif

    g_board_gpio_active_matrix_row = row;
}

uint16_t BoardGPIO_ReadMatrixColumn(uint8_t column)
{
#ifndef UNIT_TEST
    if (column >= KEY_COL_NUM)
    {
        return 0u;
    }

    return (GPIO_readPin(g_key_col_gpio[column]) == KEY_ACTIVE_LEVEL) ? 1u : 0u;
#else
    uint8_t bit_index;

    bit_index = (uint8_t)((g_board_gpio_active_matrix_row * KEY_COL_NUM) + column);
    if (bit_index >= (KEY_ROW_NUM * KEY_COL_NUM))
    {
        return 0u;
    }

    return (g_board_gpio_mock_matrix_mask & (uint16_t)(1u << bit_index)) != 0u;
#endif
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
