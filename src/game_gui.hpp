#pragma once
#include "game.hpp"
#include "game_event.hpp"
#include "raylib.h"
#include "raylib-cpp.hpp"

namespace GameGui {

enum GameScene : uint8_t { START_SCENE, MAIN_GAME_SCENE, ESC_SCENE };

GameScene GetGameScene();
void SetGameScene(GameScene newScene);
void Init();

// CookInputEvent just translates kbd/mouse event into GameEvent
std::pair<std::vector<GameEvent>, GameEvent> CookInputEvent(void);

// GuiEvent here means raygui interaction, e.g., button clicked.
GameEvent Draw(raylib::Window& window, const Sokoban& game);
void ProcessGuiEvent(GameEvent guiEvent, Sokoban& game);
std::pair<int,int> GetWindowSize(const Sokoban::State& state);
Sokoban::Pos PixelToPos(Vector2 pos);

}
