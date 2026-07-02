# C2000 电源控制工程 Codex 开发指导文件

## 1. 项目目标

本项目目标是创建一个在 **TI C2000** 平台上实现一个电源控制系统的HMI文件：

1. 控制算法主体由 **MATLAB/Simulink / Embedded Coder** 生成。
2. 工程额外加入：
   - 按键 / 键盘输入模块；
   - 小屏幕显示模块；
   - 菜单与参数设置模块；
   - 控制算法与 HMI 之间的参数交互接口。
3. 最终形成一个可复用工程结构：  
   每次 Simulink 重新生成控制算法代码后，键盘与屏幕模块不需要重新手动移植，只需要重新编译即可。

核心原则：

键盘和屏幕应封装为独立 HMI 模块，通过固定接口与控制算法通信。

---

## 2. 目标硬件平台

目标芯片系列：

- TI C2000 系列 MCU；
- 具体型号F280015x；

 

外设需求：

| 功能 | 推荐外设 |
|---|---|
| PWM 输出 | ePWM |
| ADC 采样 | ADC |
| 快速保护 | CMPSS / Trip Zone |
| 按键输入 | GPIO |
| 矩阵键盘 | GPIO 行列扫描 |
| OLED 小屏 | I2C |


本指导文件优先按照以下硬件方案设计：

```text
C2000 + 矩阵键盘 + OLED 小屏
```

---

## 3. 总体软件架构

工程分为两部分：

```text
C2000_Power_Project
├── Simulink 生成控制算法部分
│   ├── ADC 采样
│   ├── 电压环 / 电流环
│   ├── PWM 更新
│   ├── 保护逻辑
│   └── 控制状态机
│
└── 手写 C 语言 HMI 部分
    ├── 按键扫描
    ├── 按键消抖
    ├── 菜单系统
    ├── OLED 显示
    ├── 参数设置
    └── 与控制算法的数据接口
```

推荐运行结构：

```text
高速中断：
ADC采样 → 控制算法 step → PWM更新 → 快速保护判断

低速任务：
按键扫描 → 菜单处理 → 屏幕刷新 → 参数更新
```

---

## 4. 重要工程约束

Codex 必须遵守以下约束。

### 4.1 不要直接修改 Simulink 生成文件

禁止手动修改以下类型文件：

```text
model.c
model.h
model_private.h
model_data.c
ert_main.c
rtwtypes.h
```

原因：

- 这些文件可能在下一次代码生成时被覆盖；
- 手动改动不可维护；
- 不利于后续模型迭代。

正确做法：

- 新建独立 C 文件；
- 通过外部接口与 Simulink 生成代码交互；
- 通过 Simulink Custom Code、C Caller、S-Function、或者 CCS 工程方式集成。

---

### 4.2 控制中断中禁止执行慢任务

控制中断中只允许执行：

```text
ADC读取
控制计算
PWM更新
必要的软件保护判断
中断标志清除
```

禁止在控制中断中执行：

```text
OLED刷新
按键长按判断
字符串格式化
浮点转字符串
菜单绘制
Flash参数保存
阻塞延时
printf
```

原因：

- 电源控制是强实时任务；
- 屏幕刷新和字符串处理耗时不可控；
- 中断执行时间过长会引起 PWM 抖动、控制环不稳定，甚至保护失效。

---

### 4.3 所有 HMI 任务必须非阻塞

禁止使用阻塞式延时：

```c
DELAY_US(10000);
delay_ms(20);
while(flag == 0);
```

推荐使用周期任务：

```text
1 ms 系统节拍
10 ms 按键扫描任务
20 ms 菜单任务
100 ms 屏幕刷新任务
```

---

### 4.4 HMI 与控制算法通过参数接口通信

不要让 HMI 直接修改控制算法内部变量。

推荐使用接口函数：

```c
float HMI_Get_Vref(void);
float HMI_Get_Iref(void);
uint16_t HMI_Get_EnableCmd(void);

void HMI_Set_Vout(float value);
void HMI_Set_Iout(float value);
void HMI_Set_FaultCode(uint16_t fault);
void HMI_Set_RunState(uint16_t state);
```

控制算法只读取设定值，只写入显示反馈值。

---

