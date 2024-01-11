#include "raylib-cpp.hpp"
#include "raylib.h"
#include "raygui.h"

#include "game.hpp"
#include "game_gui.hpp"
#include "game_config.hpp"

using namespace GameGui;

int main() {

    // Initialization
    //--------------------------------------------------------------------------------------
    raylib::Window window(800, 600, "sokoban");

    GuiLoadStyle("assets/styles/bluish/style_bluish.rgs");
    GuiSetStyle(DEFAULT, TEXT_SIZE, 40);
    Sokoban game;
    game.LoadDefaultLevel();

    GameGui::Init();
    SetTargetFPS(60);               // Set our game to run at 60 frames-per-second
    SetExitKey(KEY_NULL);           // Disable quit-on-ESC
    //---------------------------------------------------------------------------------------

    // Main game loop
    int lastKeyPressed = KEY_NULL;

    while (!window.ShouldClose()) {    // Detect window close button or ESC key
#if defined(DEBUG)
        if (GetCharPressed() == 'q') {
            break;
        }
#endif

        // Update
        //----------------------------------------------------------------------------------
        if (auto key = GetKeyPressed())
            lastKeyPressed = key;

        if (GetGameScene() != MAIN_GAME_SCENE) {
            if (lastKeyPressed == KEY_SPACE || lastKeyPressed == KEY_ENTER) {
                SetGameScene(MAIN_GAME_SCENE);
            } else if (GameConfig::IsRestart(lastKeyPressed)) {
                game.Restart();
            }
        } else if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) ||
            IsMouseButtonPressed(MOUSE_MIDDLE_BUTTON) ||
            IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) {
            auto [nrow, ncol] = GameGui::PixelToIndex(GetMousePosition());
            game.Click(nrow, ncol);
        } else if (IsKeyPressed(lastKeyPressed) || IsKeyPressedRepeat(lastKeyPressed)) {
            auto key = lastKeyPressed;
            if (GameConfig::IsUp     (key)) { game.PushNorth(); }
            if (GameConfig::IsDown   (key)) { game.PushSouth(); }
            if (GameConfig::IsRight  (key)) { game.PushEast (); }
            if (GameConfig::IsLeft   (key)) { game.PushWest (); }
            if (GameConfig::IsRestart(key)) { game.Restart  (); }
            if (GameConfig::IsRegret (key)) { game.Regret   (); }
            if (key == KEY_ESCAPE) {
                SetGameScene(ESC_SCENE);
            }
        }
        // TODO: do nothing when no need to redraw?
 
        //----------------------------------------------------------------------------------
        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();
        window.ClearBackground(RAYWHITE);
        switch(GetGameScene()) {
        case START_SCENE: {
            char textBoxText[64] = "Start";
            bool textBoxEditMode = false;
            if (GuiTextBox((Rectangle){ 45, 215, 325, 60 }, textBoxText, 64, textBoxEditMode)) {
                SetGameScene(MAIN_GAME_SCENE);
                auto [screenWidth, screenHeight] = GameGui::GetWindowSize(game.GetState());
                window.SetSize(screenWidth, screenHeight);
            }
            char exitBoxText[64] = "Exit";
            bool exitBoxEditMode = false;
            if (GuiTextBox((Rectangle){ 45, 315, 325, 60 }, exitBoxText, 64, exitBoxEditMode)) {
                return 0;
            }
        } break;
        case ESC_SCENE: {
            char resumeBoxText[64] = "Resume (SPACE)";
            bool resumeBoxEditMode = false;
            if (GuiTextBox((Rectangle){ 45, 215, 325, 60 }, resumeBoxText, 64, resumeBoxEditMode)) {
                SetGameScene(MAIN_GAME_SCENE);
            }
            char textBoxText[64] = "Restart (R)";
            bool textBoxEditMode = false;
            if (GuiTextBox((Rectangle){ 45, 315, 325, 60 }, textBoxText, 64, textBoxEditMode)) {
                SetGameScene(MAIN_GAME_SCENE);
                game.Restart();
            }
            char exitBoxText[64] = "Exit";
            bool exitBoxEditMode = false;
            if (GuiTextBox((Rectangle){ 45, 415, 325, 60 }, exitBoxText, 64, exitBoxEditMode)) {
                return 0;
            }
        } break;
        case MAIN_GAME_SCENE: {
            auto expectedsize = GameGui::GetWindowSize(game.GetState());
            if (GetScreenWidth () != expectedsize.first ||
                GetScreenHeight() != expectedsize.second) {
                window.SetSize(expectedsize.first, expectedsize.second);
            }
            GameGui::Draw(game.GetState());
        } break;
        default: break;
        }
        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    return 0;
}
