/*
 * 文件: key.c
 * 说明: 面向矩阵键盘的消抖短按/长按事件扫描器。
 * 备注: 需由后台调度器每 10 ms 调用一次；不使用阻塞延时。
 */

#include "key.h"

#include "app_config.h"
#include "board_gpio.h"
#include "board_pinmap.h"

#define KEY_DEBOUNCE_TICKS 3u
#define KEY_MATRIX_ROWS    KEY_ROW_NUM
#define KEY_MATRIX_COLS    KEY_COL_NUM

typedef struct
{
    uint16_t raw_last;
    uint16_t stable;
    uint16_t active_key;
    uint16_t debounce_ticks;
    uint16_t long_ticks;
    uint16_t long_sent;
    KeyEvent_t pending_event;
} Key_State_t;

static Key_State_t g_key_state;

static uint16_t Key_FirstPressedMask(uint16_t mask)
{
    static const uint16_t key_order[] =
    {
        KEY_MASK_UP,
        KEY_MASK_DOWN,
        KEY_MASK_LEFT,
        KEY_MASK_RIGHT,
        KEY_MASK_OK,
        KEY_MASK_BACK,
        KEY_MASK_RUN
    };
    uint16_t index;

    for (index = 0u; index < (uint16_t)(sizeof(key_order) / sizeof(key_order[0])); index++)
    {
        if ((mask & key_order[index]) != 0u)
        {
            return key_order[index];
        }
    }

    return 0u;
}

static KeyEvent_t Key_ShortEventFromMask(uint16_t mask)
{
    switch (mask)
    {
    case KEY_MASK_UP:
        return KEY_EVENT_UP;
    case KEY_MASK_DOWN:
        return KEY_EVENT_DOWN;
    case KEY_MASK_LEFT:
        return KEY_EVENT_LEFT;
    case KEY_MASK_RIGHT:
        return KEY_EVENT_RIGHT;
    case KEY_MASK_OK:
        return KEY_EVENT_OK;
    case KEY_MASK_BACK:
        return KEY_EVENT_BACK;
    case KEY_MASK_RUN:
        return KEY_EVENT_RUN;
    default:
        return KEY_EVENT_NONE;
    }
}

static KeyEvent_t Key_LongEventFromMask(uint16_t mask)
{
    switch (mask)
    {
    case KEY_MASK_UP:
        return KEY_EVENT_LONG_UP;
    case KEY_MASK_DOWN:
        return KEY_EVENT_LONG_DOWN;
    case KEY_MASK_OK:
        return KEY_EVENT_LONG_OK;
    case KEY_MASK_RUN:
        return KEY_EVENT_LONG_RUN;
    default:
        return KEY_EVENT_NONE;
    }
}

#if (KEY_MODE == KEY_MODE_INDEPENDENT)
static uint16_t Key_ReadIndependentMask(void)
{
    uint16_t mask;

    mask = 0u;
    mask |= (BoardGPIO_ReadKeyUp() != 0u) ? KEY_MASK_UP : 0u;
    mask |= (BoardGPIO_ReadKeyDown() != 0u) ? KEY_MASK_DOWN : 0u;
    mask |= (BoardGPIO_ReadKeyLeft() != 0u) ? KEY_MASK_LEFT : 0u;
    mask |= (BoardGPIO_ReadKeyRight() != 0u) ? KEY_MASK_RIGHT : 0u;
    mask |= (BoardGPIO_ReadKeyOk() != 0u) ? KEY_MASK_OK : 0u;
    mask |= (BoardGPIO_ReadKeyBack() != 0u) ? KEY_MASK_BACK : 0u;
    mask |= (BoardGPIO_ReadKeyRun() != 0u) ? KEY_MASK_RUN : 0u;

    return mask;
}
#endif

