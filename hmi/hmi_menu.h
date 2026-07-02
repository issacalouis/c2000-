#ifndef HMI_MENU_H
#define HMI_MENU_H

/*
 * File: hmi_menu.h
 * Description: Key-driven menu state machine interface.
 * Notes: Menu code interprets key events; key drivers only report events.
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
