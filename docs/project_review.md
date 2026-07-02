# 工程审查报告

## 1. 工程目录结构

```text
app/                 应用初始化、全局配置、后台任务调度
hmi/                 HMI 顶层任务、菜单、参数模型、显示页面
drivers_user/        F2800157 板级 GPIO/I2C、按键、OLED 驱动
control_if/          HMI 与控制算法之间的数据交换接口
generated_control/   预留给 Simulink / Embedded Coder 生成代码
docs/                架构、移植、Simulink 集成、ISR 模板文档
tests/               PC 端 mock 测试
```

## 2. 每个 .c/.h 文件的作用

- `app/app_main.c/.h`：实现 `APP_Init()`、1 ms 调度 tick、后台循环任务分发。
- `app/app_config.h`：定义 Vref/Iref 默认值、上下限、任务周期和长按时间。
- `app/app_types.h`：定义故障码、布尔值和运行状态。
- `hmi/hmi.c/.h`：HMI 顶层任务入口，连接菜单、参数、显示和 `control_if`。
- `hmi/hmi_menu.c/.h`：按键驱动的菜单状态机。
- `hmi/hmi_param.c/.h`：HMI 参数存储、限幅、反馈缓存。
- `hmi/hmi_display.c/.h`：按当前页面渲染 OLED 文本内容。
- `drivers_user/board.c/.h`：显式 `Board_Init()`，统一执行 F2800157 板级初始化。
- `drivers_user/board_pinmap.h`：集中定义 LAUNCHXL-F2800157 引脚、I2C、OLED 和键盘参数。
- `drivers_user/board_gpio.c/.h`：4x4 矩阵键盘 GPIO 后端，目标构建读真实 GPIO，`UNIT_TEST` 下使用 mock。
- `drivers_user/board_i2c.c/.h`：I2CA 400 kHz 传输后端，用于 SSD1306 OLED。
- `drivers_user/key.c/.h`：矩阵键盘扫描、消抖、短按/长按事件生成。
- `drivers_user/oled.c/.h`：SSD1306 初始化、文本缓存、帧缓冲渲染和 I2C 刷新。
- `drivers_user/oled_font.c/.h`：保留的字库占位文件；当前实际字形在 `oled.c` 内部。
- `control_if/control_interface.c/.h`：设定值和反馈值共享接口。
- `tests/key_scan_test.c`：按键消抖和长按 mock 测试。
- `tests/hmi_mock_test.c`：HMI 默认值、按键修改、RUN 使能和 OLED 文本刷新 mock 测试。

## 3. 每个模块的职责边界

- `app` 只负责初始化入口和低速调度，不包含菜单或硬件细节。
- `drivers_user` 负责 F2800157 板级外设、按键和 OLED 驱动。
- `hmi` 负责交互逻辑，不直接访问 C2000 寄存器。
- `control_if` 是 HMI 与控制算法之间唯一共享数据层。
- `generated_control` 只放 Simulink 生成控制算法，不放手写 HMI 代码。

## 4. 模块之间的调用关系

```text
main
  -> APP_Init()
       -> Board_Init()
            -> Device_init()
            -> Device_initGPIO()
            -> BoardGPIO_Init()
            -> BoardI2C_Init()
            -> Key_Init()
            -> OLED_Init()
       -> ControlIF_Init()
       -> HMI_Init()
            -> HMI_Param_Init()
            -> HMI_Menu_Init()
            -> HMI_Display_Init()
            -> ControlIF_SetSetpoint()

1 ms ISR/tick
  -> APP_TaskScheduler_1ms_ISR()

while (1)
  -> APP_BackgroundLoop()
       -> HMI_Task_1ms()
       -> HMI_Task_10ms()
            -> Key_Task_10ms()
            -> HMI_Menu_Task_20ms()
            -> ControlIF_SetSetpoint()
       -> HMI_Task_100ms()
            -> ControlIF_GetFeedback()
            -> HMI_Display_Task_100ms()
            -> OLED_Update()
```

## 5. 主程序入口在哪里

仓库内没有 `main.c`。目标 CCS/C2000 工程应在 `main()` 中完成中断和系统级配置后调用：

```c
APP_Init();

while (1)
{
    APP_BackgroundLoop();
}
```

## 6. 初始化顺序是什么

当前初始化顺序已改为显式板级初始化：

