# F2800157 移植说明

## 1. 引脚配置

当前工程已按 `LAUNCHXL-F2800157` 落地默认引脚，集中定义在 `drivers_user/board_pinmap.h`。

- OLED I2C：`I2CA`
- OLED SDA：`GPIO42`
- OLED SCL：`GPIO43`
- OLED 地址：`0x3C`，7-bit 地址
- I2C 速率：`400 kHz`
- 键盘行线：`GPIO25 / GPIO26 / GPIO27 / GPIO44`
- 键盘列线：`GPIO45 / GPIO46 / GPIO48 / GPIO33`

如原理图不同，优先修改 `board_pinmap.h`，再检查 `board_gpio.c` 和 `board_i2c.c` 是否需要同步调整。

## 2. 矩阵键盘

键盘为 4x4 行扫描、列读取方案。列线使用上拉，低电平表示按下。`BoardGPIO_ReadMatrixColumn()` 返回逻辑按键状态：

```text
GPIO 读到 0 -> 返回 1，表示按下
GPIO 读到 1 -> 返回 0，表示未按下
```

当前功能键映射为：

```text
ROW0 COL0 = UP
ROW0 COL1 = DOWN
ROW0 COL2 = LEFT
ROW0 COL3 = RIGHT
ROW1 COL3 = OK
ROW2 COL3 = BACK
ROW3 COL3 = RUN
```

## 3. I2C OLED

OLED 驱动位于 `drivers_user/oled.c`，底层 I2C 传输位于 `drivers_user/board_i2c.c`。当前实现包含 SSD1306 初始化、文本缓存、帧缓冲渲染和整屏刷新。

`BoardI2C_Write()` 是阻塞轮询式传输，适合 100 ms HMI 显示任务，不应放入控制 ISR。

## 4. 初始化顺序

目标工程建议只从应用入口调用：

```c
APP_Init();
```

`APP_Init()` 内部会调用 `Board_Init()`，再初始化 `ControlIF` 和 HMI。不要在外部重复调用 `BoardGPIO_Init()`、`BoardI2C_Init()`、`Key_Init()` 或 `OLED_Init()`。

## 5. 禁止事项

不要在控制 ISR 中调用 `HMI_Task_10ms()`、`HMI_Task_100ms()`、`OLED_Update()`、`Key_Task_10ms()` 或任何阻塞延时。
