# 软件架构

## 数据流

```text
用户按键 -> Key 事件 -> HMI 菜单 -> HMI 参数 -> ControlIF setpoint -> Simulink 控制算法
Simulink 控制算法 -> ControlIF feedback -> HMI 参数 -> OLED 页面刷新
```

## 实时边界

高速控制 ISR 只允许执行 ADC 读取、控制 step、PWM 更新、必要保护判断和中断标志清除。按键扫描、OLED 刷新、字符串格式化、Flash 保存、printf 和阻塞延时都必须留在后台低速任务。

## 模块边界

- `hmi` 不直接访问 C2000 寄存器。
- `drivers_user` 集中所有 GPIO/I2C 硬件差异，以及按键和 OLED 驱动抽象。
- `control_if` 是 HMI 与控制算法的唯一共享数据层。
- `generated_control` 只保存自动生成代码，重新生成时不影响 HMI 模块。

## 周期任务

- 1 ms: 只保留轻量 HMI 软计时入口。
- 10 ms: 矩阵键盘扫描和事件生成。
- 20 ms: 菜单状态机处理按键事件并更新参数。
- 100 ms: 控制反馈同步和屏幕刷新。
