# Working in progress

# How to build and run:

+ Debug build:
  `mkdir build && cd build && cmake .. -DCMAKE_BUILD_TYPE=DEBUG && make && ./sokoban`

+ Release build:
  `mkdir build && cd build && cmake .. && make && ./sokoban`

# Coding conventions:

same as [raylib-coding-conventions](https://github.com/raysan5/raylib/wiki/raylib-coding-conventions)

# TODO

+ Load levels from txt file
+ select levels
+ testing (raylib `RecordAutomationEvent` and `PlayAutomationEvent`)
+ add github CI
