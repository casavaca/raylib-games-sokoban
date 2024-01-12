# Working in progress

# How to build:

+ Debug build:   `cmake -S . -B build -DCMAKE_BUILD_TYPE=DEBUG           && cmake --build build`
+ Release build: `cmake -S . -B release-build -DCMAKE_BUILD_TYPE=RELEASE && cmake --build release-build`
+ WASM build:    `emcmake cmake -S . -B emscripten-build -DPLATFORM=Web  && cmake --build emscripten-build`
+ host the html: `python3 -m http.server -d emscripten-build`

# How to run:

+ Linux: ./build/sokoban
+ Windows build: sokoban.exe and assets need to be at the same level. Currently they are not; the binary is one level lower in Debug/ or in Release/. This is a TODO. Just move things around for now. There is CI-built artifact in [artifact](https://github.com/casavaca/raylib-games-sokoban/actions). You can also download and extract sokoban.exe, put it in some folder that also has "assets" directory from source code.
+ WASM: just visit localhost 8000 by python3 -m http.server

# Coding conventions:

same as [raylib-coding-conventions](https://github.com/raysan5/raylib/wiki/raylib-coding-conventions)

# TODO

+ Load levels from txt file
+ select levels
+ config gui
+ testing (raylib `RecordAutomationEvent` and `PlayAutomationEvent`)
+ Windows CMakeList.txt tweak.
