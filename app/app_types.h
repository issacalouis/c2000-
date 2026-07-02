/*
 * File: app_types.h
 * Description: Shared application constants and simple project types.
 * Notes: Fault codes are shared by HMI, ControlIF, and generated control code.
 */
#ifndef APP_TYPES_H
#define APP_TYPES_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define FAULT_NONE      0x0000u
#define FAULT_OVP       0x0001u
#define FAULT_OCP       0x0002u
#define FAULT_OTP       0x0004u
#define FAULT_UVLO      0x0008u
#define FAULT_ADC       0x0010u
#define FAULT_PWM       0x0020u

typedef enum
{
    APP_RUN_STATE_STOP = 0,
    APP_RUN_STATE_RUN = 1,
    APP_RUN_STATE_FAULT = 2
} APP_RunState_t;

#ifdef __cplusplus
}
#endif

#endif
