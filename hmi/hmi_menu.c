/*
 * File: hmi_menu.c
 * Description: Small non-blocking menu state machine.
 * Notes: Called from the 10 ms HMI task; never call from the control ISR.
 */
#include "hmi_menu.h"
#include "hmi_param.h"
#include "app_config.h"
#include "app_types.h"

static HMI_MenuPage_t g_hmi_menu_page;

static void HMIMenu_NextPage(void)
{
    if (g_hmi_menu_page >= HMI_MENU_PAGE_FAULT)
    {
        g_hmi_menu_page = HMI_MENU_PAGE_MAIN;
    }
    else
    {
        g_hmi_menu_page = (HMI_MenuPage_t)((uint16_t)g_hmi_menu_page + 1u);
    }
}

static void HMIMenu_PreviousPage(void)
{
    if (g_hmi_menu_page == HMI_MENU_PAGE_MAIN)
    {
        g_hmi_menu_page = HMI_MENU_PAGE_FAULT;
    }
    else
    {
        g_hmi_menu_page = (HMI_MenuPage_t)((uint16_t)g_hmi_menu_page - 1u);
    }
}

static void HMIMenu_AdjustSelected(float direction)
{
    switch (g_hmi_menu_page)
    {
    case HMI_MENU_PAGE_SET_VREF:
        HMIParam_SetVref(HMIParam_GetVref() + (direction * 0.1f));
        break;

    case HMI_MENU_PAGE_SET_IREF:
        HMIParam_SetIref(HMIParam_GetIref() + (direction * 0.1f));
        break;

    case HMI_MENU_PAGE_SET_ENABLE:
        HMIParam_SetEnableCmd(direction > 0.0f ? APP_ENABLE_ON : APP_ENABLE_OFF);
        break;

    case HMI_MENU_PAGE_SET_MODE:
        if (direction > 0.0f)
        {
            HMIParam_SetModeCmd((uint16_t)(HMIParam_GetModeCmd() + 1u));
        }
        else if (HMIParam_GetModeCmd() > APP_MODE_MIN)
        {
            HMIParam_SetModeCmd((uint16_t)(HMIParam_GetModeCmd() - 1u));
        }
        break;

    default:
        break;
    }
}

void HMIMenu_Init(void)
{
    g_hmi_menu_page = HMI_MENU_PAGE_MAIN;
}

void HMIMenu_Task_10ms(KeyEvent_t event)
{
    switch (event)
    {
    case KEY_EVENT_UP:
        HMIMenu_AdjustSelected(1.0f);
        break;

    case KEY_EVENT_DOWN:
        HMIMenu_AdjustSelected(-1.0f);
        break;

    case KEY_EVENT_LEFT:
    case KEY_EVENT_BACK:
        HMIMenu_PreviousPage();
        break;

    case KEY_EVENT_RIGHT:
    case KEY_EVENT_OK:
        HMIMenu_NextPage();
        break;

    case KEY_EVENT_RUN:
        HMIParam_SetEnableCmd(HMIParam_GetEnableCmd() == APP_ENABLE_OFF);
        break;

    case KEY_EVENT_LONG_UP:
        HMIMenu_AdjustSelected(1.0f);
        HMIMenu_AdjustSelected(1.0f);
        HMIMenu_AdjustSelected(1.0f);
        HMIMenu_AdjustSelected(1.0f);
        HMIMenu_AdjustSelected(1.0f);
        break;

    case KEY_EVENT_LONG_DOWN:
        HMIMenu_AdjustSelected(-1.0f);
        HMIMenu_AdjustSelected(-1.0f);
        HMIMenu_AdjustSelected(-1.0f);
        HMIMenu_AdjustSelected(-1.0f);
        HMIMenu_AdjustSelected(-1.0f);
        break;

    case KEY_EVENT_LONG_OK:
        g_hmi_menu_page = HMI_MENU_PAGE_MAIN;
        break;

    case KEY_EVENT_LONG_RUN:
        HMIParam_SetEnableCmd(APP_ENABLE_OFF);
        break;

    case KEY_EVENT_NONE:
    default:
        break;
    }
}

HMI_MenuPage_t HMIMenu_GetPage(void)
{
    return g_hmi_menu_page;
}

const char *HMIMenu_GetFaultText(uint16_t fault_code)
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
