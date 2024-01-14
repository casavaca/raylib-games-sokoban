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
GameEvent Draw(raylib::Window& window, const Sokoban& game);
void ProcessEvent(raylib::Window& window, Sokoban& game, GameEvent);
std::pair<int,int> GetWindowSize(const Sokoban::State& state);
std::pair<int,int> PixelToIndex(Vector2 pos);

}
