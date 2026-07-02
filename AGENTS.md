# Repository Guidelines

## 项目结构与模块组织

本仓库面向 `TMS320F2800157` / `LAUNCHXL-F2800157` 电源控制 HMI。`app/` 放应用初始化和调度；`hmi/` 放菜单、参数和显示逻辑；`drivers_user/` 放板级初始化、GPIO/I2C、按键和 OLED 驱动；`control_if/` 是 HMI 与控制算法的数据边界；`generated_control/` 只放 Simulink / Embedded Coder 生成代码；`docs/` 放设计和移植文档；`tests/` 放 PC mock 测试。

## 构建、测试与开发命令

目标固件应导入 TI C2000 工具链或 Code Composer Studio，并使用 C2000Ware DriverLib。PC mock 测试可用：

```sh
gcc -DUNIT_TEST -Iapp -Ihmi -Idrivers_user -Icontrol_if tests/key_scan_test.c drivers_user/key.c drivers_user/board_gpio.c -o key_scan_test
gcc -DUNIT_TEST -Iapp -Ihmi -Idrivers_user -Icontrol_if tests/hmi_mock_test.c hmi/*.c drivers_user/*.c control_if/control_interface.c -lm -o hmi_mock_test
./key_scan_test
./hmi_mock_test
```

Windows PowerShell 中运行 `.\key_scan_test.exe` 和 `.\hmi_mock_test.exe`。

## 编码风格与命名约定

使用 C 语言，4 空格缩进，花括号单独成行。公共 API 使用模块前缀，例如 `APP_`、`HMI_`、`Key_`、`OLED_`、`BoardGPIO_`、`ControlIF_`。类型使用 `_t` 后缀，宏和常量使用大写命名。硬件引脚优先集中到 `drivers_user/board_pinmap.h`。

## 测试指南

PC 可测行为放在 `tests/`，使用 `-DUNIT_TEST` 启用 mock。测试文件按功能命名，例如 `key_scan_test.c`。修改 `hmi/`、`drivers_user/` 或 `control_if/` 后，应运行两个现有 mock 测试。

## 提交与 Pull Request 指南

建议使用简洁祈使句提交主题，例如 `add f2800157 board init` 或 `fix oled page update`。PR 应说明影响模块、目标硬件假设、已运行测试；如 UI 行为变化，应附 OLED 显示结果或截图。

## Agent 专用说明

不要手工编辑 `generated_control/` 中的生成文件。`Board_Init()` 统一管理板级初始化，避免外部重复调用 `BoardGPIO_Init()`、`BoardI2C_Init()`、`Key_Init()` 或 `OLED_Init()`。控制 ISR 中不要执行按键扫描、OLED 刷新、字符串格式化、Flash 保存或阻塞调用。
