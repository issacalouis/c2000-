/*
 * File: hmi_menu.h
 * Description: Menu state machine interface.
 * Notes: Menu interprets key events and updates HMI parameters only.
 */
#ifndef HMI_MENU_H
#define HMI_MENU_H

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

void HMIMenu_Init(void);
void HMIMenu_Task_10ms(KeyEvent_t event);
HMI_MenuPage_t HMIMenu_GetPage(void);
const char *HMIMenu_GetFaultText(uint16_t fault_code);

#ifdef __cplusplus
}
#endif

#endif
