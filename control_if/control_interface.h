#ifndef CONTROL_INTERFACE_H
#define CONTROL_INTERFACE_H

/*
 * File: control_interface.h
 * Description: Data exchange API between HMI code and generated control code.
 * Notes: Simulink generated files should call this API instead of touching HMI
 *        internals. The implementation is non-blocking.
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
