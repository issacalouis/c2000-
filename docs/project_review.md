# 工程审查报告

## 1. 工程目录结构

```text
app/                 应用初始化、全局配置、后台任务调度
hmi/                 HMI 顶层任务、菜单、参数模型、显示页面
drivers_user/        GPIO、I2C、按键、OLED 等用户驱动抽象
control_if/          HMI 与控制算法之间的数据交换接口
generated_control/   预留给 Simulink / Embedded Coder 生成代码
docs/                架构、移植、Simulink 集成、ISR 模板文档
tests/               PC 端 mock 测试
```

根目录还包含 `README.md`、`AGENTS.md` 和 `C2000_Power_HMI_Codex_Guide.md` 等说明文档。

## 2. 每个 .c/.h 文件的作用

### app

- `app/app_main.c`：实现 `APP_Init()`、1 ms 调度标志置位函数、后台循环任务分发。
- `app/app_main.h`：声明应用初始化和调度 API。
- `app/app_config.h`：定义 Vref/Iref 默认值、上下限、任务周期、长按时间等全局配置。
- `app/app_types.h`：定义通用布尔值、故障码和运行状态枚举。

### hmi

- `hmi/hmi.c`：HMI 顶层胶水层，连接参数、按键、菜单、显示和 `control_if`。
- `hmi/hmi.h`：对 app 或生成代码暴露 HMI 初始化、周期任务和参数访问 API。
- `hmi/hmi_menu.c`：按键驱动的菜单状态机，不直接访问硬件或控制算法。
- `hmi/hmi_menu.h`：定义菜单页面枚举和菜单任务接口。
- `hmi/hmi_param.c`：HMI 参数集中存储、默认值初始化、设定值限幅和反馈值保存。
- `hmi/hmi_param.h`：定义 `HMI_Data_t` 和参数读写接口。
- `hmi/hmi_display.c`：根据当前菜单页和参数数据渲染 OLED 文本页面。
- `hmi/hmi_display.h`：声明显示初始化和 100 ms 刷新入口。

### drivers_user

- `drivers_user/key.c`：实现 4x4 矩阵键盘扫描、消抖、短按/长按事件生成。
- `drivers_user/key.h`：定义按键模式、按键 mask、按键事件和按键模块 API。
- `drivers_user/board_gpio.c`：GPIO 隔离层，目前是安全 stub 和 UNIT_TEST mock；待接入 C2000Ware GPIO。
- `drivers_user/board_gpio.h`：声明独立按键、矩阵行列和 mock 接口。
- `drivers_user/oled.c`：SSD1306 风格 OLED 文本缓存和显示 API，占位调用 I2C 写接口。
- `drivers_user/oled.h`：声明 OLED 初始化、清屏、刷新和文本/数字显示接口。
- `drivers_user/oled_font.c`：6x8 字库占位表。
- `drivers_user/oled_font.h`：声明字库表。
- `drivers_user/board_i2c.c`：I2C 传输占位层，待实现 C2000Ware I2C 初始化和发送。
- `drivers_user/board_i2c.h`：声明 I2C 初始化和写接口。

### control_if

- `control_if/control_interface.c`：保存 HMI 设定值和控制反馈值，作为 HMI 与控制算法共享数据层。
- `control_if/control_interface.h`：定义 `Control_Setpoint_t`、`Control_Feedback_t` 和读写接口。

### tests

- `tests/key_scan_test.c`：PC mock 测试按键消抖和 RUN 长按事件。
- `tests/hmi_mock_test.c`：PC mock 测试 HMI 默认值、按键修改设定值、RUN 使能和 OLED 文本刷新。

## 3. 每个模块的职责边界

- `app`：只负责初始化顺序和低速任务调度，不处理具体菜单、显示或控制算法。
- `hmi`：负责人机交互逻辑，不直接操作 C2000 寄存器。
- `drivers_user`：集中封装板级 GPIO/I2C/OLED/按键差异。
- `control_if`：是 HMI 与控制算法之间唯一共享数据接口。
- `generated_control`：仅放自动生成的控制算法代码，不放手写 HMI 逻辑。
- `tests`：只用于 PC mock 验证，不依赖真实 C2000 硬件。

