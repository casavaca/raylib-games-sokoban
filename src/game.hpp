#pragma once

#include <stack>
#include <unordered_set>
#include <vector>
#include <string>
#include <cstdint>

#include "game_event.hpp"

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

static constexpr const char* ALLOWED_CHARACTERS = " #$@*._+";

class Sokoban {
public:

    struct Pos {
        int row,col;
        bool operator==(const Pos& b) const noexcept { return row == b.row && col == b.col;  }
        Pos operator+(const Pos& rhs) const noexcept { return Pos{row+rhs.row, col+rhs.col}; }
        Pos operator-(const Pos& rhs) const noexcept { return Pos{row-rhs.row, col-rhs.col}; }
        Pos operator-()               const noexcept { return Pos{-row, -col}; }
    };
    struct PosHash {
        int64_t operator()(const Pos& p) const noexcept {
            return std::hash<int64_t>()((1LL<<32) * p.row + p.col);
        }
    };
    struct Level {
        std::string              name;
        std::vector<std::string> lines;
    };

    using State = std::vector<std::vector<TileType>>;
public:
    bool LoadLevels(const char* levelFile);
    bool LoadLevel(const Level& lines);
    void LoadDefaultLevels();
    int  LoadLevelsFromTxt();
    const State& GetState() const { return state; }
    void ProcessEvent(const std::vector<GameEvent>& events, const Pos& pos);
    void Restart  (){ LoadLevel(levels[curLevel]); }
    void PushNorth(){ Push(-1, 0); }
    void PushSouth(){ Push( 1, 0); }
    void PushEast (){ Push( 0, 1); }
    void PushWest (){ Push( 0,-1); }
    void Push(int dy, int dx);
    void Click(Pos pos);
    void Regret();
    bool IsLastLevel() const { return curLevel == levels.size() - 1; }
    void NextLevel() { curLevel++; Restart(); }
    bool LevelCompleted() const { return numBoxes == numBoxesOnTarget; }
private:
    bool LoadLevelFromFile(const char* levelFile);
    void Pull(Pos LastPlayerPos, Pos dp);
    const TileType& Get(Pos p) const {return state[p.row][p.col]; }
    TileType& Get(Pos p) {return state[p.row][p.col]; }
    void MoveBox(Pos p, Pos dp);
    bool IsBlocked(Pos p) const {return InBound(p) && (Get(p) & TILE_BLOCKED);     }
    bool IsBox    (Pos p) const {return InBound(p) && (Get(p) & TILE_BOX);         }
    bool IsSpace  (Pos p) const {return InBound(p) && !(Get(p) & TILE_SPACE_MASK); }
    bool InBound(Pos p) const {return InBound(p.row, p.col); }
    bool InBound(int row, int col) const { return (row >= 0 && row < state.size() && col >= 0 && col < state[0].size()); }
    bool Accessible(Pos s, Pos t);
    void ClearPlayerPos();
    void SetPlayerPos(Pos p, int dy, int dx);
    void Clear() {numBoxes = numBoxesOnTarget = 0; history={}; accessCache.clear();}
private:
    Pos   playerPos;
    State state;
    int32_t numBoxes;         // set on LoadLevel and never changes.
    int32_t numBoxesOnTarget; // updated on LoadLevel and MoveBox
    std::vector<Level> levels;
    int curLevel;
    // After each push, history contains new player Pos and dp
    std::stack<std::pair<Pos,Pos>> history;
    std::unordered_set<Pos, PosHash> accessCache;
};
