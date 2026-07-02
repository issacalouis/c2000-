/*
 * 文件: hmi_display.c
 * 说明: 面向 128x64 OLED 抽象层的页面渲染器。
 * 备注: 字符串格式化仅在 100 ms 低速任务中执行。
 */

#include "hmi_display.h"

#include "app_types.h"
#include "hmi_menu.h"
#include "hmi_param.h"
#include "oled.h"

static const char *HMI_Display_FaultText(uint16_t fault_code)
{
    if ((fault_code & FAULT_OVP) != 0u)
    {
        return "OVP";
    }
    if ((fault_code & FAULT_OCP) != 0u)
    {
        return "OCP";
    }
    if ((fault_code & FAULT_OTP) != 0u)
    {
        return "OTP";
    }
    if ((fault_code & FAULT_UVLO) != 0u)
    {
        return "UVLO";
    }
    if ((fault_code & FAULT_ADC) != 0u)
    {
        return "ADC Fault";
    }
    if ((fault_code & FAULT_PWM) != 0u)
    {
        return "PWM Fault";
    }
    return "No Fault";
}

static void HMI_Display_ShowMain(const HMI_Data_t *data)
{
    OLED_ShowString(0u, 0u, "Vin :");
    OLED_ShowFloat(42u, 0u, data->vin, 1u);
    OLED_ShowString(88u, 0u, "V");

    OLED_ShowString(0u, 1u, "Vout:");
    OLED_ShowFloat(42u, 1u, data->vout, 1u);
    OLED_ShowString(88u, 1u, "V");

    OLED_ShowString(0u, 2u, "Iout:");
    OLED_ShowFloat(42u, 2u, data->iout, 2u);
    OLED_ShowString(88u, 2u, "A");

    OLED_ShowString(0u, 3u, "Duty:");
    OLED_ShowFloat(42u, 3u, data->duty, 1u);
    OLED_ShowString(88u, 3u, "%");

    OLED_ShowString(0u, 5u, (data->enable_cmd != 0u) ? "CMD: ON " : "CMD: OFF");
    OLED_ShowString(72u, 5u, (data->run_state == APP_RUN_STATE_RUN) ? "RUN" : "STOP");
    OLED_ShowString(0u, 7u, HMI_Display_FaultText(data->fault_code));
}

static void HMI_Display_ShowSetValue(const char *title, float value, const char *unit)
{
    OLED_ShowString(0u, 0u, title);
    OLED_ShowFloat(0u, 2u, value, 2u);
    OLED_ShowString(64u, 2u, unit);
    OLED_ShowString(0u, 5u, "UP/DOWN Adjust");
    OLED_ShowString(0u, 7u, "OK Next");
}

static void HMI_Display_ShowEnable(const HMI_Data_t *data)
{
    OLED_ShowString(0u, 0u, "Set Enable");
    OLED_ShowString(0u, 2u, (data->enable_cmd != 0u) ? "Output Request ON" : "Output Request OFF");
    OLED_ShowString(0u, 5u, "UP/DOWN Toggle");
    OLED_ShowString(0u, 7u, "RUN Toggle");
}

static void HMI_Display_ShowMode(const HMI_Data_t *data)
{
    OLED_ShowString(0u, 0u, "Set Mode");
    OLED_ShowString(0u, 2u, "Mode:");
    OLED_ShowInt(48u, 2u, (int32_t)data->mode_cmd);
    OLED_ShowString(0u, 5u, "UP/DOWN Change");
    OLED_ShowString(0u, 7u, "OK Next");
}

static void HMI_Display_ShowFault(const HMI_Data_t *data)
{
    OLED_ShowString(0u, 0u, "Fault");
    OLED_ShowString(0u, 2u, HMI_Display_FaultText(data->fault_code));
    OLED_ShowString(0u, 5u, "Code:");
    OLED_ShowInt(48u, 5u, (int32_t)data->fault_code);
    OLED_ShowString(0u, 7u, "BACK Main");
}

/*
 * 函数: HMI_Display_Init
 * 调用周期: 在 HMI_Init() 中调用一次。
 * ISR: 否。
 * 阻塞: 在该抽象层中为否。
 */
void HMI_Display_Init(void)
{
    OLED_Clear();
    OLED_Update();
}

/*
 * 函数: HMI_Display_Task_100ms
 * 调用周期: 100 ms 后台任务。
 * ISR: 否，因为该任务会格式化文本并可能触发显示传输。
 * 阻塞: 在本层为否；板级传输应实现为非阻塞方式。
 */
void HMI_Display_Task_100ms(void)
{
    HMI_Data_t data;

    HMI_Param_GetData(&data);
    OLED_Clear();

    switch (HMI_Menu_GetPage())
    {
    case HMI_MENU_PAGE_SET_VREF:
        HMI_Display_ShowSetValue("Set Vref", data.vref, "V");
        break;

    case HMI_MENU_PAGE_SET_IREF:
        HMI_Display_ShowSetValue("Set Iref", data.iref, "A");
        break;

    case HMI_MENU_PAGE_SET_ENABLE:
        HMI_Display_ShowEnable(&data);
        break;

    case HMI_MENU_PAGE_SET_MODE:
        HMI_Display_ShowMode(&data);
        break;

    case HMI_MENU_PAGE_FAULT:
        HMI_Display_ShowFault(&data);
        break;

    case HMI_MENU_PAGE_MAIN:
    default:
        HMI_Display_ShowMain(&data);
        break;
    }

    OLED_Update();
}