## 4. 模块之间的调用关系

```text
目标 main
  -> APP_Init()
       -> ControlIF_Init()
       -> HMI_Init()
            -> HMI_Param_Init()
            -> Key_Init()
                 -> BoardGPIO_Init()
            -> OLED_Init()
                 -> BoardI2C_Init()
            -> HMI_Menu_Init()
            -> HMI_Display_Init()
            -> ControlIF_SetSetpoint()

1 ms 定时 ISR 或调度 tick
  -> APP_TaskScheduler_1ms_ISR()
       -> 只置位 1 ms / 10 ms / 100 ms 标志

main while(1)
  -> APP_BackgroundLoop()
       -> HMI_Task_1ms()
       -> HMI_Task_10ms()
            -> Key_Task_10ms()
                 -> BoardGPIO_SetMatrixRow()
                 -> BoardGPIO_ReadMatrixColumn()
            -> Key_GetEvent()
            -> HMI_Menu_Task_20ms()
            -> ControlIF_SetSetpoint()
       -> HMI_Task_100ms()
            -> ControlIF_GetFeedback()
            -> HMI_Display_Task_100ms()
                 -> OLED_Clear()
                 -> OLED_ShowString()/OLED_ShowFloat()/OLED_ShowInt()
                 -> OLED_Update()
                      -> BoardI2C_Write()
```

控制 ISR 或 Simulink step 预期通过 `ControlIF_GetSetpoint()` 读取设定值，通过 `ControlIF_SetFeedback()` 写入反馈值。

## 5. 主程序入口在哪里

当前仓库没有实际 `main.c`。预期目标工程的主入口在外部 CCS/C2000 工程中，典型结构是：

```c
APP_Init();

while (1)
{
    APP_BackgroundLoop();
}
```

`APP_Init()` 和 `APP_BackgroundLoop()` 在 `app/app_main.c` 中实现。

## 6. 初始化顺序是什么

当前初始化顺序为：

1. 外部硬件基础初始化：时钟、GPIO 复用、外设基础配置等，仓库内尚未实现。
2. 调用 `APP_Init()`。
3. `ControlIF_Init()` 初始化设定值和反馈值。
4. `HMI_Init()` 初始化 HMI 参数、按键、OLED、菜单、显示。
5. `HMI_Init()` 末尾将默认设定值推送到 `control_if`。

## 7. 主循环执行了哪些任务

主循环应反复调用 `APP_BackgroundLoop()`。该函数根据 ISR 设置的标志执行：

- 1 ms：`HMI_Task_1ms()`，当前为空，预留给轻量非阻塞计时任务。
- 10 ms：`HMI_Task_10ms()`，执行按键扫描、事件读取，并每 20 ms 推进菜单状态机。
- 100 ms：`HMI_Task_100ms()`，同步控制反馈并刷新 OLED 页面。

## 8. 是否使用了中断

仓库内没有完整中断向量或真实 ISR 实现，但已有两个中断相关设计点：

- `APP_TaskScheduler_1ms_ISR()` 设计为由 1 ms 定时器 ISR 或生成调度 tick 调用，只置位任务标志。
- `docs/control_isr_template.md` 提供控制 ISR 模板，建议在控制 ISR 中执行 ADC、控制 step、PWM 更新和 `ControlIF_SetFeedback()`。

按键扫描、OLED 刷新、字符串格式化和阻塞操作明确不应放入控制 ISR。

## 9. 是否使用了定时器

工程逻辑依赖一个外部 1 ms 周期 tick，但仓库内没有定时器外设配置代码。目标工程需要配置 C2000 定时器、ePWM 中断或生成代码调度器，每 1 ms 调用 `APP_TaskScheduler_1ms_ISR()`。

## 10. 是否直接操作 C2000 寄存器

当前源码没有直接操作 C2000 寄存器，也没有包含 C2000Ware 头文件。`board_gpio.c`、`board_i2c.c` 中只有 `TODO(F280015x)` 占位注释。真实硬件移植时，寄存器或 C2000Ware 调用应集中放在这些 board 层文件中。

