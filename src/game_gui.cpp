#include "game_gui.hpp"
#include "raylib-cpp.hpp"

#include <unordered_map>
#include <cstdint>

using namespace std;

unordered_map<uint8_t, raylib::Texture> g_basePng;
unordered_map<uint8_t, std::vector<raylib::Texture*>> g_pngs;

namespace GameGui {

static GameScene scene = START_SCENE;

GameScene GetGameScene() {
    return scene;
}
void SetGameScene(GameScene newScene){
    scene = newScene;
}

void Init() {
#if defined(PLATFORM_WEB)
    std::string prefix = "assets/kenney_sokoban-pack/PNG/Default size";
#else
    std::string prefix = "assets/kenney_sokoban-pack/PNG/Retina";
#endif

    g_basePng[TILE_NULL            ] = raylib::Texture(prefix + "/Ground/ground_03.png");
    g_basePng[TILE_WALL            ] = raylib::Texture(prefix + "/Blocks/block_08.png");
    g_basePng[TILE_SPACE           ] = raylib::Texture(prefix + "/Ground/ground_04.png");
    g_basePng[TILE_TARGET          ] = raylib::Texture(prefix + "/Environment/environment_12.png");
    g_basePng[TILE_BOX             ] = raylib::Texture(prefix + "/Crates/crate_42.png");
    g_basePng[TILE_BOX_ON_TARGET   ] = raylib::Texture(prefix + "/Crates/crate_45.png");
    g_basePng[TILE_PLAYER_N        ] = raylib::Texture(prefix + "/Player/player_06.png");
    g_basePng[TILE_PLAYER_E        ] = raylib::Texture(prefix + "/Player/player_20.png");
    g_basePng[TILE_PLAYER_S        ] = raylib::Texture(prefix + "/Player/player_03.png");
    g_basePng[TILE_PLAYER_W        ] = raylib::Texture(prefix + "/Player/player_17.png");

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
#if defined(DEBUG)
    assert(g_basePng.at(TILE_NULL).GetWidth() == g_basePng.at(TILE_NULL).GetHeight());
#endif
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
