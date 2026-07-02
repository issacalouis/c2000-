/*
 * File: app_main.h
 * Description: Application init and low-speed task scheduler API.
 * Notes: Call APP_TaskScheduler_1ms_ISR from a 1 ms timer ISR or equivalent tick.
 */
#ifndef APP_MAIN_H
#define APP_MAIN_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void APP_Init(void);
void APP_TaskScheduler_1ms_ISR(void);
void APP_BackgroundLoop(void);

uint16_t APP_IsTask1msPending(void);
uint16_t APP_IsTask10msPending(void);
uint16_t APP_IsTask100msPending(void);

#ifdef __cplusplus
}
#endif

#endif
