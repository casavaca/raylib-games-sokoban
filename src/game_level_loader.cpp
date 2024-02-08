#include "game.hpp"

#include <algorithm>
#include <iostream>
#include <fstream>
#include <optional>
#include <string>
#include <queue>
#include <array>

using namespace std;

// https://github.com/nMusacchio/sokoban/blob/master/niveles.txt
static std::optional<Sokoban::Level> LoadOneLevel(const vector<string>& vs) {
    // minimal level example:
    //
    // Level XXX
    // #####
    // #@.$#
    // #####
    //
    // which takes 4 lines.
    if (vs.size() < 4)
        return {};
    Sokoban::Level level;
    if (vs[1].find_first_not_of(ALLOWED_CHARACTERS) != string::npos) {
        level.name  = vs[1];
        level.lines = {vs.begin()+2, vs.end()};
    } else {
        level.name = vs[0];
        level.lines = {vs.begin()+1, vs.end()};
    }
    // validation:
    // step 0: pad all lines to be the same length.
    auto numCol = max_element(level.lines.begin(), level.lines.end(),
                              [](const string& a, const string& b){
                                  return a.size()<b.size();
                              })->size();
    for (auto& line : level.lines) {
        line.resize(numCol, ' ');
    }
    // step 1: ensure @ exists (player position)
    // step 2: ensure #box = #target
    int numPlayer  = 0;
    int playerRow  = 0;
    int playerCol  = 0;
    int numBoxes   = 0;
    int numTargets = 0;
    for (auto & s: level.lines) {
        for (auto& c:s) switch (c) {
            case '+':
                numTargets++;
                [[fallthrough]];
            case '@':
                numPlayer++;
                playerRow = &s-&level.lines[0];
                playerCol = &c-&s[0];
                break;
            case '$': numBoxes++;               break;
            case '.': numTargets++;             break;
            case '*': numBoxes++; numTargets++; break;
            case ' ': break;
            case '#': break;
            default: return {};
        }
    }
    if (numPlayer != 1 || numBoxes != numTargets)
        return {};

    // step 3: ensure wall is closed
    // BFS
    int M = static_cast<int>(level.lines.size());
    int N = static_cast<int>(level.lines[0].size());
    vector<vector<bool>> visited(M, vector<bool>(N));

    auto InBound = [M,N](const Sokoban::Pos& p)->bool {
        return p.row >= 0 && p.row < M && p.col >= 0 && p.col < N;
    };
    auto Get     = [&level](const Sokoban::Pos& p)->char {
        return level.lines[p.row][p.col];
    };

    Sokoban::Pos s {playerRow, playerCol};
    visited[s.row][s.col] = true;
    queue<Sokoban::Pos> q;
    q.push(s);
    while (q.size()) {
        auto n = q.front();
        q.pop();
        std::array<Sokoban::Pos,4> dps = {Sokoban::Pos{-1,0},{1,0},{0,-1},{0,1}};
        for (auto dp:dps) {
            auto next = n+dp;
            if (!InBound(next)) {
                cout << "Debug " << __LINE__ << endl;
                cout << n.row << ' ' << n.col << endl;
                cout << dp.row << ' ' << dp.col << endl;
                return {}; // Wall not closed.
            }
            if (visited[next.row][next.col])
                continue;
            visited[next.row][next.col] = true;
            if (Get(next) != '#')
                q.push(next);
        }
    }
    // step 4: replace any tile outside the wall with '_'
    for (int i=0; i<M; i++)
        for (int j=0; j<N; j++)
            if (!visited[i][j])
                level.lines[i][j] = '_';

    // step 5: fill the corners,
    // So that we go from this to this:
    // __##__         _####_
    // _#  #_  -----> _#  #_
    // __##__         _####_
    // step 4: replace any tile outside the wall with '_'
    auto In = [M,N](int i, int j)->bool {
        return i >= 0 && i < M && j >= 0 && j < N;
    };
    for (int i=0; i<M; i++)
        for (int j=0; j<N; j++)
            if (level.lines[i][j] == '_') {
                bool fillIt = false;
                fillIt |= (In(i-1,j-1) && level.lines[i-1][j-1] != '#' && level.lines[i-1][j-1] != '_');
                fillIt |= (In(i-1,j+1) && level.lines[i-1][j+1] != '#' && level.lines[i-1][j+1] != '_');
                fillIt |= (In(i+1,j-1) && level.lines[i+1][j-1] != '#' && level.lines[i+1][j-1] != '_');
                fillIt |= (In(i+1,j+1) && level.lines[i+1][j+1] != '#' && level.lines[i+1][j+1] != '_');
                if (fillIt) {
                    level.lines[i][j] = '#';
                }
            }

    return {level};
}

bool Sokoban::LoadLevelFromFile(const char* levelFile) {
    ifstream fin(levelFile);
    vector<string> vs;
    bool fileGood = true;
    while (fileGood) {
        string s;
        fileGood = static_cast<bool>(getline(fin,s));
        if (s == "" && vs.size()) {
            auto level = LoadOneLevel(vs);
            vs.clear();
            if (level.has_value()) {
                levels.push_back(level.value());
            } else {
                return false;
            }
        } else if (s.size()) {
            vs.push_back(s);
        }
    }
    return levels.size() && !fileGood;
}

bool Sokoban::LoadLevels(const char *file) {
    levels.clear();
    if (!LoadLevelFromFile(file)) {
        LoadDefaultLevels();
        return false;
    } else {
        LoadLevel(levels[curLevel]);
    }
    return true;
}
