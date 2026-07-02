# Repository Guidelines

## 项目结构与模块组织

本仓库是面向 TI C2000 F280015x 电源控制项目的嵌入式 C HMI 模板。应用初始化和调度逻辑放在 `app/`；HMI 状态、菜单、参数和显示页面放在 `hmi/`；可移植的 GPIO/I2C/SPI/按键/OLED 驱动放在 `drivers_user/`；HMI 与控制算法之间的共享数据边界放在 `control_if/`。Simulink 或 Embedded Coder 生成代码应放在 `generated_control/`，不要手工修改。架构说明、移植指南和 ISR 模板位于 `docs/`。可在 PC 上运行的 mock 测试位于 `tests/`。

## 构建、测试与开发命令

当前仓库没有统一的顶层构建脚本。目标固件构建时，请将这些源码导入 TI C2000 工具链或 Code Composer Studio 工程，并补全带有 `TODO(F280015x)` 标记的硬件 GPIO/I2C/SPI 绑定。

使用 C 编译器运行主机 mock 测试：

```sh
gcc -DUNIT_TEST -Iapp -Ihmi -Idrivers_user -Icontrol_if tests/key_scan_test.c drivers_user/key.c drivers_user/board_gpio.c -o key_scan_test
gcc -DUNIT_TEST -Iapp -Ihmi -Idrivers_user -Icontrol_if tests/hmi_mock_test.c hmi/*.c drivers_user/*.c control_if/control_interface.c -lm -o hmi_mock_test
./key_scan_test
./hmi_mock_test
```

在 Windows PowerShell 中，生成的可执行文件请使用 `.\key_scan_test.exe` 和 `.\hmi_mock_test.exe` 运行。

## 编码风格与命名约定

使用 C 语言，缩进为 4 个空格，花括号单独成行，保持与现有文件一致。公共模块 API 使用 `APP_`、`HMI_`、`Key_`、`OLED_`、`BoardGPIO_`、`ControlIF_` 等前缀。类型使用 `_t` 后缀，常量和宏使用大写命名。文件应按模块成对维护，例如 `hmi_menu.c` 与 `hmi_menu.h`。

## 测试指南

可在 PC 上测试的行为应加入 `tests/`，并使用 `-DUNIT_TEST` 编译以启用 mock hook。测试文件按被测功能命名，例如 `key_scan_test.c`。优先使用确定性的 `assert` 检查按键消抖时序、菜单跳转、设定值限幅和显示文本。修改 `hmi/`、`drivers_user/` 或 `control_if/` 前后，应运行现有两个 mock 测试。

## 提交与 Pull Request 指南

当前 Git 历史使用较短的小写提交主题，例如 `first`、`second`、`third`；后续建议使用更清晰的祈使句主题，例如 `add hmi debounce test` 或 `fix oled mock refresh`。Pull Request 应说明影响的模块、目标硬件假设、已运行的主机测试；若 UI 行为变化，还应附上截图、串口输出或 OLED 显示结果。

## Agent 专用说明

不要手工编辑 `generated_control/` 中的生成文件；应更新生成器并重新导入输出。实时控制 ISR 与低速 HMI 任务必须保持分离：按键扫描、字符串格式化、OLED 刷新、Flash 保存和阻塞调用应放在后台任务或 HMI 定时任务中。
