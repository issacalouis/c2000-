/*
 * File: hmi_mock_test.c
 * Description: Simple PC-side smoke test for HMI defaults and ControlIF flow.
 * Notes: Build with the HMI sources and -DUNIT_TEST.
 */
#include "hmi.h"
#include "control_interface.h"
#include "app_config.h"
#include "app_types.h"
#include <assert.h>

int main(void)
{
    Control_Feedback_t feedback;
    Control_Setpoint_t setpoint;

    HMI_Init();

    assert(HMI_Get_Vref() == APP_VREF_DEFAULT);
    assert(HMI_Get_Iref() == APP_IREF_DEFAULT);
    assert(HMI_Get_EnableCmd() == APP_ENABLE_OFF);

    feedback.vin = 24.0f;
    feedback.vout = 12.0f;
    feedback.iout = 1.5f;
    feedback.duty = 42.0f;
    feedback.run_state = APP_RUN_STATE_RUN;
    feedback.fault_code = FAULT_NONE;
    ControlIF_SetFeedback(&feedback);

    HMI_Task_1ms();
    HMI_Task_100ms();

    HMI_Task_10ms();
    ControlIF_GetSetpoint(&setpoint);
    assert(setpoint.vref == APP_VREF_DEFAULT);
    assert(setpoint.iref == APP_IREF_DEFAULT);
    assert(setpoint.enable_cmd == APP_ENABLE_OFF);

    return 0;
}
