/*
 * 文件: key_scan_test.c
 * 说明: 用于按键消抖和长按事件生成的 PC mock 测试。
 * 备注: 需使用 -DUNIT_TEST 编译；不依赖 C2000 硬件。
 */

#include <assert.h>
#include <stdio.h>

#include "board_gpio.h"
#include "key.h"

#define TEST_KEY_UP_MATRIX_MASK  (1u << 0u)
#define TEST_KEY_RUN_MATRIX_MASK (1u << 15u)

static void scan_ticks(uint16_t mask, unsigned ticks)
{
    unsigned i;

    BoardGPIO_MockSetMatrixMask(mask);
    for (i = 0u; i < ticks; i++)
    {
        Key_Task_10ms();
    }
}

int main(void)
{
    Key_Init();

    scan_ticks(TEST_KEY_UP_MATRIX_MASK, 4u);
    assert(Key_GetEvent() == KEY_EVENT_NONE);
    scan_ticks(0u, 4u);
    assert(Key_GetEvent() == KEY_EVENT_UP);

    scan_ticks(TEST_KEY_RUN_MATRIX_MASK, 110u);
    assert(Key_GetEvent() == KEY_EVENT_LONG_RUN);
    scan_ticks(0u, 4u);
    assert(Key_GetEvent() == KEY_EVENT_NONE);

    printf("key_scan_test passed\n");
    return 0;
}
