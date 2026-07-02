# 控制 ISR 模板

```c
__interrupt void ControlISR(void)
{
    /* 1. Read ADC values. */
    /* 2. Call Simulink generated control step function. */
    /* 3. Update PWM compare registers. */
    /* 4. Update ControlIF feedback. */
    /* 5. Clear interrupt flags. */

    /* 禁止在这里刷新屏幕。 */
    /* 禁止在这里扫描键盘。 */
    /* 禁止在这里执行字符串格式化、printf、Flash 保存或阻塞延时。 */
}
```

控制 ISR 可以调用 `ControlIF_GetSetpoint()` 读取设定值，也可以调用 `ControlIF_SetFeedback()` 写入反馈值。当前接口层已经通过双缓冲和序列号校验返回多字段一致快照。
