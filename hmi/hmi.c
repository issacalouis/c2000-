/*
 * File: hmi.c
 * Description: HMI top-level scheduler glue for key, menu, display, and ControlIF.
 * Notes: All tasks are non-blocking. Do not call display/key tasks from the
 *        high-speed control ISR.
 */

#include "hmi.h"

#include "control_interface.h"
#include "app_config.h"
#include "hmi_display.h"
#include "hmi_menu.h"
#include "hmi_param.h"
#include "key.h"
#include "oled.h"

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
 * Function: HMI_Init
 * Call period: once during APP_Init().
 * ISR: no.
 * Blocking: no.
 */
void HMI_Init(void)
{
    HMI_Param_Init();
    Key_Init();
    OLED_Init();
    HMI_Menu_Init();
    HMI_Display_Init();
    HMI_PushSetpointToControlIF();
}

/*
 * Function: HMI_Task_1ms
 * Call period: 1 ms background task.
 * ISR: no; APP_TaskScheduler_1ms_ISR only sets a flag.
 * Blocking: no.
 */
void HMI_Task_1ms(void)
{
    /* Reserved for future non-blocking timers. Keep heavy work out of 1 ms. */
}

/*
 * Function: HMI_Task_10ms
 * Call period: 10 ms background task.
 * ISR: no, because it scans keys and updates menu state.
 * Blocking: no.
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
 * Function: HMI_Task_100ms
 * Call period: 100 ms background task.
 * ISR: no, because it formats display data and refreshes OLED.
 * Blocking: no at this layer; target driver must also remain non-blocking.
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
