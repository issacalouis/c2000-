#ifndef CONTROL_INTERFACE_H
#define CONTROL_INTERFACE_H

/*
 * 文件: control_interface.h
 * 说明: HMI 代码与生成控制代码之间的数据交换 API。
 * 备注: Simulink 生成文件应调用本 API，而不是直接访问 HMI 内部数据。
 *       该实现为非阻塞方式。
 */

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    float vref;
    float iref;
    uint16_t enable_cmd;
    uint16_t mode_cmd;
} Control_Setpoint_t;

typedef struct
{
    float vin;
    float vout;
    float iout;
    float duty;
    uint16_t run_state;
    uint16_t fault_code;
} Control_Feedback_t;

void ControlIF_Init(void);

void ControlIF_SetSetpoint(const Control_Setpoint_t *setpoint);
void ControlIF_GetSetpoint(Control_Setpoint_t *setpoint);

void ControlIF_SetFeedback(const Control_Feedback_t *feedback);
void ControlIF_GetFeedback(Control_Feedback_t *feedback);

#ifdef __cplusplus
}
#endif

#endif
