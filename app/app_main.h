#ifndef APP_MAIN_H
#define APP_MAIN_H

/*
 * 文件: app_main.h
 * 说明: 应用初始化与低速任务调度器 API。
 * 备注: 1 ms 调度钩子可由定时器 ISR 调用。
 */

#ifdef __cplusplus
extern "C" {
#endif

void APP_Init(void);
void APP_TaskScheduler_1ms_ISR(void);
void APP_BackgroundLoop(void);

#ifdef __cplusplus
}
#endif

#endif
