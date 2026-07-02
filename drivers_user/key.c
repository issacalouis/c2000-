/*
 * File: key.c
 * Description: Debounced short/long key event scanner.
 * Notes: Non-blocking 10 ms task; never access control variables here.
 */
#include "key.h"
#include "app_config.h"
#include "board_gpio.h"

typedef enum
{
    KEY_ID_NONE = 0,
    KEY_ID_UP,
    KEY_ID_DOWN,
    KEY_ID_LEFT,
    KEY_ID_RIGHT,
    KEY_ID_OK,
    KEY_ID_BACK,
    KEY_ID_RUN
} KeyId_t;

typedef struct
{
    uint16_t raw_current;
    uint16_t raw_last;
    uint16_t stable;
    uint16_t debounce_ticks;
    uint16_t press_ticks;
    uint16_t long_reported;
    KeyId_t active_key;
    KeyEvent_t pending_event;
} Key_State_t;

static Key_State_t g_key_state;

static uint16_t Key_MaskFromId(KeyId_t key)
{
    return (uint16_t)(1u << ((uint16_t)key - 1u));
}

static KeyEvent_t Key_ShortEventFromId(KeyId_t key)
{
    switch (key)
    {
    case KEY_ID_UP: return KEY_EVENT_UP;
    case KEY_ID_DOWN: return KEY_EVENT_DOWN;
    case KEY_ID_LEFT: return KEY_EVENT_LEFT;
    case KEY_ID_RIGHT: return KEY_EVENT_RIGHT;
    case KEY_ID_OK: return KEY_EVENT_OK;
    case KEY_ID_BACK: return KEY_EVENT_BACK;
    case KEY_ID_RUN: return KEY_EVENT_RUN;
    case KEY_ID_NONE:
    default: return KEY_EVENT_NONE;
    }
}

static KeyEvent_t Key_LongEventFromId(KeyId_t key)
{
    switch (key)
    {
    case KEY_ID_UP: return KEY_EVENT_LONG_UP;
    case KEY_ID_DOWN: return KEY_EVENT_LONG_DOWN;
    case KEY_ID_OK: return KEY_EVENT_LONG_OK;
    case KEY_ID_RUN: return KEY_EVENT_LONG_RUN;
    case KEY_ID_LEFT:
    case KEY_ID_RIGHT:
    case KEY_ID_BACK:
    case KEY_ID_NONE:
    default: return KEY_EVENT_NONE;
    }
}

static KeyId_t Key_FirstPressed(uint16_t mask)
{
    if ((mask & Key_MaskFromId(KEY_ID_UP)) != 0u) { return KEY_ID_UP; }
    if ((mask & Key_MaskFromId(KEY_ID_DOWN)) != 0u) { return KEY_ID_DOWN; }
    if ((mask & Key_MaskFromId(KEY_ID_LEFT)) != 0u) { return KEY_ID_LEFT; }
    if ((mask & Key_MaskFromId(KEY_ID_RIGHT)) != 0u) { return KEY_ID_RIGHT; }
    if ((mask & Key_MaskFromId(KEY_ID_OK)) != 0u) { return KEY_ID_OK; }
    if ((mask & Key_MaskFromId(KEY_ID_BACK)) != 0u) { return KEY_ID_BACK; }
    if ((mask & Key_MaskFromId(KEY_ID_RUN)) != 0u) { return KEY_ID_RUN; }
    return KEY_ID_NONE;
}

static void Key_PushEvent(KeyEvent_t event)
{
    if ((event != KEY_EVENT_NONE) && (g_key_state.pending_event == KEY_EVENT_NONE))
    {
        g_key_state.pending_event = event;
    }
}

static uint16_t Key_ReadIndependentMask(void)
{
    uint16_t mask = 0u;

    if (BoardGPIO_ReadKeyUp() != BOARD_KEY_RELEASED) { mask |= Key_MaskFromId(KEY_ID_UP); }
    if (BoardGPIO_ReadKeyDown() != BOARD_KEY_RELEASED) { mask |= Key_MaskFromId(KEY_ID_DOWN); }
    if (BoardGPIO_ReadKeyLeft() != BOARD_KEY_RELEASED) { mask |= Key_MaskFromId(KEY_ID_LEFT); }
    if (BoardGPIO_ReadKeyRight() != BOARD_KEY_RELEASED) { mask |= Key_MaskFromId(KEY_ID_RIGHT); }
    if (BoardGPIO_ReadKeyOk() != BOARD_KEY_RELEASED) { mask |= Key_MaskFromId(KEY_ID_OK); }
    if (BoardGPIO_ReadKeyBack() != BOARD_KEY_RELEASED) { mask |= Key_MaskFromId(KEY_ID_BACK); }
    if (BoardGPIO_ReadKeyRun() != BOARD_KEY_RELEASED) { mask |= Key_MaskFromId(KEY_ID_RUN); }

    return mask;
}

