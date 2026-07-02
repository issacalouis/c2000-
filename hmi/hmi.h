#ifndef HMI_H
#define HMI_H

/*
 * File: hmi.h
 * Description: Public HMI API used by app code and generated-control glue.
 * Notes: The HMI API exchanges data through stable functions, not through
 *        generated Simulink internal variables.
 */

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void HMI_Init(void);
void HMI_Task_1ms(void);
void HMI_Task_10ms(void);
void HMI_Task_100ms(void);

float HMI_Get_Vref(void);
float HMI_Get_Iref(void);
uint16_t HMI_Get_EnableCmd(void);
uint16_t HMI_Get_ModeCmd(void);

void HMI_Set_Vin(float vin);
void HMI_Set_Vout(float vout);
void HMI_Set_Iout(float iout);
void HMI_Set_Duty(float duty);
void HMI_Set_FaultCode(uint16_t fault_code);
void HMI_Set_RunState(uint16_t run_state);

#ifdef __cplusplus
}
#endif

#endif
