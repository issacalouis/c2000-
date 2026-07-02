# C2000 电源 HMI 工程模板

本工程是面向 TI C2000 电源控制项目的可移植 HMI 模板，包含按键扫描、菜单、OLED 显示抽象、参数管理、控制算法接口和低速调度框架。

当前工程没有 Simulink / Embedded Coder 生成的控制算法代码，因此 `generated_control/` 只保留占位说明。后续生成算法代码后，应放入该目录或由外部 CCS 工程引用，HMI 代码不直接修改任何生成文件。

## 目录结构

```text
app/                主程序调度、工程配置、公共类型
hmi/                HMI 顶层、菜单、参数、显示页面
drivers_user/       GPIO/I2C/SPI/OLED/按键等硬件抽象
control_if/         HMI 与控制算法之间的固定数据接口
generated_control/  Simulink 生成代码占位目录
docs/               架构、移植和 Simulink 集成说明
tests/              PC 侧 mock 测试示例
```

## 核心运行方式

高速控制 ISR 只做 ADC、控制 step、PWM 更新、保护判断和接口数据写入。按键扫描、菜单处理和屏幕刷新全部在低速后台任务中执行。

推荐调度周期：

```text
1 ms   HMI_Task_1ms      同步控制反馈
10 ms  HMI_Task_10ms     按键扫描、菜单处理、设定值发布
100 ms HMI_Task_100ms    屏幕刷新
```

## 数据接口

HMI 到控制算法：

```c
ControlIF_GetSetpoint(&setpoint);
```

控制算法到 HMI：

```c
ControlIF_SetFeedback(&feedback);
```

HMI 不直接访问 Simulink 模型内部变量，Simulink 生成文件也不需要手动修改。

## 后续移植重点

1. 在 `drivers_user/board_gpio.c` 中补充实际按键 GPIO。
2. 在 `drivers_user/board_i2c.c` 或 `drivers_user/board_spi.c` 中补充屏幕总线驱动。
3. 在 `drivers_user/oled.c` 中把文本缓冲转换为 SSD1306 页面刷新。
4. 在控制 ISR 中调用 Simulink step，并通过 `control_if` 交换设定值和反馈值。
5. 如 HMI 后台和控制 ISR 并发访问多字节数据，在 `control_if/control_interface.c` 中接入临界区宏。

更多细节见 `docs/`。