## 5. 推荐工程目录结构

请生成以下目录结构：

```text
C2000_Power_HMI_Project
├── README.md
├── docs
│   ├── architecture.md
│   ├── porting_guide.md
│   └── simulink_integration.md
│
├── app
│   ├── app_main.c
│   ├── app_main.h
│   ├── app_config.h
│   └── app_types.h
│
├── hmi
│   ├── hmi.c
│   ├── hmi.h
│   ├── hmi_menu.c
│   ├── hmi_menu.h
│   ├── hmi_param.c
│   ├── hmi_param.h
│   ├── hmi_display.c
│   └── hmi_display.h
│
├── drivers_user
│   ├── key.c
│   ├── key.h
│   ├── oled.c
│   ├── oled.h
│   ├── oled_font.c
│   ├── oled_font.h
│   ├── board_gpio.c
│   ├── board_gpio.h
│   ├── board_i2c.c
│   ├── board_i2c.h
│
├── control_if
│   ├── control_interface.c
│   └── control_interface.h
│
├── generated_control
│   └── README_GENERATED_CODE.md
│
└── tests
    ├── hmi_mock_test.c
    └── key_scan_test.c
```

说明：

- `generated_control` 用来放 Simulink 生成代码；
- `hmi` 放人机交互逻辑；
- `drivers_user` 放屏幕、键盘、GPIO、I2C 等底层驱动；
- `control_if` 放控制算法与 HMI 的接口；
- `app` 放主程序调度；
- `docs` 放说明文档；
- `tests` 放可在 PC 上运行或半模拟的测试代码。

---

## 6. HMI 模块设计要求

### 6.1 HMI 顶层接口

请实现 `hmi.h`：

```c
#ifndef HMI_H
#define HMI_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void HMI_Init(void);
void HMI_Task_1ms(void);
void HMI_Task_10ms(void);
void HMI_Task_100ms(void);

float HMI_Get_Vref(void);
float HMI_Get_Iref(void);
uint16_t HMI_Get_EnableCmd(void);
uint16_t HMI_Get_ModeCmd(void);

void HMI_Set_Vin(float vin);
void HMI_Set_Vout(float vout);
void HMI_Set_Iout(float iout);
void HMI_Set_Duty(float duty);
void HMI_Set_FaultCode(uint16_t fault_code);
void HMI_Set_RunState(uint16_t run_state);

#ifdef __cplusplus
}
#endif

#endif
```

### 6.2 HMI 内部参数

HMI 至少应维护以下参数：

```c
typedef struct
{
    float vref;
    float iref;
    float vin;
    float vout;
    float iout;
    float duty;

    uint16_t enable_cmd;
    uint16_t mode_cmd;
    uint16_t run_state;
    uint16_t fault_code;
} HMI_Data_t;
```

参数默认值：

```text
vref = 12.0 V
iref = 2.0 A
enable_cmd = 0
mode_cmd = 0
fault_code = 0
```

---

## 7. 按键模块设计要求

### 7.1 支持按键形式

矩阵键盘。


---


### 7.3 按键事件类型

请定义：

```c
typedef enum
{
    KEY_EVENT_NONE = 0,
    KEY_EVENT_UP,
    KEY_EVENT_DOWN,
    KEY_EVENT_LEFT,
    KEY_EVENT_RIGHT,
    KEY_EVENT_OK,
    KEY_EVENT_BACK,
    KEY_EVENT_RUN,
    KEY_EVENT_LONG_UP,
    KEY_EVENT_LONG_DOWN,
    KEY_EVENT_LONG_OK,
    KEY_EVENT_LONG_RUN
} KeyEvent_t;
```

---

### 7.4 按键扫描要求

- 每 10 ms 调用一次扫描函数；
- 支持软件消抖；
- 支持短按；
- 支持长按；
- 不允许阻塞式延时；
- 不允许在按键模块中直接操作控制算法变量；
- 按键模块只产生事件，菜单模块负责解释事件含义。

推荐接口：

```c
void Key_Init(void);
void Key_Task_10ms(void);
KeyEvent_t Key_GetEvent(void);
```

---

## 8. 显示模块设计要求

### 8.1 支持的显示设备

优先实现抽象接口，具体底层可适配：

