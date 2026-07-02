/*
 * File: key_scan_test.c
 * Description: PC-side key debounce and short/long event smoke test.
 * Notes: Build with key.c, board_gpio.c, app_config.h and -DUNIT_TEST.
 */
#include "key.h"
#include "board_gpio.h"
#include "app_config.h"
#include <assert.h>

static void RunTicks(unsigned int count)
{
    unsigned int i;

    for (i = 0u; i < count; i++)
    {
        Key_Task_10ms();
    }
}

int main(void)
{
    Key_Init();
    BoardGPIO_MockSetIndependent(BOARD_KEY_RELEASED,
                                 BOARD_KEY_RELEASED,
                                 BOARD_KEY_RELEASED,
                                 BOARD_KEY_RELEASED,
                                 BOARD_KEY_RELEASED,
                                 BOARD_KEY_RELEASED,
                                 BOARD_KEY_RELEASED);
    RunTicks(5u);
    assert(Key_GetEvent() == KEY_EVENT_NONE);

    BoardGPIO_MockSetIndependent(BOARD_KEY_PRESSED,
                                 BOARD_KEY_RELEASED,
                                 BOARD_KEY_RELEASED,
                                 BOARD_KEY_RELEASED,
                                 BOARD_KEY_RELEASED,
                                 BOARD_KEY_RELEASED,
                                 BOARD_KEY_RELEASED);
    RunTicks(5u);
    assert(Key_GetEvent() == KEY_EVENT_NONE);

    BoardGPIO_MockSetIndependent(BOARD_KEY_RELEASED,
                                 BOARD_KEY_RELEASED,
                                 BOARD_KEY_RELEASED,
                                 BOARD_KEY_RELEASED,
                                 BOARD_KEY_RELEASED,
                                 BOARD_KEY_RELEASED,
                                 BOARD_KEY_RELEASED);
    RunTicks(5u);
    assert(Key_GetEvent() == KEY_EVENT_UP);

    BoardGPIO_MockSetIndependent(BOARD_KEY_RELEASED,
                                 BOARD_KEY_RELEASED,
                                 BOARD_KEY_RELEASED,
                                 BOARD_KEY_RELEASED,
                                 BOARD_KEY_PRESSED,
                                 BOARD_KEY_RELEASED,
                                 BOARD_KEY_RELEASED);
    RunTicks(KEY_LONG_TICKS + 5u);
    assert(Key_GetEvent() == KEY_EVENT_LONG_OK);

    BoardGPIO_MockSetIndependent(BOARD_KEY_RELEASED,
                                 BOARD_KEY_RELEASED,
                                 BOARD_KEY_RELEASED,
                                 BOARD_KEY_RELEASED,
                                 BOARD_KEY_RELEASED,
                                 BOARD_KEY_RELEASED,
                                 BOARD_KEY_RELEASED);
    RunTicks(5u);
    assert(Key_GetEvent() == KEY_EVENT_NONE);

    return 0;
}
