#include "game.hpp"

#include <algorithm>
#include <sstream>

static const char* defaultLevelStr = 
        "_####__\n"
        "_# .#__\n"
        "_#  ###\n"
        "_#*@  #\n"
        "##  $ #\n"
        "#   ###\n"
        "#####__\n";

static std::vector<std::string> Split(const std::string& target) {
    std::string temp;
    std::stringstream stringstream { target };
    std::vector<std::string> result;
    while (std::getline(stringstream, temp)) {
        result.push_back(temp);
    }
    return result;
}

static Sokoban::State ConvertFromTxt(const std::vector<std::string>& lines) {
    // simple validation.
    static std::string allowed = "#$@*._ ";

    if(!lines.size()) return {};
    auto nCol = std::max_element(lines.begin(), lines.end(),
                                 [](auto a,auto b){return a.size()<b.size();})->size();
    for (auto &s:lines) {
        if (s.find_first_of("#") == s.npos) return {};
        if (s.size() != nCol) return {};
        if (s.find_first_not_of(allowed) != s.npos) return {};
    }

    Sokoban::State ret;
    for (auto s:lines) {
        ret.emplace_back();
        std::transform(s.begin(), s.end(), std::back_inserter(ret.back()),
                       [](char c){return Sokoban::txtMap[c];});
    }
    return ret;
}

bool Sokoban::LoadDefaultLevel() {
    return Sokoban::LoadLevel(Split(defaultLevelStr));
}

bool Sokoban::LoadLevel(const std::vector<std::string>& lines) {
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

Sokoban::Pos operator+(Sokoban::Pos a, Sokoban::Pos b) { return {a.row+b.row, a.col+b.col}; }
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
        if (IsSpace(tmp)) {
            Get(tmp)    |= TILE_BOX;
            Get(newPos) &= ~TILE_BOX;
        }
    } // FALLTHROUGH
    case TILE_SPACE:
    case TILE_TARGET:
        Get(playerPos) &= ~TILE_PLAYER;
        Get(playerPos) &= ~3;
        Get(newPos)    |= TILE_PLAYER;
        Get(newPos)    &= ~3;
        Get(newPos)    |= (dy ? (1-dy) : (2+dx));
        playerPos = newPos;
    default:  return;
    }
}
