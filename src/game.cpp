#include "game.hpp"

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
