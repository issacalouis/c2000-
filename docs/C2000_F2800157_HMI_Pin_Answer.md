# C2000 F2800157 HMI 外设推荐回答

适用对象：`LAUNCHXL-F2800157` / `TMS320F2800157`，目标功能为：

```text
C2000 电源控制 + SSD1306 I2C OLED + 4x4 矩阵键盘
```

本文件给出直接可用的推荐答案，方便后续写入 `app_config.h`、`board_gpio.c`、`board_i2c.c`、`key.c` 和 `oled.c`。

---

## 1. 推荐配置总表

| 问题 | 推荐答案 |
|---|---|
| OLED 使用哪个 I2C 模块 | `I2CA` |
| OLED SDA 接到哪个 GPIO | `GPIO42`，配置为 `I2CA_SDA` |
| OLED SCL 接到哪个 GPIO | `GPIO43`，配置为 `I2CA_SCL` |
| OLED 地址 | `0x3C`，按 7-bit I2C 地址处理 |
| I2C 速率 | `400 kHz` |
| OLED 类型 | `SSD1306` |
| OLED 分辨率 | `128 x 64` |
| 矩阵键盘类型 | `4 行 x 4 列` |
| 矩阵键盘扫描方式 | 行扫描，列读取 |
| 行线/列线有效电平 | 低有效 |
| 上拉/下拉 | 列线使用上拉；I2C 的 SDA/SCL 必须上拉到 3.3V |
| 底层驱动方式 | 推荐使用 `C2000Ware DriverLib` |
| 工程芯片型号 | `TMS320F2800157` |
| 开发板 | `LAUNCHXL-F2800157` |
| 系统时钟 | `120 MHz SYSCLK` |
| 外设初始化方式 | `Device_init()` + `Device_initGPIO()` + 用户 `Board_Init()` |

---

## 2. OLED I2C 推荐方案

### 2.1 推荐结论

```text
OLED VCC  → 3.3V
OLED GND  → GND
OLED SDA  → GPIO42 / I2CA_SDA
OLED SCL  → GPIO43 / I2CA_SCL
```

推荐使用：

```c
#define BOARD_OLED_I2C_BASE        I2CA_BASE
#define BOARD_OLED_SDA_GPIO        42U
#define BOARD_OLED_SCL_GPIO        43U
#define BOARD_OLED_ADDR_7BIT       0x3CU
#define BOARD_OLED_I2C_FREQ_HZ     400000UL
#define BOARD_OLED_WIDTH           128U
#define BOARD_OLED_HEIGHT          64U
```

---

### 2.2 为什么选择 I2CA

选择 `I2CA` 的原因：

1. `I2CA` 是 C2000 上常用的主 I2C 外设，适合接 OLED 这类低速人机交互设备。
2. `GPIO42` / `GPIO43` 可作为一组 I2CA 引脚使用，适合接在 LaunchPad 的外部排针上。
3. 相比把 I2C 放在 `GPIO18` / `GPIO19`，`GPIO42` / `GPIO43` 更适合作为普通外设扩展脚，不容易与晶振相关复用功能混淆。
4. OLED 属于低速 HMI 外设，不应该占用 ePWM、ADC、BOOT、调试串口等关键资源。

---

### 2.3 为什么地址选 0x3C

SSD1306 I2C OLED 模块最常见的 7-bit 地址是：

```text
0x3C
```

注意这里是 **7-bit 地址**，不是左移后的 8-bit 地址。

常见关系如下：

```text
7-bit 地址        = 0x3C
写操作地址字节    = 0x78 = 0x3C << 1
读操作地址字节    = 0x79 = (0x3C << 1) | 1
```

在 C2000Ware DriverLib 里，通常给 `I2C_setTargetAddress()` 传入的是 7-bit 地址，所以代码中应写：

```c
I2C_setTargetAddress(I2CA_BASE, 0x3C);
```

如果 OLED 不响应，再检查模块背面的地址焊盘。有些模块可切换为 `0x3D`。

---

### 2.4 为什么 I2C 速率直接用 400 kHz

本工程直接按：

```text
I2C = 400 kHz
```

原因：

1. SSD1306 128x64 全屏显存为：

   ```text
   128 × 64 / 8 = 1024 byte
   ```

2. 如果用 100 kHz，全屏刷新时间较长，会降低 HMI 响应速度。
3. OLED 显示属于低速任务，但 100 ms 周期刷新时，400 kHz 更合适。
4. 400 kHz 仍属于常见 I2C Fast-mode 范围，适合短线连接的 OLED。

工程建议：

```c
#define BOARD_OLED_I2C_FREQ_HZ     400000UL
```

硬件注意：

```text
SDA / SCL 必须上拉到 3.3V
建议上拉电阻：4.7 kΩ
线尽量短，不要飞线过长
```

---

## 3. OLED 分辨率推荐

推荐默认屏幕：

```text
SSD1306 OLED
128 x 64
I2C 接口
```

原因：

