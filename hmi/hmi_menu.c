/*
 * File: hmi_menu.c
 * Description: Non-blocking menu state machine for setpoint and status pages.
 * Notes: Safe to test on PC. It performs no direct GPIO, OLED, or control access.
 */

#include "hmi_menu.h"

#include "app_types.h"
#include "hmi_param.h"

static HMI_MenuPage_t g_hmi_menu_page;
static uint16_t g_hmi_menu_edit_index;

static void HMI_Menu_NextPage(void)
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

static void HMI_Menu_PreviousPage(void)
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

static void HMI_Menu_AdjustCurrent(int16_t step)
{
    float small_step;

    small_step = (float)step;

    switch (g_hmi_menu_page)
    {
    case HMI_MENU_PAGE_SET_VREF:
        HMI_Param_AdjustVref(0.1f * small_step);
        break;

    case HMI_MENU_PAGE_SET_IREF:
        HMI_Param_AdjustIref(0.1f * small_step);
        break;

    case HMI_MENU_PAGE_SET_ENABLE:
        HMI_Param_ToggleEnable();
        break;

    case HMI_MENU_PAGE_SET_MODE:
        HMI_Param_SetModeCmd((uint16_t)((HMI_Param_GetModeCmd() + 1u) & 0x0003u));
        break;

    default:
        break;
    }
}

/*
 * Function: HMI_Menu_Init
 * Call period: once from HMI_Init().
 * ISR: no.
 * Blocking: no.
 */
void HMI_Menu_Init(void)
{
    g_hmi_menu_page = HMI_MENU_PAGE_MAIN;
    g_hmi_menu_edit_index = 0u;
}

/*
 * Function: HMI_Menu_Task_10ms
 * Call period: after Key_Task_10ms().
 * ISR: no.
 * Blocking: no.
 */
void HMI_Menu_Task_10ms(KeyEvent_t event)
{
    switch (event)
    {
    case KEY_EVENT_UP:
        if (g_hmi_menu_page == HMI_MENU_PAGE_MAIN || g_hmi_menu_page == HMI_MENU_PAGE_FAULT)
        {
            HMI_Menu_PreviousPage();
        }
        else
        {
            HMI_Menu_AdjustCurrent(1);
        }
        break;

    case KEY_EVENT_DOWN:
        if (g_hmi_menu_page == HMI_MENU_PAGE_MAIN || g_hmi_menu_page == HMI_MENU_PAGE_FAULT)
        {
            HMI_Menu_NextPage();
        }
        else
        {
            HMI_Menu_AdjustCurrent(-1);
        }
        break;

    case KEY_EVENT_LONG_UP:
        HMI_Menu_AdjustCurrent(10);
        break;

    case KEY_EVENT_LONG_DOWN:
        HMI_Menu_AdjustCurrent(-10);
        break;

    case KEY_EVENT_LEFT:
        HMI_Menu_PreviousPage();
        break;

    case KEY_EVENT_RIGHT:
    case KEY_EVENT_OK:
        HMI_Menu_NextPage();
        break;

    case KEY_EVENT_BACK:
        g_hmi_menu_page = HMI_MENU_PAGE_MAIN;
        break;

    case KEY_EVENT_RUN:
    case KEY_EVENT_LONG_RUN:
        HMI_Param_ToggleEnable();
        break;

    case KEY_EVENT_LONG_OK:
        g_hmi_menu_page = HMI_MENU_PAGE_MAIN;
        break;

    default:
        break;
    }

    if (HMI_Param_GetFaultCode() != FAULT_NONE)
    {
        g_hmi_menu_page = HMI_MENU_PAGE_FAULT;
    }
}

HMI_MenuPage_t HMI_Menu_GetPage(void)
{
    return g_hmi_menu_page;
}

uint16_t HMI_Menu_GetEditIndex(void)
{
    return g_hmi_menu_edit_index;
}
