/*
 * File: board_i2c.c
 * Description: LAUNCHXL-F2800157 I2CA transport for the SSD1306 OLED.
 */

#include "board_i2c.h"

#include "board_pinmap.h"

#ifndef UNIT_TEST
static void BoardI2C_ConfigPins(void)
{
#ifdef GPIO_42_I2CA_SDA
    GPIO_setPinConfig(GPIO_42_I2CA_SDA);
#endif
#ifdef GPIO_43_I2CA_SCL
    GPIO_setPinConfig(GPIO_43_I2CA_SCL);
#endif

    GPIO_setPadConfig(BOARD_OLED_SDA_GPIO, GPIO_PIN_TYPE_PULLUP);
    GPIO_setPadConfig(BOARD_OLED_SCL_GPIO, GPIO_PIN_TYPE_PULLUP);
    GPIO_setQualificationMode(BOARD_OLED_SDA_GPIO, GPIO_QUAL_ASYNC);
    GPIO_setQualificationMode(BOARD_OLED_SCL_GPIO, GPIO_QUAL_ASYNC);
}

static uint16_t BoardI2C_WaitBusReady(void)
{
    uint32_t timeout;

    timeout = 100000UL;
    while ((I2C_isBusBusy(BOARD_OLED_I2C_BASE) != 0u) && (timeout > 0UL))
    {
        timeout--;
    }

    return (timeout > 0UL) ? 1u : 0u;
}
#endif

void BoardI2C_Init(void)
{
#ifndef UNIT_TEST
    BoardI2C_ConfigPins();

    I2C_disableModule(BOARD_OLED_I2C_BASE);
    I2C_initMaster(BOARD_OLED_I2C_BASE,
                   BOARD_SYSCLK_FREQ_HZ,
                   BOARD_OLED_I2C_FREQ_HZ,
                   I2C_DUTYCYCLE_50);
    I2C_setConfig(BOARD_OLED_I2C_BASE, I2C_MASTER_SEND_MODE);
    I2C_setTargetAddress(BOARD_OLED_I2C_BASE, BOARD_OLED_ADDR_7BIT);
    I2C_enableFIFO(BOARD_OLED_I2C_BASE);
    I2C_enableModule(BOARD_OLED_I2C_BASE);
#endif
}

void BoardI2C_Write(uint8_t dev_addr, const uint8_t *data, uint16_t len)
{
#ifndef UNIT_TEST
    uint16_t index;
    uint32_t timeout;

    if ((data == 0) || (len == 0u))
    {
        return;
    }

    if (BoardI2C_WaitBusReady() == 0u)
    {
        return;
    }

    I2C_setTargetAddress(BOARD_OLED_I2C_BASE, dev_addr);
    I2C_setDataCount(BOARD_OLED_I2C_BASE, len);
    I2C_setConfig(BOARD_OLED_I2C_BASE, I2C_MASTER_SEND_MODE);

    for (index = 0u; index < len; index++)
    {
        I2C_putData(BOARD_OLED_I2C_BASE, data[index]);
    }

    I2C_sendStartCondition(BOARD_OLED_I2C_BASE);
    I2C_sendStopCondition(BOARD_OLED_I2C_BASE);

    timeout = 100000UL;
    while ((I2C_getStopConditionStatus(BOARD_OLED_I2C_BASE) != 0u) &&
           (timeout > 0UL))
    {
        timeout--;
    }
#else
    (void)dev_addr;
    (void)data;
    (void)len;
#endif
}
