# 软件架构

## 数据流

```text
用户按键 -> Key 事件 -> HMI 菜单 -> HMI 参数 -> ControlIF setpoint -> Simulink 控制算法
Simulink 控制算法 -> ControlIF feedback -> HMI 参数 -> OLED 页面刷新
```

## 初始化流

```text
APP_Init()
  -> Board_Init()
       -> Device_init()
       -> Device_initGPIO()
       -> BoardGPIO_Init()
       -> BoardI2C_Init()
       -> Key_Init()
       -> OLED_Init()
  -> ControlIF_Init()
  -> HMI_Init()
```

`HMI_Init()` 只初始化 HMI 参数、菜单和显示状态，不再重复初始化板级外设。

## 实时边界

高速控制 ISR 只允许执行 ADC 读取、控制 step、PWM 更新、必要保护判断和中断标志清除。按键扫描、OLED 刷新、字符串格式化、Flash 保存、`printf` 和阻塞延时都必须留在后台低速任务。

## 模块边界

- `app`：初始化入口和低速任务调度。
- `drivers_user`：F2800157 板级初始化、GPIO/I2C、Key、OLED。
- `hmi`：菜单、参数和显示逻辑，不直接访问 C2000 寄存器。
- `control_if`：HMI 与控制算法之间唯一共享数据层。
- `generated_control`：只保存自动生成控制算法代码。

## 周期任务

- 1 ms：轻量 HMI 预留任务。
- 10 ms：矩阵键盘扫描和事件生成。
- 20 ms：菜单状态机处理按键事件并更新参数。
- 100 ms：控制反馈同步和 OLED 刷新。
