#ifndef HMI_H
#define HMI_H

/*
 * 文件: hmi.h
 * 说明: 供应用代码和生成控制胶水层使用的公开 HMI API。
 * 备注: HMI API 应通过稳定的函数接口交换数据，而不是直接依赖
 *       Simulink 生成的内部变量。
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
