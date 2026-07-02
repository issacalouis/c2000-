/*
 * 文件: board_gpio.c
 * 说明: 针对 C2000 GPIO 按键输入的硬件隔离层。
 * 备注: 默认实现为安全桩代码。请用 F280015x 引脚映射和
 *       C2000Ware GPIO_readPin/GPIO_writePin 调用补全 TODO。
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
 * 函数: BoardGPIO_Init
 * 调用周期: 按键扫描开始前调用一次。
 * ISR: 否。
 * 阻塞: 否。
 */
void BoardGPIO_Init(void)
{
    /*
     * TODO(F280015x): 将独立按键 GPIO 配置为带上拉输入，
     * 并将矩阵键盘的行配置为输出、列配置为输入。
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
 * 函数: BoardGPIO_SetMatrixRow
 * 调用周期: 在 Key_Task_10ms() 内部调用。
 * ISR: 否。
 * 阻塞: 否；这里不要加入延时。
 */
void BoardGPIO_SetMatrixRow(uint8_t row)
{
    g_board_gpio_active_matrix_row = row;
    /* TODO(F280015x): 将选中的行驱动为有效电平，其余行驱动为无效电平。 */
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
