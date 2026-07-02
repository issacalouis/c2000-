# C2000 Power HMI Project

这是一个面向 `TMS320F2800157` / `LAUNCHXL-F2800157` 的电源控制 HMI 工程模板。控制算法主体预留给 Simulink / Embedded Coder 生成，键盘、OLED、菜单、参数和控制接口由本工程维护。

## 设计原则

- 不直接修改 `generated_control` 中的 Simulink 生成文件。
- 高速控制 ISR 只处理 ADC、控制计算、PWM、保护和反馈写入。
- 按键扫描、菜单处理、字符串格式化和 OLED 刷新只在低速后台任务中执行。
- HMI 通过 `control_if` 中的固定接口与控制算法交换数据。
- 用户设定值范围集中放在 `app/app_config.h`。

## 目录说明

- `app`：应用初始化和 1 ms / 10 ms / 20 ms / 100 ms 调度。
- `hmi`：HMI 顶层接口、菜单状态机、参数模型和显示页面。
- `drivers_user`：F2800157 板级初始化、GPIO/I2C、Key、OLED 驱动。
- `control_if`：HMI 与 Simulink 控制算法之间的数据接口。
- `generated_control`：放置 Simulink 自动生成代码，手写代码不要改这里。
- `docs`：架构、移植、Simulink 集成和控制 ISR 模板。
- `tests`：可在 PC 上编译运行的 mock 测试。

## 典型调度

```c
APP_Init();

while (1)
{
    APP_BackgroundLoop();
}
```

定时器 ISR 每 1 ms 调用：

```c
APP_TaskScheduler_1ms_ISR();
```

控制 ISR 应独立调用 Simulink 生成的 step 函数，并通过 `ControlIF_SetFeedback()` 写入 Vin/Vout/Iout/Duty/Fault。

## 当前硬件状态

- OLED：SSD1306 128x64，I2CA，SDA=`GPIO42`，SCL=`GPIO43`，地址 `0x3C`，400 kHz。
- 矩阵键盘：4x4，行 `GPIO25/26/27/44`，列 `GPIO45/46/48/33`，列上拉，低有效。
- 板级参数集中在 `drivers_user/board_pinmap.h`。
- `Board_Init()` 统一执行 `Device_init()`、`Device_initGPIO()`、GPIO/I2C、Key 和 OLED 初始化。