## 11. OLED、按键、菜单、控制接口分别在哪里实现

- OLED：`drivers_user/oled.c`、`drivers_user/oled.h`，底层 I2C 在 `drivers_user/board_i2c.c`。
- 按键：`drivers_user/key.c`、`drivers_user/key.h`，GPIO 行列访问在 `drivers_user/board_gpio.c`。
- 菜单：`hmi/hmi_menu.c`、`hmi/hmi_menu.h`。
- 控制接口：`control_if/control_interface.c`、`control_if/control_interface.h`。

## 12. 哪些文件是可以由用户修改的

可由用户维护的手写文件包括：

- `app/`：调度、参数范围、通用类型。
- `hmi/`：菜单、参数模型、显示页面、HMI API。
- `drivers_user/`：板级驱动适配和 OLED/按键实现。
- `control_if/`：HMI 与控制算法的数据交换结构及保护策略。
- `docs/`：项目说明、移植记录、接口约定。
- `tests/`：PC mock 测试。

其中 `drivers_user/board_gpio.c`、`board_i2c.c` 是移植到真实硬件时最需要修改的文件。

## 13. 哪些文件未来可能被 Simulink 生成代码替换

未来可能被 Simulink / Embedded Coder 生成或覆盖的文件应限定在 `generated_control/` 下，例如：

- `model.c`
- `model.h`
- `model_private.h`
- `model_data.c`
- `rtwtypes.h`

当前 `app/`、`hmi/`、`drivers_user/`、`control_if/` 不应被生成代码替换。生成代码应通过 `control_if` 交互，而不是直接访问 HMI 内部变量。

## 14. 当前工程的潜在风险点

- 缺少实际 `main.c`、CCS 工程文件、链接脚本和目标构建脚本，当前更像可集成模板。
- GPIO/I2C 均为占位实现，尚不能直接驱动真实 C2000 硬件。
- OLED 目前只是文本缓存和空 I2C 写占位，未实现 SSD1306 初始化、显存转换和真实刷新。
- `control_interface.c` 使用 `volatile` 结构体共享多字段数据，但没有临界区、双缓冲或版本号；ISR 与后台并发访问时可能读到不一致快照。
- 矩阵键盘扫描配置为 4x4，但当前只映射 7 个功能键，未处理组合键、鬼键或二极管隔离策略。
- `BoardGPIO_ReadMatrixColumn()` 中矩阵列跨度使用固定 `4u`，与 `KEY_MATRIX_COLS` 没有统一来源。
- 按键事件只有一个 `pending_event`，短时间多个事件可能被丢弃。
- 嵌入式目标上使用 `snprintf` 和浮点格式化可能增加代码体积和运行时间，需要评估 C2000 编译库支持与实时预算。
- 文档中部分中文存在编码显示异常，可能影响后续维护阅读。
- 当前测试覆盖按键和 HMI 基本流程，但没有覆盖边界值、故障页面、多键输入和并发访问。

## 15. 当前工程的编译前置条件

### PC mock 测试

需要 C 编译器、标准 C 库、头文件 include path，以及 `UNIT_TEST` 宏。`hmi_mock_test.c` 还需要数学库支持：

```sh
gcc -DUNIT_TEST -Iapp -Ihmi -Idrivers_user -Icontrol_if tests/key_scan_test.c drivers_user/key.c drivers_user/board_gpio.c -o key_scan_test
gcc -DUNIT_TEST -Iapp -Ihmi -Idrivers_user -Icontrol_if tests/hmi_mock_test.c hmi/*.c drivers_user/*.c control_if/control_interface.c -lm -o hmi_mock_test
```

### C2000 目标工程

需要 TI C2000 编译器或兼容工具链、Code Composer Studio 或等效工程、C2000Ware、启动文件、链接脚本、中断向量、时钟和外设初始化代码。还需要补全 `TODO(F280015x)` 标记的 GPIO/I2C 实现，并将 Simulink 生成的控制算法源码放入 `generated_control/` 或外部工程中统一编译。
