/*
 * File: app_main.c
 * Description: Non-blocking application scheduler for C2000 HMI background work.
 * Notes: The control ISR must remain separate from this low-speed HMI scheduler.
 */

#include "app_main.h"

#include <stdint.h>

#include "app_config.h"
#include "control_interface.h"
#include "hmi.h"

static volatile uint16_t g_app_task_1ms_flag = 0u;
static volatile uint16_t g_app_task_10ms_flag = 0u;
static volatile uint16_t g_app_task_100ms_flag = 0u;

/*
 * Function: APP_Init
 * Call period: once after clock/GPIO/peripheral base initialization.
 * ISR: no.
 * Blocking: no.
 */
void APP_Init(void)
{
    ControlIF_Init();
    HMI_Init();
}

/*
 * Function: APP_TaskScheduler_1ms_ISR
 * Call period: every 1 ms from a timer ISR or a generated scheduler tick.
 * ISR: allowed. It only sets flags and does not scan keys or refresh display.
 * Blocking: no.
 */
void APP_TaskScheduler_1ms_ISR(void)
{
    static uint16_t cnt_10ms = 0u;
    static uint16_t cnt_100ms = 0u;

    g_app_task_1ms_flag = 1u;

    cnt_10ms++;
    cnt_100ms++;

    if (cnt_10ms >= APP_KEY_TASK_10MS)
    {
        cnt_10ms = 0u;
        g_app_task_10ms_flag = 1u;
    }

    if (cnt_100ms >= APP_DISPLAY_TASK_100MS)
    {
        cnt_100ms = 0u;
        g_app_task_100ms_flag = 1u;
    }
}

/*
 * Function: APP_BackgroundLoop
 * Call period: repeatedly from main while(1).
 * ISR: no. This function runs slow HMI tasks.
 * Blocking: no.
 */
void APP_BackgroundLoop(void)
{
    if (g_app_task_1ms_flag != 0u)
    {
        g_app_task_1ms_flag = 0u;
        HMI_Task_1ms();
    }

    if (g_app_task_10ms_flag != 0u)
    {
        g_app_task_10ms_flag = 0u;
        HMI_Task_10ms();
    }

    if (g_app_task_100ms_flag != 0u)
    {
        g_app_task_100ms_flag = 0u;
        HMI_Task_100ms();
    }
}
