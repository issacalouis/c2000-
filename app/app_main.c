/*
 * 文件: app_main.c
 * 说明: 用于 C2000 HMI 后台任务的非阻塞应用调度器。
 * 备注: 控制 ISR 必须与该低速 HMI 调度器保持分离。
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
 * 函数: APP_Init
 * 调用周期: 时钟/GPIO/外设基础初始化完成后调用一次。
 * ISR: 否。
 * 阻塞: 否。
 */
void APP_Init(void)
{
    ControlIF_Init();
    HMI_Init();
}

/*
 * 函数: APP_TaskScheduler_1ms_ISR
 * 调用周期: 每 1 ms 由定时器 ISR 或生成的调度节拍调用一次。
 * ISR: 允许。该函数只置位标志，不进行按键扫描或显示刷新。
 * 阻塞: 否。
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
 * 函数: APP_BackgroundLoop
 * 调用周期: 在 main 的 while(1) 中循环调用。
 * ISR: 否。该函数运行低速 HMI 任务。
 * 阻塞: 否。
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
