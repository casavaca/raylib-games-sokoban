[![Actions Status](https://github.com/casavaca/raylib-games-sokoban/actions/workflows/cmake-multi-platform.yml/badge.svg)](https://github.com/casavaca/raylib-games-sokoban/actions/workflows/cmake-multi-platform.yml)
[![Actions Status](https://github.com/casavaca/raylib-games-sokoban/actions/workflows/cmake-emscripten.yml/badge.svg)](https://github.com/casavaca/raylib-games-sokoban/actions/workflows/cmake-emscripten.yml)
[![codecov](https://codecov.io/gh/casavaca/raylib-games-sokoban/graph/badge.svg)](https://codecov.io/gh/casavaca/raylib-games-sokoban)

# Working in progress

[live preview](https://casavaca.github.io/raylib-games-sokoban/sokoban.html)

# How to build:

see [github workflow file](https://github.com/casavaca/raylib-games-sokoban/blob/master/.github/workflows/cmake-multi-platform.yml) for dependencies.

+ Debug build:   `cmake -S . -B build -DCMAKE_BUILD_TYPE=DEBUG           && cmake --build build`
+ Release build: `cmake -S . -B release-build -DCMAKE_BUILD_TYPE=RELEASE && cmake --build release-build`
+ WASM build:    `emcmake cmake -S . -B emscripten-build -DPLATFORM=Web  && cmake --build emscripten-build`

# How to run:

+ Linux: ./build/sokoban
+ Windows build: just run sokoban.exe.
+ Windows download: get the binary and assets from github CI [artifact](https://github.com/casavaca/raylib-games-sokoban/actions/workflows/cmake-multi-platform.yml), then put the exe and "assets" into the same directory. Note: GitHub signed-in needed to download workflow artifacts. [link](https://docs.github.com/en/actions/managing-workflow-runs/downloading-workflow-artifacts)
+ WASM example: `python3 -m http.server -d emscripten-build`

# Coding conventions:

based on [raylib-coding-conventions](https://github.com/raysan5/raylib/wiki/raylib-coding-conventions)
except that

+ `{` follows previous line.

# TODO

+ Load levels from txt file
+ select levels
+ config gui
+ more testing and coverage
