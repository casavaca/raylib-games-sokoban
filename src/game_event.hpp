#pragma once

// Suppose you have a 2D FPS game,
//
// GameEvent actually consists of 2 components,
// Those that should be handled by a game.
//   e.g., movement, shoot, etc.
// Those that should be handled by gui library
//   e.g., button buy_a_gun clicked.
//
// Suppose that game core engine doesn't care how it is displayed.
// So, buy_a_gun event needs to be converted into
//     game_add_gun         XXX
//     game_switch_to_gun   XXX
//     game_decrease_money $YYY
//
// We could just do this in the gui:
// ```
//   if (buy_a_gun clicked) {
//     game_add_gun         XXX
//     game_switch_to_gun   XXX
//     game_decrease_money $YYY
//   }
// ```
// But how do you test it?
// Here, we make the gui code return all gui events:
// `auto gui_events = Gui::processInput()`. // gui_events contains buy_a_gun
// `Gui::processEvents(gui_events)`.        // then process them.
//
// In this case, we can mock Gui::processInput easily.

enum class GameEvent {
    // control
    EVENT_MOVE_UP,
    EVENT_MOVE_DOWN,
    EVENT_MOVE_LEFT,
    EVENT_MOVE_RIGHT,
    EVENT_MOVE_RESTART,
    EVENT_MOVE_REGRET,
    EVENT_MOVE_CLICK, // + row, col
};

enum class GuiEvent {
    // menu
    EVENT_NULL,
    EVENT_MENU_PAUSE,
    EVENT_MENU_RESUME,
    EVENT_MENU_RESTART,
    EVENT_MENU_START,
    EVENT_MENU_LEVEL_FINISHED,
    EVENT_MENU_NEXT_LEVEL,
    EVENT_MENU_EXIT,
};