#if (KEY_MODE == KEY_MODE_MATRIX)
static uint16_t Key_ReadMatrixMask(void)
{
    uint16_t row;
    uint16_t column;
    uint16_t matrix_mask;
    uint16_t key_mask;

    matrix_mask = 0u;
    for (row = 0u; row < KEY_MATRIX_ROWS; row++)
    {
        BoardGPIO_SetMatrixRow((uint8_t)row);
        for (column = 0u; column < KEY_MATRIX_COLS; column++)
        {
            if (BoardGPIO_ReadMatrixColumn((uint8_t)column) != 0u)
            {
                matrix_mask |= (uint16_t)(1u << ((row * KEY_MATRIX_COLS) + column));
            }
        }
    }

    key_mask = 0u;
    key_mask |= (matrix_mask & (1u << ((0u * KEY_MATRIX_COLS) + 0u))) != 0u ? KEY_MASK_UP : 0u;
    key_mask |= (matrix_mask & (1u << ((0u * KEY_MATRIX_COLS) + 1u))) != 0u ? KEY_MASK_DOWN : 0u;
    key_mask |= (matrix_mask & (1u << ((0u * KEY_MATRIX_COLS) + 2u))) != 0u ? KEY_MASK_LEFT : 0u;
    key_mask |= (matrix_mask & (1u << ((0u * KEY_MATRIX_COLS) + 3u))) != 0u ? KEY_MASK_RIGHT : 0u;
    key_mask |= (matrix_mask & (1u << ((1u * KEY_MATRIX_COLS) + 3u))) != 0u ? KEY_MASK_OK : 0u;
    key_mask |= (matrix_mask & (1u << ((2u * KEY_MATRIX_COLS) + 3u))) != 0u ? KEY_MASK_BACK : 0u;
    key_mask |= (matrix_mask & (1u << ((3u * KEY_MATRIX_COLS) + 3u))) != 0u ? KEY_MASK_RUN : 0u;

    return key_mask;
}
#endif

static uint16_t Key_ReadMask(void)
{
#if (KEY_MODE == KEY_MODE_MATRIX)
    return Key_ReadMatrixMask();
#else
    return Key_ReadIndependentMask();
#endif
}

static void Key_StoreEvent(KeyEvent_t event)
{
    if ((event != KEY_EVENT_NONE) && (g_key_state.pending_event == KEY_EVENT_NONE))
    {
        g_key_state.pending_event = event;
    }
}

/*
 * 函数: Key_Init
 * 调用周期: 在 HMI_Init() 中调用一次。
 * ISR: 否。
 * 阻塞: 否。
 */
void Key_Init(void)
{
    g_key_state.raw_last = 0u;
    g_key_state.stable = 0u;
    g_key_state.active_key = 0u;
    g_key_state.debounce_ticks = 0u;
    g_key_state.long_ticks = 0u;
    g_key_state.long_sent = 0u;
    g_key_state.pending_event = KEY_EVENT_NONE;
}

/*
 * 函数: Key_Task_10ms
 * 调用周期: 必须严格每 10 ms 调用一次，以保证消抖和长按计时可预测。
 * ISR: 否，因为矩阵扫描可能访问多个 GPIO。
 * 阻塞: 否。
 */
void Key_Task_10ms(void)
{
    uint16_t raw;
    uint16_t next_stable;
    uint16_t previous_active;

    raw = Key_FirstPressedMask(Key_ReadMask());

    if (raw == g_key_state.raw_last)
    {
        if (g_key_state.debounce_ticks < KEY_DEBOUNCE_TICKS)
        {
            g_key_state.debounce_ticks++;
        }
    }
    else
    {
        g_key_state.raw_last = raw;
        g_key_state.debounce_ticks = 0u;
    }

    if (g_key_state.debounce_ticks < KEY_DEBOUNCE_TICKS)
    {
        return;
    }

    next_stable = raw;
    if (next_stable == g_key_state.stable)
    {
        if (g_key_state.active_key != 0u && g_key_state.long_sent == 0u)
        {
            if (g_key_state.long_ticks < APP_KEY_LONG_PRESS_TICKS)
            {
                g_key_state.long_ticks++;
            }

            if (g_key_state.long_ticks >= APP_KEY_LONG_PRESS_TICKS)
            {
                Key_StoreEvent(Key_LongEventFromMask(g_key_state.active_key));
                g_key_state.long_sent = 1u;
            }
        }
        return;
    }

    previous_active = g_key_state.active_key;
    g_key_state.stable = next_stable;

    if (next_stable != 0u)
    {
        g_key_state.active_key = next_stable;
        g_key_state.long_ticks = 0u;
        g_key_state.long_sent = 0u;
    }
    else
    {
        if ((previous_active != 0u) && (g_key_state.long_sent == 0u))
        {
            Key_StoreEvent(Key_ShortEventFromMask(previous_active));
        }

        g_key_state.active_key = 0u;
        g_key_state.long_ticks = 0u;
        g_key_state.long_sent = 0u;
    }
}

/*
 * 函数: Key_GetEvent
 * 调用周期: 在 Key_Task_10ms() 之后调用。
 * ISR: 否。
 * 阻塞: 否。
 */
KeyEvent_t Key_GetEvent(void)
{
    KeyEvent_t event;

    event = g_key_state.pending_event;
    g_key_state.pending_event = KEY_EVENT_NONE;
    return event;
}
