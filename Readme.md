# Working in progress

# How to build:

+ Debug build:   `cmake -S . -B build -DCMAKE_BUILD_TYPE=DEBUG           && cmake --build build`
+ Release build: `cmake -S . -B release-build -DCMAKE_BUILD_TYPE=RELEASE && cmake --build release-build`
+ WASM build:    `emcmake cmake -S . -B emscripten-build -DPLATFORM=Web  && cmake --build emscripten-build`

# How to run:

+ Linux: ./build/sokoban
+ Windows build: just run sokoban.exe.
+ Windows download: get the binary from github CI [artifact](https://github.com/casavaca/raylib-games-sokoban/actions), then
                    download "assets" and put it to the same directory as sokoban.exe
+ WASM example: `python3 -m http.server -d emscripten-build`

# Coding conventions:

same as [raylib-coding-conventions](https://github.com/raysan5/raylib/wiki/raylib-coding-conventions)

# TODO

+ Load levels from txt file
+ select levels
+ config gui
+ testing (raylib `RecordAutomationEvent` and `PlayAutomationEvent`)
