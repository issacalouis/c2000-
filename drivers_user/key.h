#ifndef KEY_H
#define KEY_H

/*
 * File: key.h
 * Description: Non-blocking key event scanner interface.
 * Notes: The key module emits events only; menu logic decides their meaning.
 */

#include <stdint.h>

#define KEY_MODE_INDEPENDENT 0
#define KEY_MODE_MATRIX      1

#ifndef KEY_MODE
#define KEY_MODE KEY_MODE_MATRIX
#endif

#define KEY_MASK_UP    0x0001u
#define KEY_MASK_DOWN  0x0002u
#define KEY_MASK_LEFT  0x0004u
#define KEY_MASK_RIGHT 0x0008u
#define KEY_MASK_OK    0x0010u
#define KEY_MASK_BACK  0x0020u
#define KEY_MASK_RUN   0x0040u

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
