/*
 * File: control_interface.c
 * Description: Shared setpoint and feedback storage for HMI/control exchange.
 * Notes: No blocking calls. On the target, protect multi-word updates if this
 *        module is accessed from both ISR and background contexts.
 */

#include "control_interface.h"

#include "app_config.h"
#include "app_types.h"

static volatile Control_Setpoint_t g_controlif_setpoint;
static volatile Control_Feedback_t g_controlif_feedback;

/*
 * Function: ControlIF_Init
 * Call period: once during APP_Init().
 * ISR: allowed before interrupts start; avoid calling repeatedly from ISR.
 * Blocking: no.
 */
void ControlIF_Init(void)
{
    g_controlif_setpoint.vref = APP_VREF_DEFAULT;
    g_controlif_setpoint.iref = APP_IREF_DEFAULT;
    g_controlif_setpoint.enable_cmd = APP_FALSE;
    g_controlif_setpoint.mode_cmd = 0u;

    g_controlif_feedback.vin = 0.0f;
    g_controlif_feedback.vout = 0.0f;
    g_controlif_feedback.iout = 0.0f;
    g_controlif_feedback.duty = 0.0f;
    g_controlif_feedback.run_state = APP_RUN_STATE_STOP;
    g_controlif_feedback.fault_code = FAULT_NONE;
}

/*
 * Function: ControlIF_SetSetpoint
 * Call period: low-speed HMI task, after user parameters change.
 * ISR: not recommended because multi-field writes are not atomic on all C2000s.
 * Blocking: no.
 */
void ControlIF_SetSetpoint(const Control_Setpoint_t *setpoint)
{
    if (setpoint == 0)
    {
        return;
    }

    g_controlif_setpoint = *setpoint;
}

/*
 * Function: ControlIF_GetSetpoint
 * Call period: control step or generated-code wrapper.
 * ISR: allowed; add a critical section if the application needs a coherent
 *      multi-field snapshot during concurrent HMI writes.
 * Blocking: no.
 */
void ControlIF_GetSetpoint(Control_Setpoint_t *setpoint)
{
    if (setpoint == 0)
    {
        return;
    }

    *setpoint = g_controlif_setpoint;
}

/*
 * Function: ControlIF_SetFeedback
 * Call period: control ISR after ADC/control/PWM update.
 * ISR: allowed.
 * Blocking: no.
 */
void ControlIF_SetFeedback(const Control_Feedback_t *feedback)
{
    if (feedback == 0)
    {
        return;
    }

    g_controlif_feedback = *feedback;
}

/*
 * Function: ControlIF_GetFeedback
 * Call period: low-speed HMI display task.
 * ISR: not needed.
 * Blocking: no.
 */
void ControlIF_GetFeedback(Control_Feedback_t *feedback)
{
    if (feedback == 0)
    {
        return;
    }

    *feedback = g_controlif_feedback;
}
