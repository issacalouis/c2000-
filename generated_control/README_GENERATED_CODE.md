# Simulink 生成代码目录

此目录用于放置 MATLAB/Simulink / Embedded Coder 生成的控制算法代码。

请不要手动修改生成文件，例如：

```text
model.c
model.h
model_private.h
model_data.c
ert_main.c
rtwtypes.h
```

推荐做法：

1. 重新生成算法代码时覆盖本目录中的生成文件；
2. HMI、按键、显示和 ControlIF 源码保持在目录外；
3. 由 Simulink Custom Code、C Caller 或外部 CCS 工程链接本模板；
4. 控制算法只通过 `control_if/control_interface.h` 读取设定值并写入反馈。
