#pragma once

#include <array>
#include "raylib.hpp"

using Binding = std::array<int, 4>;

namespace GameConfig {

bool IsUp     (int key);
bool IsDown   (int key);
bool IsRight  (int key);
bool IsLeft   (int key);
bool IsRestart(int key);
bool IsRegret (int key);

}
