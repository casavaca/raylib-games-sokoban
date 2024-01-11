#include "game_gui.hpp"
#include "raylib-cpp.hpp"

#include <unordered_map>
#include <cstdint>

using namespace std;

unordered_map<uint8_t, raylib::Texture> g_basePng;
unordered_map<uint8_t, std::vector<raylib::Texture*>> g_pngs;

namespace GameGui {
void Init() {
    g_basePng[TILE_NULL            ] = raylib::Texture("../assets/kenney_sokoban-pack/PNG/Retina/Ground/ground_03.png");
    g_basePng[TILE_WALL            ] = raylib::Texture("../assets/kenney_sokoban-pack/PNG/Retina/Blocks/block_08.png");
    g_basePng[TILE_SPACE           ] = raylib::Texture("../assets/kenney_sokoban-pack/PNG/Retina/Ground/ground_04.png");
    g_basePng[TILE_TARGET          ] = raylib::Texture("../assets/kenney_sokoban-pack/PNG/Retina/Environment/environment_12.png");
    g_basePng[TILE_BOX             ] = raylib::Texture("../assets/kenney_sokoban-pack/PNG/Retina/Crates/crate_42.png");
    g_basePng[TILE_BOX_ON_TARGET   ] = raylib::Texture("../assets/kenney_sokoban-pack/PNG/Retina/Crates/crate_45.png");
    g_basePng[TILE_PLAYER_N        ] = raylib::Texture("../assets/kenney_sokoban-pack/PNG/Retina/Player/player_06.png");
    g_basePng[TILE_PLAYER_E        ] = raylib::Texture("../assets/kenney_sokoban-pack/PNG/Retina/Player/player_20.png");
    g_basePng[TILE_PLAYER_S        ] = raylib::Texture("../assets/kenney_sokoban-pack/PNG/Retina/Player/player_03.png");
    g_basePng[TILE_PLAYER_W        ] = raylib::Texture("../assets/kenney_sokoban-pack/PNG/Retina/Player/player_17.png");

    g_pngs[TILE_NULL               ] = {&g_basePng[TILE_NULL]};
    g_pngs[TILE_WALL               ] = {&g_basePng[TILE_NULL], &g_basePng[TILE_WALL]};
    g_pngs[TILE_SPACE              ] = {&g_basePng[TILE_SPACE]};
    g_pngs[TILE_TARGET             ] = {&g_basePng[TILE_SPACE], &g_basePng[TILE_TARGET]};
    g_pngs[TILE_BOX                ] = {&g_basePng[TILE_SPACE], &g_basePng[TILE_BOX]};
    g_pngs[TILE_BOX_ON_TARGET      ] = {&g_basePng[TILE_SPACE], &g_basePng[TILE_BOX_ON_TARGET]};
    g_pngs[TILE_PLAYER_N           ] = {&g_basePng[TILE_SPACE], &g_basePng[TILE_PLAYER_N]};
    g_pngs[TILE_PLAYER_E           ] = {&g_basePng[TILE_SPACE], &g_basePng[TILE_PLAYER_E]};
    g_pngs[TILE_PLAYER_S           ] = {&g_basePng[TILE_SPACE], &g_basePng[TILE_PLAYER_S]};
    g_pngs[TILE_PLAYER_W           ] = {&g_basePng[TILE_SPACE], &g_basePng[TILE_PLAYER_W]};
    g_pngs[TILE_PLAYER_N_ON_TARGET ] = {&g_basePng[TILE_SPACE], &g_basePng[TILE_PLAYER_N]};
    g_pngs[TILE_PLAYER_E_ON_TARGET ] = {&g_basePng[TILE_SPACE], &g_basePng[TILE_PLAYER_E]};
    g_pngs[TILE_PLAYER_S_ON_TARGET ] = {&g_basePng[TILE_SPACE], &g_basePng[TILE_PLAYER_S]};
    g_pngs[TILE_PLAYER_W_ON_TARGET ] = {&g_basePng[TILE_SPACE], &g_basePng[TILE_PLAYER_W]};
}

void Draw(const Sokoban::State& state) {
    const int blockPixels = g_basePng[TILE_NULL].GetWidth();
    for (int i=0; i<state.size(); i++) {
        for (int j=0; j<state[i].size(); j++) {
            auto c = state[i][j];
            for (auto png:g_pngs[c]) {
                png->Draw(j*blockPixels, i*blockPixels);
            }
        }
    }
}

static int GetBlockPixels() {
    assert(g_basePng.at(TILE_NULL).GetWidth() == g_basePng.at(TILE_NULL).GetHeight());
    return g_basePng.at(TILE_NULL).GetWidth();
}

std::pair<int,int> GetWindowSize(const Sokoban::State& state) {
    return {state.size() * GetBlockPixels(), state[0].size() * GetBlockPixels()};
}

std::pair<int,int> PixelToIndex(Vector2 pos) {
    int nrow = static_cast<int>(pos.y) / GetBlockPixels();
    int ncol = static_cast<int>(pos.x) / GetBlockPixels();
    return {nrow, ncol};
}

}