1. 128x64 是 SSD1306 最常见规格，资料和驱动最多。
2. 足够显示电源控制所需信息：`Vin`、`Vout`、`Iout`、`Duty`、`State`、`Fault`。
3. 与 HMI 主界面和参数设置界面匹配。

推荐宏：

```c
#define OLED_WIDTH     128U
#define OLED_HEIGHT    64U
#define OLED_PAGE_NUM  8U      /* 64 / 8 */
#define OLED_BUF_SIZE  1024U   /* 128 * 64 / 8 */
```

如果后续换成 128x32，需要改：

```c
#define OLED_HEIGHT    32U
#define OLED_PAGE_NUM  4U
#define OLED_BUF_SIZE  512U
```

并同步修改 SSD1306 初始化命令中的 multiplex ratio、COM pins 等参数。

---

## 4. 4x4 矩阵键盘 GPIO 推荐方案

### 4.1 推荐引脚分配

#### 行线 ROW：输出扫描线

| 矩阵键盘行 | 推荐 GPIO | 方向 | 说明 |
|---|---:|---|---|
| ROW0 | `GPIO25` | 输出 / 扫描时输出低 | 普通 GPIO，避开 PWM/ADC/I2C |
| ROW1 | `GPIO26` | 输出 / 扫描时输出低 | 普通 GPIO，避开 PWM/ADC/I2C |
| ROW2 | `GPIO27` | 输出 / 扫描时输出低 | 普通 GPIO，避开 PWM/ADC/I2C |
| ROW3 | `GPIO44` | 输出 / 扫描时输出低 | 普通 GPIO，适合作为键盘行线 |

#### 列线 COL：输入读取线

| 矩阵键盘列 | 推荐 GPIO | 方向 | 说明 |
|---|---:|---|---|
| COL0 | `GPIO45` | 输入，上拉 | 按下时被行线拉低 |
| COL1 | `GPIO46` | 输入，上拉 | 按下时被行线拉低 |
| COL2 | `GPIO48` | 输入，上拉 | 按下时被行线拉低 |
| COL3 | `GPIO33` | 输入，上拉 | 按下时被行线拉低 |

推荐宏：

```c
#define KEY_ROW0_GPIO    25U
#define KEY_ROW1_GPIO    26U
#define KEY_ROW2_GPIO    27U
#define KEY_ROW3_GPIO    44U

#define KEY_COL0_GPIO    45U
#define KEY_COL1_GPIO    46U
#define KEY_COL2_GPIO    48U
#define KEY_COL3_GPIO    33U
```

---

### 4.2 为什么这样分配键盘 GPIO

选择这些 GPIO 的原则：

1. 避开 ePWM 输出引脚，防止影响电源主控制。
2. 避开 ADC 输入引脚，防止影响电压、电流采样。
3. 避开 BOOT 引脚，防止影响启动模式。
4. 避开 XRSn、JTAG、调试串口等关键调试资源。
5. 避开 OLED 的 `GPIO42` / `GPIO43`。
6. 键盘只需要普通 GPIO，不应占用高价值外设复用脚。

---

## 5. 行线 / 列线有效电平

推荐采用：

```text
低有效
```

也就是：

```text
未按下：列线 = 1
按下：列线 = 0
```

扫描方法：

```text
1. 列线 COL0~COL3 配置为输入上拉
2. 每次只选中一行 ROWx
3. 被选中的 ROWx 输出低电平
4. 读取 COL0~COL3
5. 如果某列读到 0，则说明该行该列按键按下
```

典型扫描过程：

```text
ROW0 = 0，读取 COL0~COL3
ROW1 = 0，读取 COL0~COL3
ROW2 = 0，读取 COL0~COL3
ROW3 = 0，读取 COL0~COL3
```

工程上更稳妥的做法：

```text
当前扫描行：配置为输出低
非当前扫描行：配置为输入高阻，或者输出高
```

如果只允许单键按下，非当前行输出高也可以工作。若希望降低多键同时按下导致行线互相短接的风险，推荐非当前行配置为输入高阻。

---

## 6. 是否需要上拉 / 下拉

### 6.1 矩阵键盘

推荐：

```text
列线 COL0~COL3：上拉
行线 ROW0~ROW3：不需要上拉/下拉，作为扫描输出
```

可以先使用 MCU 内部上拉：

```c
GPIO_setPadConfig(KEY_COL0_GPIO, GPIO_PIN_TYPE_PULLUP);
GPIO_setDirectionMode(KEY_COL0_GPIO, GPIO_DIR_MODE_IN);
```

如果键盘线较长，或者按键板离 LaunchPad 比较远，建议每个列线额外加：

```text
10 kΩ 上拉到 3.3V
```

不建议使用下拉，因为本方案采用的是：

```text
列线上拉 + 行线扫描拉低
```

---

### 6.2 OLED I2C

I2C 必须上拉：

```text
SDA → 4.7 kΩ → 3.3V
SCL → 4.7 kΩ → 3.3V
```

如果 OLED 模块板上已经带上拉电阻，可以不重复添加。但必须确认：

```text
上拉电压是 3.3V，不是 5V
```

