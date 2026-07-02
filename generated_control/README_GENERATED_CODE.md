# Simulink 生成代码目录

此目录用于放置 MATLAB/Simulink / Embedded Coder 自动生成的控制算法文件，例如：

- `model.c`
- `model.h`
- `model_private.h`
- `model_data.c`
- `rtwtypes.h`

不要手工修改这些生成文件。HMI、键盘、OLED 和控制数据接口都放在本目录之外，通过 `control_if` 与生成代码交互。