1. SSD1306 OLED，I2C；
5. 串口屏，SCI / UART。

本项目默认先实现：

```text
SSD1306 OLED
128 x 64
I2C接口
```

---

### 8.2 显示接口

请实现 `oled.h` 或通用 `display.h`：

```c
#ifndef OLED_H
#define OLED_H

#include <stdint.h>

void OLED_Init(void);
void OLED_Clear(void);
void OLED_Update(void);

void OLED_ShowString(uint8_t x, uint8_t y, const char *str);
void OLED_ShowInt(uint8_t x, uint8_t y, int32_t value);
void OLED_ShowFloat(uint8_t x, uint8_t y, float value, uint8_t decimals);

#endif
```

---

### 8.3 显示刷新要求

- 屏幕刷新周期默认 100 ms；
- 不允许在高速控制中断中刷新屏幕；
- 字符串格式化应尽量在低速任务中完成；
- 如果使用 I2C OLED，建议局部刷新或限制每次刷新内容；
- 代码中需要预留 `OLED_Update()` 函数，即使底层实现暂时为空。

---

## 9. 菜单系统设计要求

菜单系统应至少包含以下页面：

### 9.1 主界面

显示：

```text
Vin
Vout
Iout
Duty
State
Fault
```

示例：

```text
Vin : 24.0V
Vout: 12.0V
Iout: 1.5A
Duty: 42.3%
RUN
```

---

### 9.2 参数设置界面

可设置：

```text
Vref
Iref
Enable
Mode
```

示例：

```text
Set Vref
12.00 V
UP/DOWN Adjust
OK Save
```

---

### 9.3 故障界面

显示故障码：

```text
No Fault
OVP
OCP
OTP
UVLO
ADC Fault
PWM Fault
```

故障码定义：

```c
#define FAULT_NONE      0x0000u
#define FAULT_OVP       0x0001u
#define FAULT_OCP       0x0002u
#define FAULT_OTP       0x0004u
#define FAULT_UVLO      0x0008u
#define FAULT_ADC       0x0010u
#define FAULT_PWM       0x0020u
```

---

## 10. 控制算法接口设计

### 10.1 控制算法不直接依赖 HMI 内部实现

请实现 `control_interface.h`：

```c
#ifndef CONTROL_INTERFACE_H
#define CONTROL_INTERFACE_H

#include <stdint.h>

typedef struct
{
    float vref;
    float iref;
    uint16_t enable_cmd;
    uint16_t mode_cmd;
} Control_Setpoint_t;

typedef struct
{
    float vin;
    float vout;
    float iout;
    float duty;
    uint16_t run_state;
    uint16_t fault_code;
} Control_Feedback_t;

void ControlIF_Init(void);

void ControlIF_SetSetpoint(const Control_Setpoint_t *setpoint);
void ControlIF_GetSetpoint(Control_Setpoint_t *setpoint);

void ControlIF_SetFeedback(const Control_Feedback_t *feedback);
void ControlIF_GetFeedback(Control_Feedback_t *feedback);

#endif
```

---

### 10.2 数据流方向

推荐数据流：

```text
HMI → ControlIF → Simulink控制算法
Simulink控制算法 → ControlIF → HMI
```

不要做成：

```text
HMI 直接修改 Simulink 模型内部变量
```

---

## 11. Simulink 集成方式说明

Codex 需要生成文档 `docs/simulink_integration.md`，说明以下内容：

### 11.1 推荐集成方式一：C Caller Block

在 Simulink 中使用 C Caller 调用以下函数：

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

---

### 11.2 推荐集成方式二：Custom Code 配置

在 Simulink Model Settings 中添加：

Include headers：

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
../control_if/control_interface.c
```

---

### 11.3 推荐集成方式三：外部 CCS 工程

也可以让 Simulink 只生成控制算法源码，然后由 CCS 工程统一编译：

```text
CCS工程
├── Simulink生成代码
├── HMI模块
├── C2000Ware驱动
└── 用户app调度
```

这种方式更接近实际工程。

---

## 12. 主循环调度模板

请生成 `app/app_main.c`，包含类似结构：

```c
#include "app_main.h"
#include "hmi.h"
#include "control_interface.h"

