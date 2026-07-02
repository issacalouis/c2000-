#ifndef HMI_PARAM_H
#define HMI_PARAM_H

/*
 * File: hmi_param.h
 * Description: HMI parameter storage and range-limited accessors.
 * Notes: HMI parameters are separated from generated control internals.
 */

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    float vref;
    float iref;
    float vin;
    float vout;
    float iout;
    float duty;
    uint16_t enable_cmd;
    uint16_t mode_cmd;
    uint16_t run_state;
    uint16_t fault_code;
} HMI_Data_t;

void HMI_Param_Init(void);
void HMI_Param_GetData(HMI_Data_t *data);

float HMI_Param_GetVref(void);
float HMI_Param_GetIref(void);
uint16_t HMI_Param_GetEnableCmd(void);
uint16_t HMI_Param_GetModeCmd(void);
uint16_t HMI_Param_GetFaultCode(void);

void HMI_Param_SetVref(float value);
void HMI_Param_SetIref(float value);
void HMI_Param_AdjustVref(float delta);
void HMI_Param_AdjustIref(float delta);
void HMI_Param_ToggleEnable(void);
void HMI_Param_SetModeCmd(uint16_t mode);

void HMI_Param_SetVin(float value);
void HMI_Param_SetVout(float value);
void HMI_Param_SetIout(float value);
void HMI_Param_SetDuty(float value);
void HMI_Param_SetFaultCode(uint16_t fault_code);
void HMI_Param_SetRunState(uint16_t run_state);

#ifdef __cplusplus
}
#endif

#endif
