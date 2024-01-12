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
        auto [nrow, ncol] = GameGui::PixelToIndex(GetMousePosition());
        bool leftButton  = IsMouseButtonPressed(MOUSE_LEFT_BUTTON) ||
                           IsMouseButtonDown(MOUSE_LEFT_BUTTON);
        if (GetTouchPointCount() == 1) {
            // single touch not recognized as Pressed
            // Draw circle and touch index number
            leftButton = true;
            std::tie(nrow, ncol) = GameGui::PixelToIndex(GetTouchPosition(0));
            // nrow = GameGui::PixelToIndex(GetTouchPosition(0)).first;
            // ncol = GameGui::PixelToIndex(GetTouchPosition(0)).second;
        } else if (GetTouchPointCount()) {
            leftButton = false;
        }
        bool rightButton = IsMouseButtonReleased(MOUSE_RIGHT_BUTTON);

        if (auto key = GetKeyPressed())
            lastKeyPressed = key;

        if (GetGameScene() != MAIN_GAME_SCENE) {
            if (lastKeyPressed == KEY_SPACE || lastKeyPressed == KEY_ENTER) {
                SetGameScene(MAIN_GAME_SCENE);
            } else if (GameConfig::IsRestart(lastKeyPressed)) {
                game.Restart();
                SetGameScene(MAIN_GAME_SCENE);
            }
        } else if (leftButton) {
            game.Click(nrow, ncol);
        } else if (rightButton) {
            game.Regret();
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
 
        //----------------------------------------------------------------------------------
        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();
        window.ClearBackground(LIGHTGRAY);
        switch(GetGameScene()) {
        case START_SCENE: {
            // TODO: de-duplicate these codes.
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