1. `APP_Init()`
2. `Board_Init()`
3. `Device_init()`、`Device_initGPIO()`，仅目标构建启用
4. `BoardGPIO_Init()`、`BoardI2C_Init()`
5. `Key_Init()`、`OLED_Init()`
6. `ControlIF_Init()`
7. `HMI_Init()`，只初始化 HMI 参数、菜单、显示状态，并推送默认设定值

## 7. 主循环执行了哪些任务

- 1 ms：`HMI_Task_1ms()`，当前预留。
- 10 ms：`HMI_Task_10ms()`，扫描按键，生成事件，每 20 ms 推进菜单状态机。
- 100 ms：`HMI_Task_100ms()`，同步控制反馈并刷新 OLED。

## 8. 是否使用了中断

仓库提供 `APP_TaskScheduler_1ms_ISR()` 作为 1 ms 中断/tick 入口，只置位任务标志。控制 ISR 模板位于 `docs/control_isr_template.md`，建议只处理 ADC、控制 step、PWM、保护和 `ControlIF_SetFeedback()`。

## 9. 是否使用了定时器

逻辑依赖外部 1 ms 周期 tick，但仓库未提供定时器外设配置。目标工程需要配置 C2000 定时器、ePWM 中断或生成代码调度器，每 1 ms 调用 `APP_TaskScheduler_1ms_ISR()`。

## 10. 是否直接操作 C2000 寄存器

当前手写代码使用 C2000Ware DriverLib，不直接写寄存器。F2800157 相关 GPIO/I2C 配置集中在 `drivers_user/board_gpio.c`、`drivers_user/board_i2c.c` 和 `drivers_user/board_pinmap.h`。

## 11. OLED、按键、菜单、控制接口分别在哪里实现

- OLED：`drivers_user/oled.c/.h`，底层 I2C 在 `drivers_user/board_i2c.c`。
- 按键：`drivers_user/key.c/.h`，GPIO 行列访问在 `drivers_user/board_gpio.c`。
- 菜单：`hmi/hmi_menu.c/.h`。
- 控制接口：`control_if/control_interface.c/.h`。

## 12. 哪些文件是可以由用户修改的

可修改 `app/`、`hmi/`、`drivers_user/`、`control_if/`、`docs/` 和 `tests/` 下的手写文件。硬件引脚和外设参数优先改 `drivers_user/board_pinmap.h`；板级行为改 `board_gpio.c`、`board_i2c.c`。

## 13. 哪些文件未来可能被 Simulink 生成代码替换

仅 `generated_control/` 下文件应由 Simulink / Embedded Coder 生成或覆盖，例如 `model.c`、`model.h`、`model_private.h`、`model_data.c`、`rtwtypes.h`。`app/`、`hmi/`、`drivers_user/`、`control_if/` 不应被生成代码替换。

## 14. 当前工程的潜在风险点

- 仍缺少目标工程 `main.c`、链接脚本、中断向量和 CCS 工程配置。
- `BoardI2C_Write()` 当前是阻塞轮询式发送，适合 HMI 低速任务，不应放入控制 ISR。
- SSD1306 字形表是内置 5x7 简化字库，小写会按大写渲染，中文不支持。
- `control_interface.c` 已采用双缓冲和序列号校验来保证多字段共享数据的快照一致性，但目标工程仍应结合实时性要求评估是否需要进一步的原子性或时序保护。
- 矩阵键盘只映射 7 个功能键，其余 9 个位置未定义。
- 当前未处理矩阵键盘多键鬼键问题，默认按单键优先处理。
- 目标构建依赖 C2000Ware DriverLib 中的 F2800157 pinmux/I2C API 名称，需要在 CCS 中最终编译确认。

## 15. 当前工程的编译前置条件

PC mock 测试需要 C 编译器、标准 C 库、include path 和 `UNIT_TEST` 宏：

```sh
gcc -DUNIT_TEST -Iapp -Ihmi -Idrivers_user -Icontrol_if tests/key_scan_test.c drivers_user/key.c drivers_user/board_gpio.c -o key_scan_test
gcc -DUNIT_TEST -Iapp -Ihmi -Idrivers_user -Icontrol_if tests/hmi_mock_test.c hmi/*.c drivers_user/*.c control_if/control_interface.c -lm -o hmi_mock_test
```

C2000 目标工程需要 TI C2000 编译器、Code Composer Studio 或等效工程、C2000Ware、启动文件、链接脚本、中断向量、1 ms tick 配置，以及 `TMS320F2800157` 目标芯片设置。
