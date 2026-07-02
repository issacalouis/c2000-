/*
 * File: app_main.c
 * Description: Non-blocking low-speed scheduler for HMI tasks.
 * Notes: The 1 ms scheduler function is ISR-safe and does not run slow work.
 */
#include "app_main.h"
#include "hmi.h"
#include "control_interface.h"

static volatile uint16_t g_app_task_1ms_flag = 0u;
static volatile uint16_t g_app_task_10ms_flag = 0u;
static volatile uint16_t g_app_task_100ms_flag = 0u;

void APP_Init(void)
{
    ControlIF_Init();
    HMI_Init();
}

void APP_TaskScheduler_1ms_ISR(void)
{
    static uint16_t cnt_10ms = 0u;
    static uint16_t cnt_100ms = 0u;

    g_app_task_1ms_flag = 1u;

    cnt_10ms++;
    cnt_100ms++;

    if (cnt_10ms >= 10u)
    {
        cnt_10ms = 0u;
        g_app_task_10ms_flag = 1u;
    }

    if (cnt_100ms >= 100u)
    {
        cnt_100ms = 0u;
        g_app_task_100ms_flag = 1u;
    }
}

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

uint16_t APP_IsTask1msPending(void)
{
    return g_app_task_1ms_flag;
}

uint16_t APP_IsTask10msPending(void)
{
    return g_app_task_10ms_flag;
}

uint16_t APP_IsTask100msPending(void)
{
    return g_app_task_100ms_flag;
}
