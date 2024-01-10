#pragma once

#include <vector>
#include <algorithm>
#include <iterator>
#include <string>

#if defined(DEBUG)
#include <cassert>
#endif

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

class Sokoban {
public:

    struct Pos { int x,y; };

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
        for (int i=0; i<state.size(); i++) {
            for (int j=0; j<state[i].size(); j++) {
                if(state[i][j] & TILE_PLAYER) {
                    playerPos = Pos{j, i};
                }
            }
        }
    }
    const State& GetState() const { return state; }
    void PushNorth() {}
    void PushSouth() {}
    void PushEast () {}
    void PushWest () {}
    void Click(int nrow, int ncol) {
        Teleport(nrow, ncol);
    }
private:
    Pos   playerPos;
    State state;

    // helper functions.
    void Teleport(int nrow, int ncol) {
        int M = state.size();
        int N = state[0].size();
        int prow = playerPos.y;
        int pcol = playerPos.x;
        if (nrow>=0 && nrow < M && ncol >=0 && ncol < N) {
            state.at(nrow).at(ncol) = TILE_PLAYER;
            state.at(prow).at(pcol) = TILE_SPACE;
            playerPos.y = nrow;
            playerPos.x = ncol;
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