C2000 GPIO 按 3.3V 逻辑使用，不要把 SDA/SCL 上拉到 5V。

---

## 7. 使用 DriverLib 还是直接寄存器

推荐使用：

```text
C2000Ware DriverLib
```

原因：

1. HMI 的重点是稳定、可移植、可复用，不是极限执行速度。
2. OLED 和键盘属于低速外设，不需要直接寄存器优化。
3. DriverLib 代码可读性更高，后续换 GPIO 或换 I2C 引脚更容易。
4. 与工程分层更匹配：

```text
hmi_menu.c
    ↓
key.c / oled.c
    ↓
board_gpio.c / board_i2c.c
    ↓
C2000Ware DriverLib
```

不推荐在菜单逻辑中直接操作寄存器，也不推荐在 `hmi_menu.c` 里直接写 `GPIO_readPin()`、`I2C_putData()`。这些应封装到 `drivers_user` 目录下。

---

## 8. 工程芯片型号、时钟频率和初始化方式

### 8.1 芯片型号

```text
开发板：LAUNCHXL-F2800157
芯片：TMS320F2800157
```

CCS / MATLAB 目标芯片选择：

```text
TMS320F2800157
```

---

### 8.2 系统时钟

推荐按：

```c
#define DEVICE_SYSCLK_FREQ    120000000UL
```

即：

```text
SYSCLK = 120 MHz
```

---

### 8.3 初始化顺序

推荐初始化顺序：

```c
void Board_Init(void)
{
    Device_init();
    Device_initGPIO();

    BoardGPIO_Init();
    BoardI2C_Init();

    Key_Init();
    OLED_Init();

    HMI_Init();
}
```

各步骤职责：

| 初始化函数 | 作用 |
|---|---|
| `Device_init()` | 初始化系统时钟、PLL、看门狗等基础系统资源 |
| `Device_initGPIO()` | 初始化 GPIO 基础状态，解锁 GPIO 复用配置 |
| `BoardGPIO_Init()` | 配置矩阵键盘行列 GPIO |
| `BoardI2C_Init()` | 配置 OLED 使用的 I2CA 和 GPIO42/GPIO43 |
| `Key_Init()` | 初始化按键扫描状态机 |
| `OLED_Init()` | 发送 SSD1306 初始化命令 |
| `HMI_Init()` | 初始化 HMI 参数、菜单、显示状态 |

---

## 9. 最终推荐宏定义

建议放入：

```text
app/app_config.h
```

或：

```text
drivers_user/board_pinmap.h
```

推荐内容：

```c
#ifndef BOARD_PINMAP_H
#define BOARD_PINMAP_H

#include <stdint.h>

/* ================= System ================= */

#define BOARD_MCU_NAME                 "TMS320F2800157"
#define BOARD_NAME                     "LAUNCHXL-F2800157"
#define BOARD_SYSCLK_FREQ_HZ           120000000UL

/* ================= OLED / I2C ================= */

#define BOARD_OLED_I2C_BASE            I2CA_BASE
#define BOARD_OLED_SDA_GPIO            42U
#define BOARD_OLED_SCL_GPIO            43U
#define BOARD_OLED_ADDR_7BIT           0x3CU
#define BOARD_OLED_I2C_FREQ_HZ         400000UL

#define BOARD_OLED_WIDTH               128U
#define BOARD_OLED_HEIGHT              64U
#define BOARD_OLED_PAGE_NUM            8U
#define BOARD_OLED_BUF_SIZE            1024U

/* ================= 4x4 Matrix Keypad ================= */

#define KEY_ROW_NUM                    4U
#define KEY_COL_NUM                    4U

#define KEY_ROW0_GPIO                  25U
#define KEY_ROW1_GPIO                  26U
#define KEY_ROW2_GPIO                  27U
#define KEY_ROW3_GPIO                  44U

#define KEY_COL0_GPIO                  45U
#define KEY_COL1_GPIO                  46U
#define KEY_COL2_GPIO                  48U
#define KEY_COL3_GPIO                  33U

#define KEY_ACTIVE_LEVEL               0U
#define KEY_INACTIVE_LEVEL             1U

#define KEY_SCAN_PERIOD_MS             10U
#define OLED_REFRESH_PERIOD_MS         100U

#endif
```

---

## 10. 结论

本工程的推荐连接方案为：

```text
OLED：I2CA，GPIO42/SDA，GPIO43/SCL，地址 0x3C，400 kHz，SSD1306 128x64
键盘：4x4 矩阵键盘，GPIO25/26/27/44 作行，GPIO45/46/48/33 作列，低有效，上拉输入
驱动：C2000Ware DriverLib
时钟：TMS320F2800157，SYSCLK 120 MHz
结构：OLED 刷新和按键扫描放在低速 HMI 任务，不进入电源控制 ISR
```

关键原则：

```text
电源控制 ISR 只做 ADC、控制算法、PWM、保护。
OLED 和矩阵键盘只放在后台低速任务中。
HMI 通过 ControlIF 与 Simulink 控制算法交换参数。
```
