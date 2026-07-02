#ifndef APP_TYPES_H
#define APP_TYPES_H

/*
 * File: app_types.h
 * Description: Shared application-level constants and lightweight types.
 * Notes: This file intentionally has no hardware dependency.
 */

#include <stdint.h>

#define APP_FALSE 0u
#define APP_TRUE  1u

#define FAULT_NONE 0x0000u
#define FAULT_OVP  0x0001u
#define FAULT_OCP  0x0002u
#define FAULT_OTP  0x0004u
#define FAULT_UVLO 0x0008u
#define FAULT_ADC  0x0010u
#define FAULT_PWM  0x0020u

typedef enum
{
    APP_RUN_STATE_STOP = 0,
    APP_RUN_STATE_RUN,
    APP_RUN_STATE_FAULT
} APP_RunState_t;

#endif
