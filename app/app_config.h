/*
 * File: app_config.h
 * Description: Project-level configuration for the portable C2000 HMI layer.
 * Notes: Keep hardware-specific details in drivers_user.
 */
#ifndef APP_CONFIG_H
#define APP_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

#define APP_VREF_DEFAULT       12.0f
#define APP_IREF_DEFAULT       2.0f

#define APP_VREF_MIN           0.0f
#define APP_VREF_MAX           48.0f

#define APP_IREF_MIN           0.0f
#define APP_IREF_MAX           10.0f

#define APP_HMI_TASK_1MS       1u
#define APP_KEY_TASK_10MS      10u
#define APP_DISPLAY_TASK_100MS 100u

#define APP_ENABLE_OFF         0u
#define APP_ENABLE_ON          1u

#define APP_MODE_MIN           0u
#define APP_MODE_MAX           3u

#define KEY_MODE_INDEPENDENT   0
#define KEY_MODE_MATRIX        1
#define KEY_MODE               KEY_MODE_INDEPENDENT

#define KEY_DEBOUNCE_TICKS     3u
#define KEY_LONG_TICKS         80u

#define BOARD_KEY_RELEASED     0u
#define BOARD_KEY_PRESSED      1u

#ifdef __cplusplus
}
#endif

#endif
