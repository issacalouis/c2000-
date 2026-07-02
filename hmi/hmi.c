/*
 * File: hmi.c
 * Description: Top-level HMI orchestration and public wrapper API.
 * Notes: 10 ms and 100 ms tasks are slow-path functions; keep them out of the
 *        high-speed control ISR.
 */
#include "hmi.h"
#include "hmi_param.h"
#include "hmi_menu.h"
#include "hmi_display.h"
#include "key.h"
#include "control_interface.h"

static void HMI_PublishSetpoint(void)
{
    Control_Setpoint_t setpoint;

    HMIParam_ToSetpoint(&setpoint);
    ControlIF_SetSetpoint(&setpoint);
}

void HMI_Init(void)
{
    HMIParam_Init();
    Key_Init();
    HMIMenu_Init();
    HMIDisplay_Init();
    HMI_PublishSetpoint();
}

void HMI_Task_1ms(void)
{
    Control_Feedback_t feedback;

    ControlIF_GetFeedback(&feedback);
    HMIParam_SetFeedback(&feedback);
}

void HMI_Task_10ms(void)
{
    KeyEvent_t event;

    Key_Task_10ms();
    event = Key_GetEvent();
    HMIMenu_Task_10ms(event);
    HMI_PublishSetpoint();
}

void HMI_Task_100ms(void)
{
    HMIDisplay_Task_100ms();
}

float HMI_Get_Vref(void)
{
    return HMIParam_GetVref();
}

float HMI_Get_Iref(void)
{
    return HMIParam_GetIref();
}

uint16_t HMI_Get_EnableCmd(void)
{
    return HMIParam_GetEnableCmd();
}

uint16_t HMI_Get_ModeCmd(void)
{
    return HMIParam_GetModeCmd();
}

void HMI_Set_Vin(float vin)
{
    Control_Feedback_t feedback;

    ControlIF_GetFeedback(&feedback);
    feedback.vin = vin;
    ControlIF_SetFeedback(&feedback);
}

void HMI_Set_Vout(float vout)
{
    Control_Feedback_t feedback;

    ControlIF_GetFeedback(&feedback);
    feedback.vout = vout;
    ControlIF_SetFeedback(&feedback);
}

void HMI_Set_Iout(float iout)
{
    Control_Feedback_t feedback;

    ControlIF_GetFeedback(&feedback);
    feedback.iout = iout;
    ControlIF_SetFeedback(&feedback);
}

void HMI_Set_Duty(float duty)
{
    Control_Feedback_t feedback;

    ControlIF_GetFeedback(&feedback);
    feedback.duty = duty;
    ControlIF_SetFeedback(&feedback);
}

void HMI_Set_FaultCode(uint16_t fault_code)
{
    Control_Feedback_t feedback;

    ControlIF_GetFeedback(&feedback);
    feedback.fault_code = fault_code;
    ControlIF_SetFeedback(&feedback);
}

void HMI_Set_RunState(uint16_t run_state)
{
    Control_Feedback_t feedback;

    ControlIF_GetFeedback(&feedback);
    feedback.run_state = run_state;
    ControlIF_SetFeedback(&feedback);
}
