# Simulink 集成说明

## 方式一：C Caller Block

在 Simulink 中通过 C Caller 调用：

```c
HMI_Init();
HMI_Task_10ms();
HMI_Task_100ms();
HMI_Get_Vref();
HMI_Get_Iref();
HMI_Get_EnableCmd();
HMI_Set_Vout(value);
HMI_Set_Iout(value);
HMI_Set_FaultCode(fault);
```

## 方式二：Custom Code

Include headers:

```c
#include "hmi.h"
#include "control_interface.h"
```

Include directories:

```text
../hmi
../drivers_user
../control_if
../app
```

Source files:

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
../control_if/control_interface.c
```

## 方式三：外部 CCS 工程

推荐让 Simulink 只生成控制算法源码，CCS 工程统一编译 C2000Ware、生成代码、HMI 和 app 调度。这样重新生成控制算法时不会覆盖手写 HMI 文件。
