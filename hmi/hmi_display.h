/*
 * File: hmi_display.h
 * Description: Display rendering facade for HMI pages.
 * Notes: Called from the 100 ms HMI task; never call from the control ISR.
 */
#ifndef HMI_DISPLAY_H
#define HMI_DISPLAY_H

#ifdef __cplusplus
extern "C" {
#endif

void HMIDisplay_Init(void);
void HMIDisplay_Task_100ms(void);

#ifdef __cplusplus
}
#endif

#endif
