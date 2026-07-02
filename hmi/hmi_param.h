/*
 * File: hmi_param.h
 * Description: HMI parameter storage and limit helpers.
 * Notes: Menu code updates setpoints here; display code reads snapshots here.
 */
#ifndef HMI_PARAM_H
#define HMI_PARAM_H

#include <stdint.h>
#include "control_interface.h"

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

void HMIParam_Init(void);
void HMIParam_GetSnapshot(HMI_Data_t *data);
void HMIParam_SetVref(float value);
void HMIParam_SetIref(float value);
void HMIParam_SetEnableCmd(uint16_t value);
void HMIParam_SetModeCmd(uint16_t value);
void HMIParam_SetFeedback(const Control_Feedback_t *feedback);
void HMIParam_ToSetpoint(Control_Setpoint_t *setpoint);

float HMIParam_GetVref(void);
float HMIParam_GetIref(void);
uint16_t HMIParam_GetEnableCmd(void);
uint16_t HMIParam_GetModeCmd(void);

#ifdef __cplusplus
}
#endif

#endif
