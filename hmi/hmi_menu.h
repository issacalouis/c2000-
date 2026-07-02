#ifndef HMI_MENU_H
#define HMI_MENU_H

/*
 * 文件: hmi_menu.h
 * 说明: 由按键驱动的菜单状态机接口。
 * 备注: 菜单代码负责解释按键事件；按键驱动只负责上报事件。
 */

#include <stdint.h>

#include "key.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
    HMI_MENU_PAGE_MAIN = 0,
    HMI_MENU_PAGE_SET_VREF,
    HMI_MENU_PAGE_SET_IREF,
    HMI_MENU_PAGE_SET_ENABLE,
    HMI_MENU_PAGE_SET_MODE,
    HMI_MENU_PAGE_FAULT
} HMI_MenuPage_t;

void HMI_Menu_Init(void);
void HMI_Menu_Task_20ms(KeyEvent_t event);
HMI_MenuPage_t HMI_Menu_GetPage(void);
uint16_t HMI_Menu_GetEditIndex(void);

#ifdef __cplusplus
}
#endif

#endif
