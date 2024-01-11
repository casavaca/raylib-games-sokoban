#pragma once
#include "game.hpp"
#include "raylib.h"

namespace GameGui {

enum GameScene : uint8_t { START_SCENE, MAIN_GAME_SCENE, ESC_SCENE };

GameScene GetGameScene();
void SetGameScene(GameScene newScene);
void Init();
void Draw(const Sokoban::State& state);
std::pair<int,int> GetWindowSize(const Sokoban::State& state);
std::pair<int,int> PixelToIndex(Vector2 pos);

}
