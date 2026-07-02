/*
 * File: hmi_param.c
 * Description: Central HMI data model with setpoint clamping.
 * Notes: No hardware access and no blocking calls are used here.
 */

#include "hmi_param.h"

#include "app_config.h"
#include "app_types.h"

static HMI_Data_t g_hmi_param_data;

static float HMI_Param_Clamp(float value, float min_value, float max_value)
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

/*
 * Function: HMI_Param_Init
 * Call period: once from HMI_Init().
 * ISR: no.
 * Blocking: no.
 */
void HMI_Param_Init(void)
{
    g_hmi_param_data.vref = APP_VREF_DEFAULT;
    g_hmi_param_data.iref = APP_IREF_DEFAULT;
    g_hmi_param_data.vin = 0.0f;
    g_hmi_param_data.vout = 0.0f;
    g_hmi_param_data.iout = 0.0f;
    g_hmi_param_data.duty = 0.0f;
    g_hmi_param_data.enable_cmd = APP_FALSE;
    g_hmi_param_data.mode_cmd = 0u;
    g_hmi_param_data.run_state = APP_RUN_STATE_STOP;
    g_hmi_param_data.fault_code = FAULT_NONE;
}

void HMI_Param_GetData(HMI_Data_t *data)
{
    if (data != 0)
    {
        *data = g_hmi_param_data;
    }
}

float HMI_Param_GetVref(void)
{
    return g_hmi_param_data.vref;
}

float HMI_Param_GetIref(void)
{
    return g_hmi_param_data.iref;
}

uint16_t HMI_Param_GetEnableCmd(void)
{
    return g_hmi_param_data.enable_cmd;
}

uint16_t HMI_Param_GetModeCmd(void)
{
    return g_hmi_param_data.mode_cmd;
}

uint16_t HMI_Param_GetFaultCode(void)
{
    return g_hmi_param_data.fault_code;
}

void HMI_Param_SetVref(float value)
{
    g_hmi_param_data.vref = HMI_Param_Clamp(value, APP_VREF_MIN, APP_VREF_MAX);
}

void HMI_Param_SetIref(float value)
{
    g_hmi_param_data.iref = HMI_Param_Clamp(value, APP_IREF_MIN, APP_IREF_MAX);
}

void HMI_Param_AdjustVref(float delta)
{
    HMI_Param_SetVref(g_hmi_param_data.vref + delta);
}

void HMI_Param_AdjustIref(float delta)
{
    HMI_Param_SetIref(g_hmi_param_data.iref + delta);
}

void HMI_Param_ToggleEnable(void)
{
    g_hmi_param_data.enable_cmd = (g_hmi_param_data.enable_cmd == 0u) ? 1u : 0u;
}

void HMI_Param_SetModeCmd(uint16_t mode)
{
    g_hmi_param_data.mode_cmd = mode;
}

void HMI_Param_SetVin(float value)
{
    g_hmi_param_data.vin = value;
}

void HMI_Param_SetVout(float value)
{
    g_hmi_param_data.vout = value;
}

void HMI_Param_SetIout(float value)
{
    g_hmi_param_data.iout = value;
}

void HMI_Param_SetDuty(float value)
{
    g_hmi_param_data.duty = HMI_Param_Clamp(value, 0.0f, 100.0f);
}

void HMI_Param_SetFaultCode(uint16_t fault_code)
{
    g_hmi_param_data.fault_code = fault_code;
}

void HMI_Param_SetRunState(uint16_t run_state)
{
    g_hmi_param_data.run_state = run_state;
}
