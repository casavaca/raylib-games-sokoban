#pragma once

enum class GameEvent {
    EVENT_NULL,

    // control
    EVENT_MOVE_UP,
    EVENT_MOVE_DOWN,
    EVENT_MOVE_LEFT,
    EVENT_MOVE_RIGHT,
    EVENT_MOVE_RESTART,
    EVENT_MOVE_REGRET,
    EVENT_MOVE_CLICK, // + row, col

    // menu
    EVENT_MENU_PAUSE,
    EVENT_MENU_RESUME,
    EVENT_MENU_RESTART,
    EVENT_MENU_START,
    EVENT_MENU_EXIT,
};
