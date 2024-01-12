# Working in progress

# How to build and run:

+ Debug build:   `make debug`
+ Release build: `make release`
+ WASM build:    `make emscripten`
+ host the html: `python3 -m http.server -d emscripten-build`
+ Windows: download the [artifact](https://github.com/casavaca/raylib-games-sokoban/actions) from most recent build and extract sokoban.exe
           download "assets" directory from source code to the same folder, then run sokoban.exe

# Coding conventions:

same as [raylib-coding-conventions](https://github.com/raysan5/raylib/wiki/raylib-coding-conventions)

# TODO

+ Load levels from txt file
+ select levels
+ config gui
+ testing (raylib `RecordAutomationEvent` and `PlayAutomationEvent`)
+ github CI
