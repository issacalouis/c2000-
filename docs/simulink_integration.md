# Simulink 集成说明

## 推荐方式：外部 CCS 工程

推荐让 Simulink / Embedded Coder 只生成控制算法源码，CCS 工程统一编译 C2000Ware、生成代码、`app/`、`hmi/`、`drivers_user/` 和 `control_if/`。

HMI 与控制算法之间只通过 `control_if` 交换数据；当前 `control_if` 内部已经使用双缓冲和序列号校验来保证多字段快照一致性：

```c
ControlIF_GetSetpoint(&setpoint);
ControlIF_SetFeedback(&feedback);
```

## 应用初始化

目标工程主程序调用：

```c
APP_Init();
```

`APP_Init()` 内部会调用 `Board_Init()`，完成 F2800157 板级初始化、Key 初始化和 OLED 初始化。不要在 Simulink 生成代码中重复调用 `HMI_Init()`、`Key_Init()` 或 `OLED_Init()`。

## Include 目录

```text
../app
../hmi
../drivers_user
../control_if
../generated_control
```

## 手写源码列表

```text
../app/app_main.c
../hmi/hmi.c
../hmi/hmi_menu.c
../hmi/hmi_param.c
../hmi/hmi_display.c
../drivers_user/board.c
../drivers_user/board_gpio.c
../drivers_user/board_i2c.c
../drivers_user/key.c
../drivers_user/oled.c
../drivers_user/oled_font.c
../control_if/control_interface.c
```

`drivers_user/board_pinmap.h` 是头文件配置依赖，也必须加入 include path 可见范围。

## 控制 ISR 建议

控制 ISR 中只做 ADC、控制 step、PWM、保护和反馈写入。OLED 刷新、按键扫描、菜单处理和字符串格式化都留在 `APP_BackgroundLoop()` 调度的低速 HMI 任务中。
