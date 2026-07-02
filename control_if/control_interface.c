/*
 * File: control_interface.c
 * Description: Shared setpoint and feedback storage for HMI/control exchange.
 * Notes: Functions are non-blocking. Add platform critical sections if HMI and
 *        control ISR can access multi-word data concurrently.
 */
#include "control_interface.h"
#include "app_config.h"
#include "app_types.h"

#ifndef CONTROLIF_CRITICAL_ENTER
#define CONTROLIF_CRITICAL_ENTER() do { } while (0)
#endif

#ifndef CONTROLIF_CRITICAL_EXIT
#define CONTROLIF_CRITICAL_EXIT() do { } while (0)
#endif

static Control_Setpoint_t g_control_if_setpoint;
static Control_Feedback_t g_control_if_feedback;

void ControlIF_Init(void)
{
    g_control_if_setpoint.vref = APP_VREF_DEFAULT;
    g_control_if_setpoint.iref = APP_IREF_DEFAULT;
    g_control_if_setpoint.enable_cmd = APP_ENABLE_OFF;
    g_control_if_setpoint.mode_cmd = 0u;

    g_control_if_feedback.vin = 0.0f;
    g_control_if_feedback.vout = 0.0f;
    g_control_if_feedback.iout = 0.0f;
    g_control_if_feedback.duty = 0.0f;
    g_control_if_feedback.run_state = APP_RUN_STATE_STOP;
    g_control_if_feedback.fault_code = FAULT_NONE;
}

void ControlIF_SetSetpoint(const Control_Setpoint_t *setpoint)
{
    if (setpoint == (const Control_Setpoint_t *)0)
    {
        return;
    }

    CONTROLIF_CRITICAL_ENTER();
    g_control_if_setpoint = *setpoint;
    CONTROLIF_CRITICAL_EXIT();
}

void ControlIF_GetSetpoint(Control_Setpoint_t *setpoint)
{
    if (setpoint == (Control_Setpoint_t *)0)
    {
        return;
    }

    CONTROLIF_CRITICAL_ENTER();
    *setpoint = g_control_if_setpoint;
    CONTROLIF_CRITICAL_EXIT();
}

void ControlIF_SetFeedback(const Control_Feedback_t *feedback)
{
    if (feedback == (const Control_Feedback_t *)0)
    {
        return;
    }

    CONTROLIF_CRITICAL_ENTER();
    g_control_if_feedback = *feedback;
    CONTROLIF_CRITICAL_EXIT();
}

void ControlIF_GetFeedback(Control_Feedback_t *feedback)
{
    if (feedback == (Control_Feedback_t *)0)
    {
        return;
    }

    CONTROLIF_CRITICAL_ENTER();
    *feedback = g_control_if_feedback;
    CONTROLIF_CRITICAL_EXIT();
}
