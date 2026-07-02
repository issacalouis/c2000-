/*
 * 文件: control_interface.c
 * 说明: 用于 HMI 与控制侧交换的共享设定值和反馈存储。
 * 备注: 不包含阻塞调用。通过双缓冲和序列号校验，为 ISR 与后台并发访问
 *       提供一致的多字段快照读取。
 */

#include "control_interface.h"

#include "app_config.h"
#include "app_types.h"

typedef struct
{
    volatile uint16_t sequence;
    volatile uint16_t active_index;
    Control_Setpoint_t buffer[2];
} ControlIF_SetpointStore_t;

typedef struct
{
    volatile uint16_t sequence;
    volatile uint16_t active_index;
    Control_Feedback_t buffer[2];
} ControlIF_FeedbackStore_t;

static ControlIF_SetpointStore_t g_controlif_setpoint;
static ControlIF_FeedbackStore_t g_controlif_feedback;

static void ControlIF_WriteSetpoint(const Control_Setpoint_t *setpoint)
{
    uint16_t next_index;

    next_index = (uint16_t)(g_controlif_setpoint.active_index ^ 1u);
    g_controlif_setpoint.sequence++;
    g_controlif_setpoint.buffer[next_index] = *setpoint;
    g_controlif_setpoint.active_index = next_index;
    g_controlif_setpoint.sequence++;
}

static void ControlIF_WriteFeedback(const Control_Feedback_t *feedback)
{
    uint16_t next_index;

    next_index = (uint16_t)(g_controlif_feedback.active_index ^ 1u);
    g_controlif_feedback.sequence++;
    g_controlif_feedback.buffer[next_index] = *feedback;
    g_controlif_feedback.active_index = next_index;
    g_controlif_feedback.sequence++;
}

/*
 * 函数: ControlIF_Init
 * 调用周期: 在 APP_Init() 期间调用一次。
 * ISR: 在中断开启前允许调用；避免在 ISR 中重复调用。
 * 阻塞: 否。
 */
void ControlIF_Init(void)
{
    Control_Setpoint_t setpoint_default;
    Control_Feedback_t feedback_default;

    setpoint_default.vref = APP_VREF_DEFAULT;
    setpoint_default.iref = APP_IREF_DEFAULT;
    setpoint_default.enable_cmd = APP_FALSE;
    setpoint_default.mode_cmd = 0u;

    feedback_default.vin = 0.0f;
    feedback_default.vout = 0.0f;
    feedback_default.iout = 0.0f;
    feedback_default.duty = 0.0f;
    feedback_default.run_state = APP_RUN_STATE_STOP;
    feedback_default.fault_code = FAULT_NONE;

    g_controlif_setpoint.sequence = 0u;
    g_controlif_setpoint.active_index = 0u;
    g_controlif_setpoint.buffer[0] = setpoint_default;
    g_controlif_setpoint.buffer[1] = setpoint_default;

    g_controlif_feedback.sequence = 0u;
    g_controlif_feedback.active_index = 0u;
    g_controlif_feedback.buffer[0] = feedback_default;
    g_controlif_feedback.buffer[1] = feedback_default;
}

/*
 * 函数: ControlIF_SetSetpoint
 * 调用周期: 低速 HMI 任务中，在用户参数变化后调用。
 * ISR: 不推荐，因为设定值通常由低速 HMI 任务更新。
 * 阻塞: 否。
 */
void ControlIF_SetSetpoint(const Control_Setpoint_t *setpoint)
{
    if (setpoint == 0)
    {
        return;
    }

    ControlIF_WriteSetpoint(setpoint);
}

/*
 * 函数: ControlIF_GetSetpoint
 * 调用周期: 控制步进函数或生成代码封装层中调用。
 * ISR: 允许。通过双缓冲和序列号校验返回一致的多字段快照。
 * 阻塞: 否。
 */
void ControlIF_GetSetpoint(Control_Setpoint_t *setpoint)
{
    uint16_t sequence_start;
    uint16_t sequence_end;
    uint16_t active_index;

    if (setpoint == 0)
    {
        return;
    }

    do
    {
        sequence_start = g_controlif_setpoint.sequence;
        if ((sequence_start & 1u) != 0u)
        {
            continue;
        }

        active_index = g_controlif_setpoint.active_index;
        *setpoint = g_controlif_setpoint.buffer[active_index];
        sequence_end = g_controlif_setpoint.sequence;
    } while ((sequence_start != sequence_end) || ((sequence_end & 1u) != 0u));
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

    ControlIF_WriteFeedback(feedback);
}

/*
 * 函数: ControlIF_GetFeedback
 * 调用周期: 低速 HMI 显示任务中调用。
 * ISR: 不需要。通过双缓冲和序列号校验返回一致的多字段快照。
 * 阻塞: 否。
 */
void ControlIF_GetFeedback(Control_Feedback_t *feedback)
{
    uint16_t sequence_start;
    uint16_t sequence_end;
    uint16_t active_index;

    if (feedback == 0)
    {
        return;
    }

    do
    {
        sequence_start = g_controlif_feedback.sequence;
        if ((sequence_start & 1u) != 0u)
        {
            continue;
        }

        active_index = g_controlif_feedback.active_index;
        *feedback = g_controlif_feedback.buffer[active_index];
        sequence_end = g_controlif_feedback.sequence;
    } while ((sequence_start != sequence_end) || ((sequence_end & 1u) != 0u));
}