static volatile uint16_t g_task_1ms_flag = 0;
static volatile uint16_t g_task_10ms_flag = 0;
static volatile uint16_t g_task_100ms_flag = 0;

void APP_Init(void)
{
    ControlIF_Init();
    HMI_Init();
}

void APP_TaskScheduler_1ms_ISR(void)
{
    static uint16_t cnt_10ms = 0;
    static uint16_t cnt_100ms = 0;

    g_task_1ms_flag = 1;

    cnt_10ms++;
    cnt_100ms++;

    if (cnt_10ms >= 10)
    {
        cnt_10ms = 0;
        g_task_10ms_flag = 1;
    }

    if (cnt_100ms >= 100)
    {
        cnt_100ms = 0;
        g_task_100ms_flag = 1;
    }
}

void APP_BackgroundLoop(void)
{
    if (g_task_1ms_flag)
    {
        g_task_1ms_flag = 0;
        HMI_Task_1ms();
    }

    if (g_task_10ms_flag)
    {
        g_task_10ms_flag = 0;
        HMI_Task_10ms();
    }

    if (g_task_100ms_flag)
    {
        g_task_100ms_flag = 0;
        HMI_Task_100ms();
    }
}
```

---

## 13. 控制中断模板

请生成一个说明文件或模板，说明控制 ISR 应类似：

```c
__interrupt void ControlISR(void)
{
    /*
     * 1. Read ADC values
     * 2. Call Simulink generated control step function
     * 3. Update PWM compare registers
     * 4. Update ControlIF feedback
     * 5. Clear interrupt flags
     */

    /* 禁止在这里刷新屏幕 */
    /* 禁止在这里扫描键盘 */
}
```

---

## 14. 编码规范

### 14.1 C 语言标准

- 优先使用 C99；
- 所有头文件必须有 include guard；
- 不使用动态内存分配；
- 不使用 malloc/free；
- 不使用递归；
- 不在控制路径中使用 printf；
- 少用全局变量，必须使用时加模块前缀；
- ISR 共享变量必须使用 `volatile`；
- 对跨 ISR / 主循环共享的多字节变量要考虑临界区保护。

---

### 14.2 命名规范

模块前缀：

| 模块 | 前缀 |
|---|---|
| HMI | `HMI_` |
| Key | `Key_` |
| OLED | `OLED_` |
| Control Interface | `ControlIF_` |
| App | `APP_` |
| Board GPIO | `BoardGPIO_` |
| Board I2C | `BoardI2C_` |

---

### 14.3 文件规范

每个 `.c` 文件顶部写明：

```c
/*
 * File:
 * Description:
 * Notes:
 */
```

每个函数尽量写简短注释，说明：

- 功能；
- 调用周期；
- 是否允许在 ISR 中调用；
- 是否阻塞。

---

## 15. 可移植性要求

底层硬件操作必须集中在 `drivers_user` 中。

例如：

```c
uint16_t BoardGPIO_ReadKeyUp(void);
void BoardI2C_Write(uint8_t dev_addr, const uint8_t *data, uint16_t len);
```

不要在 HMI 菜单逻辑中直接写：

```c
GPIO_readPin(12);
I2C_putData(...);
```

原因：

- 不同 C2000 型号 GPIO、I2C 配置可能不同；
- 菜单逻辑应独立于硬件型号；
- 后续换芯片时只需要改 board 层。

---

## 16. 安全与电源控制要求

电源控制系统必须优先保证安全。

### 16.1 输出使能逻辑

HMI 的 Enable 命令不能直接等于 PWM 使能。

推荐逻辑：

```text
用户按 RUN
↓
HMI_Set_EnableCmd(1)
↓
控制状态机判断：
    无故障
    ADC正常
    母线电压正常
    软启动允许
