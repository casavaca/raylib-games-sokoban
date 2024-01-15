#include "raylib-cpp.hpp"
#include "raylib.h"
#include "raygui.h"
#include "rgestures.h"

#include "game.hpp"
#include "game_gui.hpp"
#include "game_config.hpp"

using namespace GameGui;

int main() {

    // Initialization
    //--------------------------------------------------------------------------------------
    raylib::Window window(800, 600, "sokoban");

    GuiLoadStyle("assets/styles/cyber/cyber.rgs");
    GuiSetStyle(DEFAULT, TEXT_SIZE, 40);
    Sokoban game;
    game.LoadDefaultLevels();

    GameGui::Init();
    SetTargetFPS(60);               // Set our game to run at 60 frames-per-second
    SetExitKey(KEY_NULL);           // Disable quit-on-ESC
    //---------------------------------------------------------------------------------------

    // Main game loop
    while (!window.ShouldClose()) {    // Detect window close button or ESC key
        // Update
        //----------------------------------------------------------------------------------
        Sokoban::Pos pos = GameGui::PixelToPos(GetMousePosition());
        auto [gameEvents, guiEvent] = GameGui::CookInputEvent(game);
        game.ProcessEvent(gameEvents, pos);
        GameGui::ProcessGuiEvent(guiEvent, game);

        //----------------------------------------------------------------------------------
        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();
        window.ClearBackground(LIGHTGRAY);
        auto event  = GameGui::Draw(window, game);
        GameGui::ProcessGuiEvent(event, game);

        EndDrawing();
        //----------------------------------------------------------------------------------

    }

    return 0;
}
