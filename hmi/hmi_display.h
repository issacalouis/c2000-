#ifndef HMI_DISPLAY_H
#define HMI_DISPLAY_H

/*
 * 文件: hmi_display.h
 * 说明: HMI 显示渲染入口。
 * 备注: 渲染逻辑刻意与 OLED 传输代码分离。
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
