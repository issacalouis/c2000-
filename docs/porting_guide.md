# C2000 移植说明

## 1. 选择芯片和工程方式

本模板不绑定具体 C2000 型号。可用于 F28002x、F28003x、F28004x、F2837x、F2838x 等系列，但需要根据实际芯片和原理图补齐底层驱动。

推荐方式是由 CCS 工程统一编译：

```text
CCS 工程
  Simulink 生成控制代码
  本 HMI 模板
  C2000Ware driverlib/device 支持文件
```

## 2. GPIO 按键移植

修改 `drivers_user/board_gpio.c`：

- `BoardGPIO_Init()`：配置按键 GPIO 输入、上拉、输入同步或采样资格；
- `BoardGPIO_ReadKeyUp()` 等函数：返回 `BOARD_KEY_PRESSED` 或 `BOARD_KEY_RELEASED`；
- 如使用矩阵键盘，实现 `BoardGPIO_SetMatrixRow()` 和 `BoardGPIO_ReadMatrixColumn()`。

按键模块默认认为 board 层返回 1 表示按下。若硬件为低电平按下，请在 board 层完成极性转换。

## 3. OLED / LCD 移植

默认显示接口是 SSD1306 OLED 128x64 I2C。需要补齐：

- `drivers_user/board_i2c.c`：I2C 初始化和写数据；
- `drivers_user/oled.c`：SSD1306 初始化命令、显存页面刷新、字符点阵绘制。

如换成 SPI OLED 或 TFT，可保留 `hmi_display.c` 页面逻辑，替换 `oled.c` 内部实现或新增通用 display 层。

## 4. ControlIF 临界区

`control_if/control_interface.c` 中的 setpoint 和 feedback 可能被后台任务与控制 ISR 同时访问。真实工程中建议接入临界区宏，例如：

```c
#define CONTROLIF_CRITICAL_ENTER() DINT
#define CONTROLIF_CRITICAL_EXIT()  EINT
```

具体写法取决于 TI 编译器、driverlib 和项目中断管理方式。若控制 ISR 读取 setpoint，而后台只在 10 ms 更新，仍应评估 float/struct 拷贝在目标 C2000 上的原子性。

## 5. 参数限幅

默认范围在 `app/app_config.h`：

```c
APP_VREF_MIN / APP_VREF_MAX
APP_IREF_MIN / APP_IREF_MAX
APP_MODE_MIN / APP_MODE_MAX
```

电源真实参数上线前，请按硬件耐压、电流采样范围、散热条件和保护策略重新设置。

## 6. 安全要求

HMI 的 `enable_cmd` 只是用户请求，不应直接等于 PWM 使能。控制状态机还需要检查：

- 当前无故障；
- ADC 和采样链路正常；
- 母线电压在允许范围；
- 软启动状态允许；
- 硬件保护未触发。

严重故障应由控制逻辑或硬件保护立即关闭 PWM，HMI 只能显示故障和发起复位请求，不能绕过保护。
