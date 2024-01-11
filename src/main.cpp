#include "raylib-cpp.hpp"
#include "raylib.h"

#include "game.hpp"
#include "game_gui.hpp"
#include "game_config.hpp"

int main() {

    // Initialization
    //--------------------------------------------------------------------------------------
    raylib::Window window(800, 600, "sokoban");

    Sokoban game;
    game.LoadDefaultLevel();

    GameGui::Init();
    auto [screenWidth, screenHeight] = GameGui::GetWindowSize(game.GetState());

    window.SetSize(screenWidth, screenHeight);
    SetTargetFPS(60);               // Set our game to run at 60 frames-per-second
    SetExitKey(KEY_NULL);           // Disable quit-on-ESC
    //---------------------------------------------------------------------------------------

    // Main game loop
    int LastKeyPressed = KEY_NULL;

    while (!window.ShouldClose()) {    // Detect window close button or ESC key
#if defined(DEBUG)
        if (GetCharPressed() == 'q') {
            break;
        }
#endif

        // Update
        //----------------------------------------------------------------------------------
        if (auto key = GetKeyPressed())
            LastKeyPressed = key;

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) ||
            IsMouseButtonPressed(MOUSE_MIDDLE_BUTTON) ||
            IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) {
            auto [nrow, ncol] = GameGui::PixelToIndex(GetMousePosition());
            game.Click(nrow, ncol);
        } else if (IsKeyPressed(LastKeyPressed) || IsKeyPressedRepeat(LastKeyPressed)) {
            auto key = LastKeyPressed;
            if (GameConfig::IsUp     (key)) { game.PushNorth(); }
            if (GameConfig::IsDown   (key)) { game.PushSouth(); }
            if (GameConfig::IsRight  (key)) { game.PushEast (); }
            if (GameConfig::IsLeft   (key)) { game.PushWest (); }
            if (GameConfig::IsRestart(key)) { game.Restart  (); }
            if (GameConfig::IsRegret (key)) { game.Regret   (); }
        }
        // TODO: do nothing when no need to redraw?
 
        //----------------------------------------------------------------------------------
        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();
        {
            window.ClearBackground(RAYWHITE);
            GameGui::Draw(game.GetState());
        }
        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    return 0;
}
