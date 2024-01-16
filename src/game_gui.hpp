#pragma once
#include "game.hpp"
#include "game_event.hpp"
#include "raylib.h"
#include "raylib-cpp.hpp"

#include <unordered_map>
#include <cstdint>

namespace GameGui {

enum GameScene : uint8_t {
    START_SCENE,
    MAIN_GAME_SCENE,
    ESC_SCENE,
    LEVEL_FINISHED_SCENE,
};

struct GameResources {
    std::unordered_map<uint8_t, raylib::Texture> textures;
};

GameScene GetGameScene();
void SetGameScene(GameScene newScene);
void Init(GameResources* resourcePtr);

// CookInputEvent just translates kbd/mouse event into GameEvent
std::pair<std::vector<GameEvent>, GuiEvent> CookInputEvent(const Sokoban& game);

// GuiEvent here means raygui interaction, e.g., button clicked.
GuiEvent Draw(raylib::Window& window, const Sokoban& game);
bool ProcessGuiEvent(GuiEvent guiEvent, Sokoban& game);
std::pair<int,int> GetWindowSize(const Sokoban::State& state);
Sokoban::Pos PixelToPos(Vector2 pos);

}
