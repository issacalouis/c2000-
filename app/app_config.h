#ifndef APP_CONFIG_H
#define APP_CONFIG_H

/*
 * File: app_config.h
 * Description: Global configuration for the C2000 power HMI template.
 * Notes: Keep user limits here so HMI, tests, and generated-control glue share
 *        the same safe parameter range.
 */

#define APP_VREF_DEFAULT        12.0f
#define APP_IREF_DEFAULT        2.0f

#define APP_VREF_MIN            0.0f
#define APP_VREF_MAX            48.0f

#define APP_IREF_MIN            0.0f
#define APP_IREF_MAX            10.0f

#define APP_HMI_TASK_1MS        1u
#define APP_KEY_TASK_10MS       10u
#define APP_MENU_TASK_20MS      20u
#define APP_DISPLAY_TASK_100MS  100u

/* Long press timing is counted in Key_Task_10ms() ticks. */
#define APP_KEY_LONG_PRESS_TICKS 80u

#endif