#if (KEY_MODE == KEY_MODE_MATRIX)
static uint16_t Key_ReadMatrixMask(void)
{
    static const KeyId_t map[4][4] =
    {
        { KEY_ID_UP, KEY_ID_DOWN, KEY_ID_LEFT, KEY_ID_RIGHT },
        { KEY_ID_OK, KEY_ID_BACK, KEY_ID_RUN, KEY_ID_NONE },
        { KEY_ID_NONE, KEY_ID_NONE, KEY_ID_NONE, KEY_ID_NONE },
        { KEY_ID_NONE, KEY_ID_NONE, KEY_ID_NONE, KEY_ID_NONE }
    };
    uint16_t row;
    uint16_t col;
    uint16_t mask = 0u;

    for (row = 0u; row < 4u; row++)
    {
        BoardGPIO_SetMatrixRow(row, 1u);

        for (col = 0u; col < 4u; col++)
        {
            if (BoardGPIO_ReadMatrixColumn(col) != BOARD_KEY_RELEASED)
            {
                if (map[row][col] != KEY_ID_NONE)
                {
                    mask |= Key_MaskFromId(map[row][col]);
                }
            }
        }

        BoardGPIO_SetMatrixRow(row, 0u);
    }

    return mask;
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

void Key_Init(void)
{
    BoardGPIO_Init();

    g_key_state.raw_current = 0u;
    g_key_state.raw_last = 0u;
    g_key_state.stable = 0u;
    g_key_state.debounce_ticks = 0u;
    g_key_state.press_ticks = 0u;
    g_key_state.long_reported = 0u;
    g_key_state.active_key = KEY_ID_NONE;
    g_key_state.pending_event = KEY_EVENT_NONE;
}

void Key_Task_10ms(void)
{
    KeyId_t key;
    KeyEvent_t long_event;

    g_key_state.raw_current = Key_ReadMask();

    if (g_key_state.raw_current == g_key_state.raw_last)
    {
        if (g_key_state.debounce_ticks < KEY_DEBOUNCE_TICKS)
        {
            g_key_state.debounce_ticks++;
        }
    }
    else
    {
        g_key_state.debounce_ticks = 0u;
        g_key_state.raw_last = g_key_state.raw_current;
    }

    if (g_key_state.debounce_ticks < KEY_DEBOUNCE_TICKS)
    {
        return;
    }

    if (g_key_state.stable != g_key_state.raw_current)
    {
        if ((g_key_state.stable != 0u) && (g_key_state.raw_current == 0u))
        {
            if (g_key_state.long_reported == 0u)
            {
                Key_PushEvent(Key_ShortEventFromId(g_key_state.active_key));
            }
            g_key_state.press_ticks = 0u;
            g_key_state.long_reported = 0u;
            g_key_state.active_key = KEY_ID_NONE;
        }
        else if ((g_key_state.stable == 0u) && (g_key_state.raw_current != 0u))
        {
            g_key_state.active_key = Key_FirstPressed(g_key_state.raw_current);
            g_key_state.press_ticks = 0u;
            g_key_state.long_reported = 0u;
        }

        g_key_state.stable = g_key_state.raw_current;
    }

    key = g_key_state.active_key;
    if ((key != KEY_ID_NONE) && (g_key_state.stable != 0u))
    {
        if (g_key_state.press_ticks < 0xFFFFu)
        {
            g_key_state.press_ticks++;
        }

        if ((g_key_state.press_ticks >= KEY_LONG_TICKS) && (g_key_state.long_reported == 0u))
        {
            long_event = Key_LongEventFromId(key);
            Key_PushEvent(long_event);
            g_key_state.long_reported = 1u;
        }
    }
}

KeyEvent_t Key_GetEvent(void)
{
    KeyEvent_t event = g_key_state.pending_event;

    g_key_state.pending_event = KEY_EVENT_NONE;
    return event;
}
