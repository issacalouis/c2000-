#ifndef APP_MAIN_H
#define APP_MAIN_H

/*
 * File: app_main.h
 * Description: Application initialization and low-speed task scheduler API.
 * Notes: The 1 ms scheduler hook may be called from a timer ISR.
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
