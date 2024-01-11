#pragma once

#include <array>
#include <vector>
#include <string>
#include <cstdint>

#if defined(DEBUG)
#include <cassert>
#endif

// Types
enum TileType : uint8_t {
    // [ 0 0 0 0 0 0  0 0  ]
    // [     | | | |  |_|  ]
    //       | | | |    `----- subtype
    //       | | | `---------- IsPlayer (00: S, 01: E, 02: N, 03: W)
    //       | | `------------ IsBlocked(01: outside, 10: wall)
    //       |  `------------- IsTarget
    //        `--------------- IsBox

    TILE_PLAYER           = 0x4,
    TILE_PLAYER_S         = 0x4,
    TILE_PLAYER_E         = 0x5,
    TILE_PLAYER_N         = 0x6,
    TILE_PLAYER_W         = 0x7,

    TILE_BLOCKED          = 0x8,
    TILE_NULL             = 0x9,  // outside boundary.
    TILE_WALL             = 0xa,

    TILE_BOX              = 0x20,
    TILE_SPACE            = 0x0,
    TILE_SPACE_MASK       = 0x28,

    TILE_TARGET           = 0x10,

    TILE_BOX_ON_TARGET      = (TILE_TARGET | TILE_BOX),
    TILE_PLAYER_ON_TARGET   = (TILE_TARGET | TILE_PLAYER),
    TILE_PLAYER_S_ON_TARGET = (TILE_TARGET | TILE_PLAYER_S),
    TILE_PLAYER_E_ON_TARGET = (TILE_TARGET | TILE_PLAYER_E),
    TILE_PLAYER_N_ON_TARGET = (TILE_TARGET | TILE_PLAYER_N),
    TILE_PLAYER_W_ON_TARGET = (TILE_TARGET | TILE_PLAYER_W),
};

class Sokoban {
public:

    struct Pos {int row,col;};

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
    bool LoadLevel(const std::vector<std::string>& lines);
    bool LoadDefaultLevel();
    const State& GetState() const { return state; }
    void Restart  (){ LoadLevel(level); }
    void PushNorth(){ Push(-1, 0); }
    void PushSouth(){ Push( 1, 0); }
    void PushEast (){ Push( 0, 1); }
    void PushWest (){ Push( 0,-1); }
    void Push(int dy, int dx);
    void Click(int nrow, int ncol) {
        Teleport(nrow, ncol);
    }
private:
    const TileType& Get(Pos p) const {return state[p.row][p.col]; }
    TileType& Get(Pos p) {return state[p.row][p.col]; }
    int  GetSubtype(Pos p) const {return state[p.row][p.col] & 3; }
    bool IsBlocked(Pos p) const {return InBound(p) && (Get(p) & TILE_BLOCKED);     }
    bool IsBox    (Pos p) const {return InBound(p) && (Get(p) & TILE_WALL);        }
    bool IsSpace  (Pos p) const {return InBound(p) && !(Get(p) & TILE_SPACE_MASK); }
    bool InBound(Pos p) const {return InBound(p.row, p.col); }
    bool InBound(int row, int col) const {
        int M = state.size();
        int N = state[0].size();
        return (row >= 0 && row < M && col >= 0 && col < N);
    }
    void ClearPlayerPos();
    void SetPlayerPos(Pos p, int dy, int dx);
private:
    Pos   playerPos;
    State state;
    std::vector<std::string> level;
    // helper functions.
    void Teleport(Pos p) { return Teleport(p.row, p.col); }
    void Teleport(int row, int col) {
        if (InBound(row, col)) {
            state[row][col] = TILE_PLAYER;
            state[playerPos.row][playerPos.col] = TILE_SPACE;
            playerPos = Pos{row, col};
        }
    }
};
