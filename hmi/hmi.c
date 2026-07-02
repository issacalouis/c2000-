/*
 * 文件: hmi.c
 * 说明: 用于按键、菜单、显示和 ControlIF 的 HMI 顶层调度粘合层。
 * 备注: 所有任务都应为非阻塞方式。不要在高速控制 ISR 中调用显示或按键任务。
 */

#include "hmi.h"

#include "control_interface.h"
#include "app_config.h"
#include "hmi_display.h"
#include "hmi_menu.h"
#include "hmi_param.h"

static void HMI_PushSetpointToControlIF(void)
{
    Control_Setpoint_t setpoint;

    setpoint.vref = HMI_Param_GetVref();
    setpoint.iref = HMI_Param_GetIref();
    setpoint.enable_cmd = HMI_Param_GetEnableCmd();
    setpoint.mode_cmd = HMI_Param_GetModeCmd();
    ControlIF_SetSetpoint(&setpoint);
}

static void HMI_PullFeedbackFromControlIF(void)
{
    Control_Feedback_t feedback;

    ControlIF_GetFeedback(&feedback);
    HMI_Param_SetVin(feedback.vin);
    HMI_Param_SetVout(feedback.vout);
    HMI_Param_SetIout(feedback.iout);
    HMI_Param_SetDuty(feedback.duty);
    HMI_Param_SetRunState(feedback.run_state);
    HMI_Param_SetFaultCode(feedback.fault_code);
}

/*
 * 函数: HMI_Init
 * 调用周期: 在 APP_Init() 期间调用一次。
 * ISR: 否。
 * 阻塞: 否。
 */
void HMI_Init(void)
{
    HMI_Param_Init();
    HMI_Menu_Init();
    HMI_Display_Init();
    HMI_PushSetpointToControlIF();
}

/*
 * 函数: HMI_Task_1ms
 * 调用周期: 1 ms 后台任务。
 * ISR: 否；APP_TaskScheduler_1ms_ISR 只负责置位标志。
 * 阻塞: 否。
 */
void HMI_Task_1ms(void)
{
    /* 预留给未来的非阻塞定时处理。重负载工作不要放在 1 ms 任务中。 */
}

/*
 * 函数: HMI_Task_10ms
 * 调用周期: 10 ms 后台任务。
 * ISR: 否，因为该任务会扫描按键并更新菜单状态。
 * 阻塞: 否。
 */
void HMI_Task_10ms(void)
{
    static uint16_t menu_elapsed_ms = 0u;
    static KeyEvent_t pending_event = KEY_EVENT_NONE;
    KeyEvent_t event;

    Key_Task_10ms();
    event = Key_GetEvent();
    if (event != KEY_EVENT_NONE)
    {
        pending_event = event;
    }

    menu_elapsed_ms += APP_KEY_TASK_10MS;
    if (menu_elapsed_ms >= APP_MENU_TASK_20MS)
    {
        menu_elapsed_ms = 0u;
        HMI_Menu_Task_20ms(pending_event);
        pending_event = KEY_EVENT_NONE;
        HMI_PushSetpointToControlIF();
    }
}

/*
 * 函数: HMI_Task_100ms
 * 调用周期: 100 ms 后台任务。
 * ISR: 否，因为该任务会格式化显示数据并刷新 OLED。
 * 阻塞: 在本层为否；目标驱动也必须保持非阻塞。
 */
void HMI_Task_100ms(void)
{
    HMI_PullFeedbackFromControlIF();
    HMI_Display_Task_100ms();
}

float HMI_Get_Vref(void)
{
    return HMI_Param_GetVref();
}

float HMI_Get_Iref(void)
{
    return HMI_Param_GetIref();
}

uint16_t HMI_Get_EnableCmd(void)
{
    return HMI_Param_GetEnableCmd();
}

uint16_t HMI_Get_ModeCmd(void)
{
    return HMI_Param_GetModeCmd();
}

void HMI_Set_Vin(float vin)
{
    HMI_Param_SetVin(vin);
}

void HMI_Set_Vout(float vout)
{
    HMI_Param_SetVout(vout);
}

void HMI_Set_Iout(float iout)
{
    HMI_Param_SetIout(iout);
}

void HMI_Set_Duty(float duty)
{
    HMI_Param_SetDuty(duty);
}

void HMI_Set_FaultCode(uint16_t fault_code)
{
    HMI_Param_SetFaultCode(fault_code);
}

void HMI_Set_RunState(uint16_t run_state)
{
    HMI_Param_SetRunState(run_state);
}
