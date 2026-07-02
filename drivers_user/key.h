/*
 * File: key.h
 * Description: Non-blocking key scanner event API.
 * Notes: Call Key_Task_10ms every 10 ms. The module emits events only.
 */
#ifndef KEY_H
#define KEY_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
    KEY_EVENT_NONE = 0,
    KEY_EVENT_UP,
    KEY_EVENT_DOWN,
    KEY_EVENT_LEFT,
    KEY_EVENT_RIGHT,
    KEY_EVENT_OK,
    KEY_EVENT_BACK,
    KEY_EVENT_RUN,
    KEY_EVENT_LONG_UP,
    KEY_EVENT_LONG_DOWN,
    KEY_EVENT_LONG_OK,
    KEY_EVENT_LONG_RUN
} KeyEvent_t;

void Key_Init(void);
void Key_Task_10ms(void);
KeyEvent_t Key_GetEvent(void);

#ifdef __cplusplus
}
#endif

#endif
