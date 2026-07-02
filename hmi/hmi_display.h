#ifndef HMI_DISPLAY_H
#define HMI_DISPLAY_H

/*
 * File: hmi_display.h
 * Description: HMI display rendering entry points.
 * Notes: Rendering is intentionally separated from OLED transport code.
 */

#ifdef __cplusplus
extern "C" {
#endif

void HMI_Display_Init(void);
void HMI_Display_Task_100ms(void);

#ifdef __cplusplus
}
#endif

#endif
