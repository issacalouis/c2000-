# 软件架构说明

## 分层

```text
app
  调度 HMI 低速任务，不放具体硬件逻辑

hmi
  菜单、参数、显示页面和 HMI 顶层接口

control_if
  控制算法和 HMI 的固定数据交换层

drivers_user
  GPIO、I2C、SPI、OLED、按键扫描等硬件相关或半硬件相关代码

generated_control
  Simulink / Embedded Coder 输出代码
```

## 任务模型

控制 ISR 属于高速实时路径，不允许执行以下操作：

- OLED / LCD 刷新；
- 按键扫描和长按判断；
- 菜单绘制；
- 字符串格式化；
- 阻塞延时；
- Flash 保存；
- `printf`。

低速 HMI 任务由 `APP_TaskScheduler_1ms_ISR()` 置位，再由 `APP_BackgroundLoop()` 执行：

```text
1 ms tick ISR:
  只置位 1 ms / 10 ms / 100 ms 标志

background loop:
  HMI_Task_1ms()
  HMI_Task_10ms()
  HMI_Task_100ms()
```

## 数据流

```text
按键事件
  -> 菜单状态机
  -> HMI 参数
  -> ControlIF_SetSetpoint()
  -> Simulink 控制算法读取 setpoint

Simulink 控制算法反馈
  -> ControlIF_SetFeedback()
  -> HMI_Task_1ms()
  -> HMI 参数快照
  -> HMI_Task_100ms() 显示
```

## ISR 调用边界

可以在控制 ISR 中使用：

- `ControlIF_GetSetpoint()`
- `ControlIF_SetFeedback()`

不要在控制 ISR 中使用：

- `HMI_Task_10ms()`
- `HMI_Task_100ms()`
- `Key_Task_10ms()`
- `OLED_Update()`
- 菜单和显示相关函数

`APP_TaskScheduler_1ms_ISR()` 可放在 1 ms 定时中断中，它只置位任务标志，不执行慢任务。
