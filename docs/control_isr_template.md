# 控制 ISR 模板

下面是高速控制中断的建议结构。此模板只描述边界，不绑定具体 Simulink 模型名或 C2000 寄存器。

```c
#include "control_interface.h"

__interrupt void ControlISR(void)
{
    Control_Setpoint_t setpoint;
    Control_Feedback_t feedback;

    /*
     * 1. Read ADC values.
     * 2. Read HMI setpoint through ControlIF.
     * 3. Call Simulink generated control step function.
     * 4. Update PWM compare registers.
     * 5. Run necessary fast software protection.
     * 6. Publish feedback through ControlIF.
     * 7. Clear interrupt flags.
     */

    ControlIF_GetSetpoint(&setpoint);

    /* TODO: map setpoint into generated model inputs. */
    /* TODO: call model_step(); */
    /* TODO: map generated model outputs to PWM/CMPSS/feedback. */

    ControlIF_SetFeedback(&feedback);

    /*
     * Forbidden here:
     * - OLED_Update()
     * - Key_Task_10ms()
     * - HMI_Task_10ms()
     * - HMI_Task_100ms()
     * - printf()
     * - blocking delay
     */
}
```
