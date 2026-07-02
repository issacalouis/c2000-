/*
 * 文件: control_interface.c
 * 说明: 用于 HMI 与控制侧交换的共享设定值和反馈存储。
 * 备注: 不包含阻塞调用。在目标平台上，如果该模块会被 ISR 和后台上下文
 *       同时访问，需要对多字更新进行保护。
 */

#include "control_interface.h"

#include "app_config.h"
#include "app_types.h"

static volatile Control_Setpoint_t g_controlif_setpoint;
static volatile Control_Feedback_t g_controlif_feedback;

/*
 * 函数: ControlIF_Init
 * 调用周期: 在 APP_Init() 期间调用一次。
 * ISR: 在中断开启前允许调用；避免在 ISR 中重复调用。
 * 阻塞: 否。
 */
void ControlIF_Init(void)
{
    g_controlif_setpoint.vref = APP_VREF_DEFAULT;
    g_controlif_setpoint.iref = APP_IREF_DEFAULT;
    g_controlif_setpoint.enable_cmd = APP_FALSE;
    g_controlif_setpoint.mode_cmd = 0u;

    g_controlif_feedback.vin = 0.0f;
    g_controlif_feedback.vout = 0.0f;
    g_controlif_feedback.iout = 0.0f;
    g_controlif_feedback.duty = 0.0f;
    g_controlif_feedback.run_state = APP_RUN_STATE_STOP;
    g_controlif_feedback.fault_code = FAULT_NONE;
}

/*
 * 函数: ControlIF_SetSetpoint
 * 调用周期: 低速 HMI 任务中，在用户参数变化后调用。
 * ISR: 不推荐，因为并非所有 C2000 上的多字段写入都是原子的。
 * 阻塞: 否。
 */
void ControlIF_SetSetpoint(const Control_Setpoint_t *setpoint)
{
    if (setpoint == 0)
    {
        return;
    }

    g_controlif_setpoint = *setpoint;
}

/*
 * 函数: ControlIF_GetSetpoint
 * 调用周期: 控制步进函数或生成代码封装层中调用。
 * ISR: 允许；如果应用需要在 HMI 并发写入期间获取一致的多字段快照，
 *      应增加临界区保护。
 * 阻塞: 否。
 */
void ControlIF_GetSetpoint(Control_Setpoint_t *setpoint)
{
    if (setpoint == 0)
    {
        return;
    }

    *setpoint = g_controlif_setpoint;
}

/*
 * 函数: ControlIF_SetFeedback
 * 调用周期: 控制 ISR 中，在 ADC/控制/PWM 更新后调用。
 * ISR: 允许。
 * 阻塞: 否。
 */
void ControlIF_SetFeedback(const Control_Feedback_t *feedback)
{
    if (feedback == 0)
    {
        return;
    }

    g_controlif_feedback = *feedback;
}

/*
 * 函数: ControlIF_GetFeedback
 * 调用周期: 低速 HMI 显示任务中调用。
 * ISR: 不需要。
 * 阻塞: 否。
 */
void ControlIF_GetFeedback(Control_Feedback_t *feedback)
{
    if (feedback == 0)
    {
        return;
    }

    *feedback = g_controlif_feedback;
}
