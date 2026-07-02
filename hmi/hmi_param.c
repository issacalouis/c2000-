/*
 * File: hmi_param.c
 * Description: HMI data model with parameter limit enforcement.
 * Notes: Non-blocking; call from low-speed HMI tasks or HMI wrapper API.
 */
#include "hmi_param.h"
#include "app_config.h"
#include "app_types.h"

static HMI_Data_t g_hmi_param_data;

static float HMIParam_ClampFloat(float value, float min_value, float max_value)
{
    if (value < min_value)
    {
        return min_value;
    }

    if (value > max_value)
    {
        return max_value;
    }

    return value;
}

void HMIParam_Init(void)
{
    g_hmi_param_data.vref = APP_VREF_DEFAULT;
    g_hmi_param_data.iref = APP_IREF_DEFAULT;
    g_hmi_param_data.vin = 0.0f;
    g_hmi_param_data.vout = 0.0f;
    g_hmi_param_data.iout = 0.0f;
    g_hmi_param_data.duty = 0.0f;
    g_hmi_param_data.enable_cmd = APP_ENABLE_OFF;
    g_hmi_param_data.mode_cmd = 0u;
    g_hmi_param_data.run_state = APP_RUN_STATE_STOP;
    g_hmi_param_data.fault_code = FAULT_NONE;
}

void HMIParam_GetSnapshot(HMI_Data_t *data)
{
    if (data == (HMI_Data_t *)0)
    {
        return;
    }

    *data = g_hmi_param_data;
}

void HMIParam_SetVref(float value)
{
    g_hmi_param_data.vref = HMIParam_ClampFloat(value, APP_VREF_MIN, APP_VREF_MAX);
}

void HMIParam_SetIref(float value)
{
    g_hmi_param_data.iref = HMIParam_ClampFloat(value, APP_IREF_MIN, APP_IREF_MAX);
}

void HMIParam_SetEnableCmd(uint16_t value)
{
    g_hmi_param_data.enable_cmd = (value != 0u) ? APP_ENABLE_ON : APP_ENABLE_OFF;
}

void HMIParam_SetModeCmd(uint16_t value)
{
    if (value > APP_MODE_MAX)
    {
        value = APP_MODE_MAX;
    }

    g_hmi_param_data.mode_cmd = value;
}

void HMIParam_SetFeedback(const Control_Feedback_t *feedback)
{
    if (feedback == (const Control_Feedback_t *)0)
    {
        return;
    }

    g_hmi_param_data.vin = feedback->vin;
    g_hmi_param_data.vout = feedback->vout;
    g_hmi_param_data.iout = feedback->iout;
    g_hmi_param_data.duty = feedback->duty;
    g_hmi_param_data.run_state = feedback->run_state;
    g_hmi_param_data.fault_code = feedback->fault_code;
}

void HMIParam_ToSetpoint(Control_Setpoint_t *setpoint)
{
    if (setpoint == (Control_Setpoint_t *)0)
    {
        return;
    }

    setpoint->vref = g_hmi_param_data.vref;
    setpoint->iref = g_hmi_param_data.iref;
    setpoint->enable_cmd = g_hmi_param_data.enable_cmd;
    setpoint->mode_cmd = g_hmi_param_data.mode_cmd;
}

float HMIParam_GetVref(void)
{
    return g_hmi_param_data.vref;
}

float HMIParam_GetIref(void)
{
    return g_hmi_param_data.iref;
}

uint16_t HMIParam_GetEnableCmd(void)
{
    return g_hmi_param_data.enable_cmd;
}

uint16_t HMIParam_GetModeCmd(void)
{
    return g_hmi_param_data.mode_cmd;
}