↓
PWM真正使能
```

---

### 16.2 故障处理

发生严重故障时：

```text
立即关闭 PWM
记录故障码
屏幕显示故障
等待用户复位
```

HMI 只能请求复位，不能强制绕过硬件保护。

---

### 16.3 参数限幅

所有用户可设置参数必须有限幅。

示例：

```c
#define VREF_MIN   0.0f
#define VREF_MAX   48.0f
#define IREF_MIN   0.0f
#define IREF_MAX   10.0f
```

设置时必须限制：

```c
if (vref > VREF_MAX) vref = VREF_MAX;
if (vref < VREF_MIN) vref = VREF_MIN;
```

---

## 17. 默认参数

请在 `app/app_config.h` 中定义：

```c
#define APP_VREF_DEFAULT       12.0f
#define APP_IREF_DEFAULT       2.0f

#define APP_VREF_MIN           0.0f
#define APP_VREF_MAX           48.0f

#define APP_IREF_MIN           0.0f
#define APP_IREF_MAX           10.0f

#define APP_HMI_TASK_1MS       1u
#define APP_KEY_TASK_10MS      10u
#define APP_DISPLAY_TASK_100MS 100u
```

---

## 18. 需要 Codex 生成的内容

请 Codex 生成以下内容：

```text
1. 完整目录结构；
2. 每个模块的 .c / .h 文件；
3. HMI 顶层接口；
4. 按键扫描模块；
5. OLED 抽象显示模块；
6. 菜单系统；
7. ControlIF 数据接口；
8. APP 调度模板；
9. Simulink 集成说明文档；
10. 移植说明文档；
11. README.md；
12. 简单 mock 测试代码。
```

---

## 19. 不需要 Codex 完成的内容

Codex 不需要直接完成：

```text
1. Simulink 模型搭建；
2. C2000 ePWM 真实寄存器配置；
3. ADC 真实采样通道配置；
4. CMPSS / Trip Zone 真实硬件保护配置；
5. 具体芯片引脚分配；
6. CCS 工程文件完全适配；
7. MATLAB 自动代码生成配置。
```

这些内容需要根据实际 C2000 型号、开发板、Simulink 模型和硬件原理图再补充。

但 Codex 应在代码中预留接口与注释，方便后续移植。

---

## 20. 生成代码的优先级

Codex 应按以下优先级生成：

### 第一优先级

```text
架构清晰
模块边界明确
不会修改 Simulink 生成文件
可复用
可移植
非阻塞
```

### 第二优先级

```text
功能完整
菜单可用
按键事件清晰
显示接口清晰
```

### 第三优先级

```text
具体屏幕驱动完整
具体芯片底层寄存器完整
```

若具体芯片型号未知，底层驱动可以先使用弱函数、空实现或 TODO 注释。

---

## 21. 期望输出效果

最终工程应支持以下数据流：

```text
用户按键修改 Vref / Iref
↓
HMI 参数更新
↓
Simulink 控制算法读取设定值
↓
控制 PWM 输出
↓
控制算法写入 Vin / Vout / Iout / Duty / Fault
↓
HMI 屏幕刷新显示
```

---

## 22. 验收标准

生成结果满足以下条件即可视为初版合格：

- [ ] 工程目录完整；
- [ ] HMI 模块可独立编译；
- [ ] 按键模块无阻塞延时；
- [ ] 显示模块不进入控制 ISR；
- [ ] 有统一参数接口；
- [ ] 有菜单状态机；
- [ ] 有 Simulink 集成说明；
- [ ] 有移植说明；
- [ ] 所有用户参数有上下限；
- [ ] 故障码有统一定义；
- [ ] 不修改 Simulink 生成文件；
- [ ] 代码中明确标注哪些函数可在 ISR 中调用，哪些不可在 ISR 中调用。

---

## 23. 给 Codex 的最终执行指令

请根据本文件生成一个 C2000 电源控制 HMI 工程模板。

重点要求：

1. 不要修改 Simulink 自动生成文件；
2. 将键盘和屏幕封装成可复用 HMI 模块；
3. HMI 模块通过固定 C 接口与控制算法交换数据；
4. 控制 ISR 中禁止执行屏幕刷新、按键扫描和阻塞延时；
5. 所有低速任务必须通过周期调度执行；
6. 屏幕和键盘底层驱动要与菜单逻辑解耦；
7. 生成中文 README 和移植说明；
8. 对暂时无法确定的 C2000 硬件细节，用 TODO 注释标出；
9. 代码优先保证结构正确和可维护性；
10. 后续用户会根据具体芯片型号和屏幕型号补充底层驱动。
