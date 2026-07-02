/*
 * 文件: hmi_mock_test.c
 * 说明: 用于 HMI 默认值、按键驱动设定值更新和显示刷新的 PC mock 测试。
 * 备注: 需使用 -DUNIT_TEST 编译；仅使用板级和 OLED 的 mock 钩子。
 */

#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

#include "app_types.h"
#include "board_gpio.h"
#include "control_interface.h"
#include "hmi.h"
#include "key.h"
#include "oled.h"

static void hmi_key_ticks(uint16_t mask, unsigned ticks)
{
    unsigned i;

    BoardGPIO_MockSetMatrixMask(mask);
    for (i = 0u; i < ticks; i++)
    {
        HMI_Task_10ms();
    }
}

static void hmi_short_press(uint16_t mask)
{
    hmi_key_ticks(mask, 4u);
    hmi_key_ticks(0u, 4u);
}

int main(void)
{
    Control_Feedback_t feedback;

    ControlIF_Init();
    HMI_Init();

    assert(fabs((double)(HMI_Get_Vref() - 12.0f)) < 0.001);
    assert(fabs((double)(HMI_Get_Iref() - 2.0f)) < 0.001);
    assert(HMI_Get_EnableCmd() == 0u);

    hmi_short_press(KEY_MASK_RIGHT);
    hmi_short_press(KEY_MASK_UP);
    assert(HMI_Get_Vref() > 12.05f);

    hmi_short_press(KEY_MASK_RUN);
    assert(HMI_Get_EnableCmd() == 1u);

    hmi_short_press(KEY_MASK_BACK);

    feedback.vin = 24.0f;
    feedback.vout = 12.5f;
    feedback.iout = 1.5f;
    feedback.duty = 42.3f;
    feedback.run_state = APP_RUN_STATE_RUN;
    feedback.fault_code = FAULT_NONE;
    ControlIF_SetFeedback(&feedback);

    HMI_Task_100ms();
    assert(strstr(OLED_MockGetLine(0u), "Vin") != 0);

    printf("hmi_mock_test passed\n");
    return 0;
}
