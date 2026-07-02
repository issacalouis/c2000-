/*
 * 文件: board_spi.c
 * 说明: 板级 SPI 传输占位实现。
 * 备注: 保持与上层分离，便于 OLED/TFT 驱动在不同 C2000 板卡间移植。
 */

#include "board_spi.h"

/*
 * 函数: BoardSPI_Init
 * 调用周期: 选择 SPI 显示器时调用一次。
 * ISR: 否。
 * 阻塞: 在本模板中为否。
 */
void BoardSPI_Init(void)
{
    /* TODO(F280015x): 配置 SPI 引脚、FIFO、时钟极性和波特率。 */
}

/*
 * 函数: BoardSPI_Write
 * 调用周期: 仅在低速显示任务中调用。
 * ISR: 否。
 * 阻塞: 目标实现应具有有界执行时间，且绝不能由控制 ISR 调用。
 */
void BoardSPI_Write(const uint8_t *data, uint16_t len)
{
    (void)data;
    (void)len;
    /* TODO(F280015x): 将字节写入 SPI FIFO，或排队发起异步事务。 */
}
