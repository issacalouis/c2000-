# F280015x 移植说明

## 1. 引脚确认

根据原理图确认独立按键或矩阵键盘的 GPIO 编号、上下拉方式和有效电平，然后修改 `drivers_user/board_gpio.c` 中的 `TODO(F280015x)`。

## 2. I2C OLED

默认 OLED 地址为 `0x3C`，接口在 `drivers_user/oled.c` 和 `drivers_user/board_i2c.c`。真实项目中需要补充 SSD1306 初始化命令、显存格式转换和 I2C 发送实现。

## 3. SPI 预留

`board_spi.c` 已预留 ST7735 / ILI9341 / SPI OLED 的传输接口。如果不用 SPI 显示，可保持空实现。

## 4. 临界区

`ControlIF_SetFeedback()` 可能在控制 ISR 中调用，HMI 后台会读取反馈。若目标工程要求多字段严格一致，应在 `control_interface.c` 中加入 C2000 临界区保护或双缓冲快照。

## 5. 参数安全

`app/app_config.h` 定义 Vref/Iref 默认值和上下限。后续增加新参数时，应先定义限幅，再在 `hmi_param.c` 中集中处理，避免菜单层直接写未限幅值。

## 6. 禁止事项

不要在控制 ISR 中调用 `HMI_Task_10ms()`、`HMI_Task_100ms()`、`OLED_Update()`、`Key_Task_10ms()` 或任何阻塞延时。