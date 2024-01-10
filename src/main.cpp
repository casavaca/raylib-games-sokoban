#include <algorithm>
#include <iostream>
#include <iterator>
#include <stdexcept>
#include <vector>
#include <unordered_map>
#include <cstdint>
#include <cassert>

#include "raylib-cpp.hpp"
#include "raylib.h"

class Sokoban {
    // Types
    enum TileType {
        TILE_NULL             = 0x0,  // outside boundary.
        TILE_WALL             = 0x1,
        TILE_SPACE            = 0x2,
        TILE_BOX              = 0x3,
        TILE_PLAYER_S         = 0x4,
        TILE_PLAYER_E         = 0x5,
        TILE_PLAYER_N         = 0x6,
        TILE_PLAYER_W         = 0x7,
        TILE_TARGET           = 0x8,
        // bitwise or tricks
        TILE_PLAYER           = 0x4,
        TILE_BOX_ON_TARGET    = (TILE_TARGET | TILE_BOX),
        TILE_MAX              = 0x9,
    };

    using State = std::vector<std::vector<TileType>>;

    // static data.
    static constexpr TileType txtMap[] = {
        [' '] = TILE_SPACE,
        ['#'] = TILE_WALL,
        ['$'] = TILE_BOX,
        ['@'] = TILE_PLAYER,
        ['*'] = TILE_BOX_ON_TARGET,
        ['.'] = TILE_TARGET,
        ['_'] = TILE_NULL,
    };

public:
    void LoadDefaultLevel() {
        state = convertFromTxt({
                "_####__",
                "_# .#__",
                "_#  ###",
                "_#*@  #",
                "##  $ #",
                "#   ###",
                "#####__",
            });
        for (auto i=0; const auto & vc : state) {
            for (auto j=0; const auto c : vc) {
                if(c & TILE_PLAYER) {
                    playerPos.y = i;
                    playerPos.x = j;
                }
                j++;
            }
            i++;
        }
        std::cout << playerPos.y << std::endl;
        std::cout << playerPos.x << std::endl;
    }
    void LoadAsset() {

        basePng[TILE_NULL          ] = raylib::Texture("../assets/kenney_sokoban-pack/PNG/Retina/Ground/ground_03.png");
        basePng[TILE_WALL          ] = raylib::Texture("../assets/kenney_sokoban-pack/PNG/Retina/Blocks/block_08.png");
        basePng[TILE_SPACE         ] = raylib::Texture("../assets/kenney_sokoban-pack/PNG/Retina/Ground/ground_04.png");
        basePng[TILE_TARGET        ] = raylib::Texture("../assets/kenney_sokoban-pack/PNG/Retina/Environment/environment_12.png");
        basePng[TILE_BOX           ] = raylib::Texture("../assets/kenney_sokoban-pack/PNG/Retina/Crates/crate_42.png");
        basePng[TILE_BOX_ON_TARGET ] = raylib::Texture("../assets/kenney_sokoban-pack/PNG/Retina/Crates/crate_45.png");
        basePng[TILE_PLAYER_N      ] = raylib::Texture("../assets/kenney_sokoban-pack/PNG/Retina/Player/player_06.png");
        basePng[TILE_PLAYER_E      ] = raylib::Texture("../assets/kenney_sokoban-pack/PNG/Retina/Player/player_17.png");
        basePng[TILE_PLAYER_S      ] = raylib::Texture("../assets/kenney_sokoban-pack/PNG/Retina/Player/player_03.png");
        basePng[TILE_PLAYER_W      ] = raylib::Texture("../assets/kenney_sokoban-pack/PNG/Retina/Player/player_20.png");

        pngs[TILE_NULL             ] = {&basePng[TILE_NULL]};
        pngs[TILE_WALL             ] = {&basePng[TILE_NULL], &basePng[TILE_WALL]};
        pngs[TILE_SPACE            ] = {&basePng[TILE_SPACE]};
        pngs[TILE_TARGET           ] = {&basePng[TILE_SPACE], &basePng[TILE_TARGET]};
        pngs[TILE_BOX              ] = {&basePng[TILE_SPACE], &basePng[TILE_BOX]};
        pngs[TILE_BOX_ON_TARGET    ] = {&basePng[TILE_SPACE], &basePng[TILE_BOX_ON_TARGET]};
        pngs[TILE_PLAYER_N         ] = {&basePng[TILE_SPACE], &basePng[TILE_PLAYER_N]};
        pngs[TILE_PLAYER_E         ] = {&basePng[TILE_SPACE], &basePng[TILE_PLAYER_E]};
        pngs[TILE_PLAYER_S         ] = {&basePng[TILE_SPACE], &basePng[TILE_PLAYER_S]};
        pngs[TILE_PLAYER_W         ] = {&basePng[TILE_SPACE], &basePng[TILE_PLAYER_W]};
    }
    void Draw() {
        const int blockPixels = basePng[TILE_NULL].GetWidth();
        for (auto i=0; const auto & vc : state) {
            for (auto j=0; const auto c : vc) {
                for (auto png:pngs[c]) {
                    png->Draw(j*blockPixels, i*blockPixels);
                }
                j++;
            }
            i++;
        }
    }
    void HandleKeyPressed(int c) {
        int prow = static_cast<int>(playerPos.y);
        int pcol = static_cast<int>(playerPos.x);
        switch (c) {
        case KEY_UP:    return Teleport(prow-1, pcol);
        case KEY_DOWN:  return Teleport(prow+1, pcol);
        case KEY_LEFT:  return Teleport(prow, pcol-1);
        case KEY_RIGHT: return Teleport(prow, pcol+1);
        }
    }
    void Click(int nrow, int ncol) {
        Teleport(nrow, ncol);
    }
    int GetBlockPixels() const {
        assert(basePng.at(TILE_NULL).GetWidth() == basePng.at(TILE_NULL).GetHeight());
        return basePng.at(TILE_NULL).GetWidth();
    }
    std::pair<int,int> GetDim() const {
        return {state.size(), state[0].size()};
    }
private:
    Vector2 playerPos;
    State state;
    std::unordered_map<uint8_t, raylib::Texture> basePng;
    std::unordered_map<uint8_t, std::vector<raylib::Texture*>> pngs;

