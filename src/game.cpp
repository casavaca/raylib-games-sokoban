#include "game.hpp"

#include <algorithm>
#include <functional>
#include <queue>
#include <sstream>

using namespace std;

static const char* defaultLevelStr = 
        "_####__\n"
        "_# .#__\n"
        "_#  ###\n"
        "_#*@  #\n"
        "##  $ #\n"
        "#   ###\n"
        "#####__\n";

static vector<string> Split(const string& target) {
    string temp;
    stringstream stringstream { target };
    vector<string> result;
    while (getline(stringstream, temp)) {
        result.push_back(temp);
    }
    return result;
}

static Sokoban::State ConvertFromTxt(const vector<string>& lines) {
    // simple validation.
    static string allowed = "#$@*._ ";

    if(!lines.size()) return {};
    auto nCol = max_element(lines.begin(), lines.end(),
                                 [](auto a,auto b){return a.size()<b.size();})->size();
    for (auto &s:lines) {
        if (s.find_first_of("#") == s.npos) return {};
        if (s.size() != nCol) return {};
        if (s.find_first_not_of(allowed) != s.npos) return {};
    }

    Sokoban::State ret;
    for (auto s:lines) {
        ret.emplace_back();
        transform(s.begin(), s.end(), back_inserter(ret.back()),
                       [](char c){return Sokoban::txtMap[c];});
    }
    return ret;
}

bool Sokoban::LoadDefaultLevel() {
    return Sokoban::LoadLevel(Split(defaultLevelStr));
}

bool Sokoban::LoadLevel(const vector<string>& lines) {
    state = ConvertFromTxt(lines);
    if (state.empty()) {
        return false;
    }
    level = lines;
    for (int i=0; i<state.size(); i++) {
        for (int j=0; j<state[i].size(); j++) {
            if(state[i][j] & TILE_PLAYER) {
                playerPos = Pos{j, i};
            }
        }
    }
    return true;
}

// operators
Sokoban::Pos operator+(Sokoban::Pos a, Sokoban::Pos b) { return {a.row+b.row, a.col+b.col}; }
Sokoban::Pos operator-(Sokoban::Pos a, Sokoban::Pos b) { return {a.row-b.row, a.col-b.col}; }
TileType& operator|=(TileType& lhs, const TileType& rhs) { return lhs = static_cast<TileType>(lhs | rhs); }
TileType& operator|=(TileType& lhs, int rhs)             { return lhs = static_cast<TileType>(lhs | rhs); }
TileType& operator&=(TileType& lhs, const TileType& rhs) { return lhs = static_cast<TileType>(lhs & rhs); }
TileType& operator&=(TileType& lhs, int rhs)             { return lhs = static_cast<TileType>(lhs & rhs); }

// .--------> x
// |
// |
// |
// v
// y
void Sokoban::Push(int dy, int dx) {
    Pos dp = {dy, dx};
    Pos newPos = playerPos + dp;
    if (!InBound(newPos))
        return;
    switch (state[newPos.row][newPos.col]) {
    case TILE_BOX:
    case TILE_BOX_ON_TARGET: {
        Pos tmp = newPos + dp;
        if (!IsSpace(tmp))
            return SetPlayerPos(playerPos, dy,dx);
        history.push({newPos, Pos{-dy,-dx}});
        Get(tmp)    |= TILE_BOX;
        Get(newPos) &= ~TILE_BOX;
        accessCache.clear();
    } // FALLTHROUGH
    case TILE_SPACE:
    case TILE_TARGET:
        ClearPlayerPos();
        SetPlayerPos(newPos, dy, dx);
    default:
        SetPlayerPos(playerPos, dy,dx);
    }
}

void Sokoban::Pull(Pos lastPlayerPos, Pos dp) {
    Pos newPos = lastPlayerPos + dp;
    Pos boxPos = lastPlayerPos - dp;
#if defined(DEBUG)
    assert(InBound(newPos) && IsSpace(newPos));
    assert(InBound(boxPos) && IsBox(boxPos));
#endif
    Get(boxPos) &= ~TILE_BOX;
    Get(lastPlayerPos) |=  TILE_BOX;
    ClearPlayerPos();
    SetPlayerPos(newPos, -dp.row, -dp.col);
}

void Sokoban::Regret() {
    if (history.empty())
        return;
    auto [lastPlayerPos, dp] = history.top();
    history.pop();
    Pull(lastPlayerPos, dp);
}

void Sokoban::SetPlayerPos(Pos p, int dy, int dx) {
#if defined(DEBUG)
    assert(InBound(p));
#endif
    playerPos = p;
    Get(playerPos) |= TILE_PLAYER;
    Get(playerPos) &= ~3;
    Get(playerPos) |= (dy ? (1-dy) : (2+dx));
}
void Sokoban::ClearPlayerPos() {
    Get(playerPos) &= (~(TILE_PLAYER|3));
}

void Sokoban::Click(int nrow, int ncol) {
    if (abs(nrow - playerPos.row) + abs(ncol - playerPos.col) == 1) {
        return Push(nrow - playerPos.row, ncol - playerPos.col);
    }
    Pos newPos{nrow, ncol};
    if (!IsSpace(newPos))
        return;
    if (Accessible(playerPos, newPos)) {
        ClearPlayerPos();
        SetPlayerPos(newPos, 1, 0);
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
    while(q.size()) {
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

