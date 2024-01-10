#pragma once
#include "game.hpp"
#include "raylib.h"

namespace GameGui {

void Init();
void Draw(const Sokoban::State& state);
std::pair<int,int> GetWindowSize(const Sokoban::State& state);
std::pair<int,int> PixelToIndex(Vector2 pos);

}