    // helper functions.
    void Teleport(int nrow, int ncol) {
        try {
            int prow = static_cast<int>(playerPos.y);
            int pcol = static_cast<int>(playerPos.x);
            state.at(nrow).at(ncol) = TILE_PLAYER;
            state.at(prow).at(pcol) = TILE_SPACE;
            playerPos.y = nrow;
            playerPos.x = ncol;
        } catch (const std::out_of_range& e) {
        }
    }

    State convertFromTxt(const std::vector<std::string>& lines) {
#if defined(DEBUG)
        assert(lines.size());
        auto nCol = std::max_element(lines.begin(), lines.end(), [](auto a,auto b){return a.size()<b.size();})->size();
        for (auto &s:lines) {
            assert(s.find_first_of("#") != s.npos);
            assert(s.size() == nCol);
        }
#endif
        State ret;
        for (auto s:lines) {
            ret.emplace_back();
            std::transform(s.begin(), s.end(), std::back_inserter(ret.back()), [](char c){return txtMap[c];});
        }
        return ret;
    }
};

int main() {

    // Initialization
    //--------------------------------------------------------------------------------------
    raylib::Window window(800, 600, "sokoban");

    Sokoban game;
    game.LoadDefaultLevel();
    game.LoadAsset();

    const int blockPixels = game.GetBlockPixels();
    auto [screenWidth, screenHeight] = game.GetDim();
    screenWidth  *= blockPixels;
    screenHeight *= blockPixels;

    window.SetSize(screenWidth, screenHeight);
    SetTargetFPS(60);               // Set our game to run at 60 frames-per-second
    SetExitKey(KEY_NULL);           // Disable quit-on-ESC
    //---------------------------------------------------------------------------------------

    // Main game loop
    while (!window.ShouldClose()) {    // Detect window close button or ESC key
#if defined(DEBUG)
        if (GetCharPressed() == 'q') {
            break;
        }
#endif

        // Update
        //----------------------------------------------------------------------------------
        auto mousePosition = GetMousePosition();

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) ||
            IsMouseButtonPressed(MOUSE_MIDDLE_BUTTON) ||
            IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) {
            int nrow = static_cast<int>(mousePosition.y) / blockPixels;
            int ncol = static_cast<int>(mousePosition.x) / blockPixels;
            game.Click(nrow, ncol);
        } else if (auto c = GetKeyPressed()) {
            game.HandleKeyPressed(c);
        }

        if (IsKeyPressed(KEY_ENTER)) {
          screenHeight += 10;
          window.SetSize(screenWidth, screenHeight);
        }
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();
        {
            window.ClearBackground(RAYWHITE);
            game.Draw();
        }
        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    return 0;
}
