cmake_minimum_required(VERSION 3.14)
project(raylib_cpp_example)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# raylib
find_package(raylib QUIET)
if (NOT raylib_FOUND)
    include(FetchContent)
    FetchContent_Declare(
        raylib
        GIT_REPOSITORY https://github.com/raysan5/raylib.git
        GIT_TAG 5.0
    )
    FetchContent_MakeAvailable(raylib)
endif()

# raylib-cpp
find_package(raylib_cpp QUIET)
if (NOT raylib_cpp_FOUND)
    include(FetchContent)
    FetchContent_Declare(
        raylib_cpp
        GIT_REPOSITORY https://github.com/RobLoach/raylib-cpp.git
        GIT_TAG v5.0.0
    )
    FetchContent_MakeAvailable(raylib_cpp)
endif()

file(GLOB SOURCES "${CMAKE_CURRENT_LIST_DIR}/src/*.cpp")
# Define SOURCES as a list of all source files
add_executable(${PROJECT_NAME} ${SOURCES})
set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD 17)
target_compile_options(${PROJECT_NAME} PRIVATE "-Wno-c99-designator")
target_link_libraries(${PROJECT_NAME} PUBLIC raylib raylib_cpp)

if(CMAKE_BUILD_TYPE MATCHES [Dd][Ee][Bb][Uu][Gg])
    target_compile_options(${PROJECT_NAME} PRIVATE "-DDEBUG")
endif()

# Web Configurations
if (${PLATFORM} STREQUAL "Web")
    # Tell Emscripten to build an example.html file.
    set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".html")

    # Required linker flags for using Raylib with Emscripten
    target_link_options(${PROJECT_NAME} PRIVATE -sEXPORTED_FUNCTIONS=['_main','_malloc'] -sEXPORTED_RUNTIME_METHODS=ccall -sUSE_GLFW=3)
endif()

# That's it! You should have an example executable that you can run. Have fun!
