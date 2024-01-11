#include "game_config.hpp"

#include <algorithm>

namespace GameConfig {

enum { Up, Down, Right, Left, Restart, Regret, NUM_BINDINGS };

static Binding bindings[NUM_BINDINGS] {
    [Up     ] = {KEY_UP   , KEY_I, KEY_W, KEY_NULL},
    [Down   ] = {KEY_DOWN , KEY_K, KEY_S, KEY_NULL},
    [Right  ] = {KEY_RIGHT, KEY_L, KEY_D, KEY_NULL},
    [Left   ] = {KEY_LEFT , KEY_J, KEY_A, KEY_NULL},
    [Regret ] = {KEY_Z},
    [Restart] = {KEY_R},
};

static bool Contain(Binding b, int key) {
    return std::find(b.begin(), b.end(), key) != b.end();
}

bool IsUp     (int key) { return key != KEY_NULL && Contain(bindings[Up],      key); }
bool IsDown   (int key) { return key != KEY_NULL && Contain(bindings[Down],    key); }
bool IsRight  (int key) { return key != KEY_NULL && Contain(bindings[Right],   key); }
bool IsLeft   (int key) { return key != KEY_NULL && Contain(bindings[Left],    key); }
bool IsRestart(int key) { return key != KEY_NULL && Contain(bindings[Restart], key); }
bool IsRegret (int key) { return key != KEY_NULL && Contain(bindings[Regret],  key); }

}
