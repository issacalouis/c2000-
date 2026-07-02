# Simulink 集成说明

## 原则

不要手动修改 Simulink / Embedded Coder 自动生成的文件，例如：

```text
model.c
model.h
model_private.h
model_data.c
ert_main.c
rtwtypes.h
```

HMI 通过 `hmi.h` 和 `control_interface.h` 提供稳定接口，生成代码只调用这些接口或由外部 CCS 工程统一链接。

## 方式一：C Caller Block

可在 Simulink 中使用 C Caller 调用：

```c
HMI_Init();
HMI_Task_10ms();
HMI_Task_100ms();

HMI_Get_Vref();
HMI_Get_Iref();
HMI_Get_EnableCmd();
HMI_Get_ModeCmd();

HMI_Set_Vout(value);
HMI_Set_Iout(value);
HMI_Set_FaultCode(fault);
```

更推荐控制算法通过 ControlIF：

```c
Control_Setpoint_t sp;
Control_Feedback_t fb;

ControlIF_GetSetpoint(&sp);
/* Simulink step uses sp.vref, sp.iref, sp.enable_cmd, sp.mode_cmd */

fb.vin = vin;
fb.vout = vout;
fb.iout = iout;
fb.duty = duty;
fb.run_state = run_state;
fb.fault_code = fault_code;
ControlIF_SetFeedback(&fb);
```

## 方式二：Custom Code 配置

在 Model Settings 中加入头文件：

```c
#include "hmi.h"
#include "control_interface.h"
```

Include directories：

```text
../hmi
../drivers_user
../control_if
../app
```

Source files：

```text
../hmi/hmi.c
../hmi/hmi_menu.c
../hmi/hmi_param.c
../hmi/hmi_display.c
../drivers_user/key.c
../drivers_user/oled.c
../drivers_user/oled_font.c
../drivers_user/board_gpio.c
../drivers_user/board_i2c.c
../drivers_user/board_spi.c
../control_if/control_interface.c
../app/app_main.c
```

## 方式三：外部 CCS 工程

让 Simulink 只生成控制算法源码，由 CCS 工程同时包含：

```text
Simulink 生成代码
HMI 模板源码
C2000Ware driverlib/device 文件
工程板级初始化代码
```

这种方式通常更适合实际电源控制项目，因为时钟、GPIO、PWM、ADC、CMPSS、Trip Zone 和中断优先级都可以在 CCS 工程中集中管理。

## 推荐控制 ISR 结构

```c
__interrupt void ControlISR(void)
{
    Control_Setpoint_t sp;
    Control_Feedback_t fb;

    /* 1. Read ADC values. */
    ControlIF_GetSetpoint(&sp);

    /* 2. Call Simulink generated control step function. */
    /* model_step(); */

    /* 3. Update PWM compare registers. */

    /* 4. Publish feedback for HMI. */
    ControlIF_SetFeedback(&fb);

    /* 5. Clear interrupt flags. */
}
```

禁止在这个 ISR 中调用屏幕刷新、按键扫描、菜单绘制或阻塞延时。
