/*
 * 文件: board_i2c.c
 * 说明: 板级 I2C 传输占位实现。
 * 备注: 请用非阻塞或有界执行时间的 C2000Ware I2C 代码替换这些桩代码。
 */

#include "board_i2c.h"

/*
 * 函数: BoardI2C_Init
 * 调用周期: 在 OLED_Init() 期间调用一次。
 * ISR: 否。
 * 阻塞: 在本模板中为否。
 */
void BoardI2C_Init(void)
{
    /* TODO(F280015x): 配置 I2C 模块、引脚、波特率，以及中断/DMA（如有使用）。 */
}

/*
 * 函数: BoardI2C_Write
 * 调用周期: 在 100 ms 显示任务中的 OLED_Update() 内调用。
 * ISR: 否。
 * 阻塞: 目标实现应具有有界执行时间，且绝不能在控制 ISR 中运行。
 */
void BoardI2C_Write(uint8_t dev_addr, const uint8_t *data, uint16_t len)
{
    (void)dev_addr;
    (void)data;
    (void)len;
    /* TODO(F280015x): 将字节写入 I2C FIFO，或排队发起异步事务。 */
}
