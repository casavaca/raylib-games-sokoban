#include "game.hpp"

#include <algorithm>
#include <array>
#include <queue>

#include <cassert>

using namespace std;

// static data.
static constexpr TileType TxtMap(char c) {
    switch (c) {
    case ' ': return TILE_SPACE;
    case '#': return TILE_WALL;
    case '$': return TILE_BOX;
    case '@': return TILE_PLAYER;
    case '*': return TILE_BOX_ON_TARGET;
    case '.': return TILE_TARGET;
    case '_': return TILE_NULL;
    default:
        Unreachable();
    }
}

bool Sokoban::LoadLevel(const Level& level) {
    Clear();
    state = vector<vector<TileType>>(level.lines.size());
    std::transform(level.lines.begin(), level.lines.end(), state.begin(), [](const string& s)->vector<TileType>{
            // one liner in C++23 but we are just using C++17...
            assert(s.find_first_not_of(" #$@*._") == s.npos);
            vector<TileType> ret(s.size());
            std::transform(s.begin(), s.end(), ret.begin(), TxtMap);
            return ret;
    });
    for (int i=0; i<state.size(); i++) {
        for (int j=0; j<state[i].size(); j++) {
            if (state[i][j] & TILE_PLAYER) {
                playerPos = Pos{j, i};
            }
            if (state[i][j] & TILE_BOX) {
                numBoxes++;
                numBoxesOnTarget += (state[i][j] & TILE_TARGET)?1:0;
            }
        }
    }
    return true;
}

void Sokoban::LoadDefaultLevels() {
    static const std::array defaultLevel0 {
        "######",
        "#@$ .#",
        "######",
    };
    static const std::array defaultLevel1 {
        "_####__",
        "_# .#__",
        "_#  ###",
        "_#*@  #",
        "##  $ #",
        "#   ###",
        "#####__",
    };

    levels   = {{"debug level"  , vector<string>(defaultLevel0.begin(), defaultLevel0.end())},
                {"Default Level", vector<string>(defaultLevel1.begin(), defaultLevel1.end())},
                };
    curLevel = 0;
    LoadLevel(levels[curLevel]);
}

// operators
static inline Sokoban::Pos operator+ (Sokoban::Pos a, Sokoban::Pos b)     { return {a.row+b.row, a.col+b.col}; }
static inline Sokoban::Pos operator- (Sokoban::Pos a, Sokoban::Pos b)     { return {a.row-b.row, a.col-b.col}; }
static inline Sokoban::Pos operator- (Sokoban::Pos a)                     { return {-a.row,     -a.col      }; }

static inline TileType&    operator|=(TileType& lhs, const TileType& rhs) { return lhs = static_cast<TileType>(lhs | rhs); }
static inline TileType&    operator|=(TileType& lhs, int rhs)             { return lhs = static_cast<TileType>(lhs | rhs); }
static inline TileType&    operator&=(TileType& lhs, int rhs)             { return lhs = static_cast<TileType>(lhs & rhs); }

// .--------> x
// |
// |
// |
// v
// y

// unconditionally move box at p to p+dp.
void Sokoban::MoveBox(Pos p, Pos dp) {
    Pos newPos = p + dp;
    assert(Get(p) & TILE_BOX);
    assert(IsSpace(newPos));
    Get(newPos) |= TILE_BOX;
    Get(p)      &= ~TILE_BOX;
    if ((Get(p) & TILE_TARGET) != (Get(newPos) & TILE_TARGET)) {
        numBoxesOnTarget += (Get(newPos) & TILE_TARGET) ? 1 : -1;
        assert(numBoxesOnTarget >= 0);
    }
    accessCache.clear();
}

void Sokoban::Push(int dy, int dx) {
    Pos dp = {dy, dx};
    Pos newPos = playerPos + dp;
    if (!InBound(newPos))
        return;
    if (state[newPos.row][newPos.col] & TILE_BOX) {
        if (!IsSpace(newPos + dp))
            return SetPlayerPos(playerPos, dy,dx);
        history.push({newPos, dp});
        MoveBox(newPos, dp);
    }
    if (IsSpace(newPos)) {
        ClearPlayerPos();
        SetPlayerPos(newPos, dy, dx);
    } else {
        SetPlayerPos(playerPos, dy, dx);
    }
}

void Sokoban::Pull(Pos lastPlayerPos, Pos dp) {
    Pos newPos = lastPlayerPos + dp;
    Pos boxPos = lastPlayerPos - dp;
    assert(InBound(newPos) && IsSpace(newPos));
    assert(InBound(boxPos) && IsBox(boxPos));
    ClearPlayerPos();
    SetPlayerPos(newPos, -dp.row, -dp.col);
    MoveBox(boxPos, dp);
}

void Sokoban::Regret() {
    if (history.empty())
        return;
    auto [lastPlayerPos, dp] = history.top();
    history.pop();
    Pull(lastPlayerPos, -dp);
}

void Sokoban::SetPlayerPos(Pos p, int dy, int dx) {
    assert(InBound(p));
    playerPos = p;
    Get(playerPos) |= TILE_PLAYER;
    Get(playerPos) &= ~3;
    Get(playerPos) |= (dy ? (1-dy) : (2+dx));
}
void Sokoban::ClearPlayerPos() {
    Get(playerPos) &= (~(TILE_PLAYER|3));
}

void Sokoban::Click(Pos pos) {
    auto dis = abs(pos.row - playerPos.row) + abs(pos.col - playerPos.col);
    if (dis == 0) {
        return;
    }
    if (dis == 1) {
        return Push(pos.row - playerPos.row, pos.col - playerPos.col);
    }
    if (!IsSpace(pos))
        return;
    if (Accessible(playerPos, pos)) {
        ClearPlayerPos();
        SetPlayerPos(pos, 1, 0);
    }
}
bool Sokoban::Accessible(Pos s, Pos t) {
    if (accessCache.size()) {
        return accessCache.count(t);
    }
    // BFS
    vector<vector<bool>> visited(state.size(), vector<bool>(state[0].size()));
    visited[s.row][s.col] = true;
    accessCache.insert(s);
    queue<Pos> q;
    q.push(s);
    while (q.size()) {
        auto n = q.front();
        q.pop();
        std::array<Pos,4> dps = {Pos{-1,0},{1,0},{0,-1},{0,1}};
        for (auto dp:dps) {
            auto next = n+dp;
            if (!InBound(next) || IsBox(next) || IsBlocked(next) || visited[next.row][next.col])
                continue;
            visited[next.row][next.col] = true;
            accessCache.insert(next);
            q.push(next);
        }
    }
    return accessCache.count(t);
}

void Sokoban::ProcessEvent(const std::vector<GameEvent>& events,
                           const Sokoban::Pos& pos) {
    for (auto e:events) switch (e) {
        case GameEvent::EVENT_MOVE_UP:      { PushNorth(); } break;
        case GameEvent::EVENT_MOVE_DOWN:    { PushSouth(); } break;
        case GameEvent::EVENT_MOVE_LEFT:    { PushWest();  } break;
        case GameEvent::EVENT_MOVE_RIGHT:   { PushEast();  } break;
        case GameEvent::EVENT_MOVE_RESTART: { Restart();   } break;
        case GameEvent::EVENT_MOVE_REGRET:  { Regret();    } break;
        case GameEvent::EVENT_MOVE_CLICK:   { Click(pos);  } break; // + row, col
    }
}

