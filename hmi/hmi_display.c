/*
 * File: hmi_display.c
 * Description: Low-speed HMI page rendering through the OLED abstraction.
 * Notes: Formatting and display update happen only in the 100 ms task.
 */
#include "hmi_display.h"
#include "hmi_menu.h"
#include "hmi_param.h"
#include "oled.h"

static void HMIDisplay_ShowMain(const HMI_Data_t *data)
{
    OLED_ShowString(0u, 0u, "Vin :");
    OLED_ShowFloat(42u, 0u, data->vin, 1u);
    OLED_ShowString(86u, 0u, "V");

    OLED_ShowString(0u, 1u, "Vout:");
    OLED_ShowFloat(42u, 1u, data->vout, 1u);
    OLED_ShowString(86u, 1u, "V");

    OLED_ShowString(0u, 2u, "Iout:");
    OLED_ShowFloat(42u, 2u, data->iout, 2u);
    OLED_ShowString(86u, 2u, "A");

    OLED_ShowString(0u, 3u, "Duty:");
    OLED_ShowFloat(42u, 3u, data->duty, 1u);
    OLED_ShowString(86u, 3u, "%");

    OLED_ShowString(0u, 5u, data->enable_cmd != 0u ? "CMD:ON " : "CMD:OFF");
    OLED_ShowString(68u, 5u, HMIMenu_GetFaultText(data->fault_code));
}

static void HMIDisplay_ShowSetFloat(const char *title, float value, const char *unit)
{
    OLED_ShowString(0u, 0u, title);
    OLED_ShowFloat(0u, 2u, value, 2u);
    OLED_ShowString(56u, 2u, unit);
    OLED_ShowString(0u, 5u, "UP/DOWN Adjust");
    OLED_ShowString(0u, 6u, "OK Next");
}

void HMIDisplay_Init(void)
{
    OLED_Init();
    OLED_Clear();
    OLED_Update();
}

void HMIDisplay_Task_100ms(void)
{
    HMI_Data_t data;

    HMIParam_GetSnapshot(&data);

    OLED_Clear();

    switch (HMIMenu_GetPage())
    {
    case HMI_MENU_PAGE_SET_VREF:
        HMIDisplay_ShowSetFloat("Set Vref", data.vref, "V");
        break;

    case HMI_MENU_PAGE_SET_IREF:
        HMIDisplay_ShowSetFloat("Set Iref", data.iref, "A");
        break;

    case HMI_MENU_PAGE_SET_ENABLE:
        OLED_ShowString(0u, 0u, "Set Enable");
        OLED_ShowString(0u, 2u, data.enable_cmd != 0u ? "ON" : "OFF");
        OLED_ShowString(0u, 5u, "UP ON / DOWN OFF");
        break;

    case HMI_MENU_PAGE_SET_MODE:
        OLED_ShowString(0u, 0u, "Set Mode");
        OLED_ShowInt(0u, 2u, (int32_t)data.mode_cmd);
        OLED_ShowString(0u, 5u, "UP/DOWN Adjust");
        break;

    case HMI_MENU_PAGE_FAULT:
        OLED_ShowString(0u, 0u, "Fault");
        OLED_ShowString(0u, 2u, HMIMenu_GetFaultText(data.fault_code));
        OLED_ShowInt(0u, 4u, (int32_t)data.fault_code);
        break;

    case HMI_MENU_PAGE_MAIN:
    default:
        HMIDisplay_ShowMain(&data);
        break;
    }

    OLED_Update();
}
