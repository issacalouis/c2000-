#ifndef BOARD_PINMAP_H
#define BOARD_PINMAP_H

/*
 * File: board_pinmap.h
 * Description: LAUNCHXL-F2800157 HMI pin and peripheral configuration.
 */

#include <stdint.h>

#ifndef UNIT_TEST
#include "driverlib.h"
#endif

#define BOARD_MCU_NAME                 "TMS320F2800157"
#define BOARD_NAME                     "LAUNCHXL-F2800157"
#define BOARD_SYSCLK_FREQ_HZ           120000000UL

#define BOARD_OLED_I2C_BASE            I2CA_BASE
#define BOARD_OLED_SDA_GPIO            42U
#define BOARD_OLED_SCL_GPIO            43U
#define BOARD_OLED_ADDR_7BIT           0x3CU
#define BOARD_OLED_I2C_FREQ_HZ         400000UL

#define BOARD_OLED_WIDTH               128U
#define BOARD_OLED_HEIGHT              64U
#define BOARD_OLED_PAGE_NUM            8U
#define BOARD_OLED_BUF_SIZE            1024U

#define KEY_ROW_NUM                    4U
#define KEY_COL_NUM                    4U

#define KEY_ROW0_GPIO                  25U
#define KEY_ROW1_GPIO                  26U
#define KEY_ROW2_GPIO                  27U
#define KEY_ROW3_GPIO                  44U

#define KEY_COL0_GPIO                  45U
#define KEY_COL1_GPIO                  46U
#define KEY_COL2_GPIO                  48U
#define KEY_COL3_GPIO                  33U

#define KEY_ACTIVE_LEVEL               0U
#define KEY_INACTIVE_LEVEL             1U

#define KEY_SCAN_PERIOD_MS             10U
#define OLED_REFRESH_PERIOD_MS         100U

#endif
