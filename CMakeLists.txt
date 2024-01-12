cmake_minimum_required(VERSION 3.14)
project(sokoban)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# raylib
find_package(raylib QUIET)
if (NOT raylib_FOUND)
    include(FetchContent)
    FetchContent_Declare(
        raylib
        URL      https://github.com/raysan5/raylib/archive/refs/tags/5.0.tar.gz
        URL_HASH SHA256=98f049b9ea2a9c40a14e4e543eeea1a7ec3090ebdcd329c4ca2cf98bc9793482
        # Note that due to a long-unresolved bug in cmake
        # (still open and no one is working on it)
        # https://gitlab.kitware.com/cmake/cmake/-/issues/17770
        # GIT_SHALLOW is not shallow enough.
        # So, prefer URL if possible.
    )
    FetchContent_MakeAvailable(raylib)
endif()

# raylib-gui
find_package(raygui QUIET)
if (NOT raygui_FOUND)
    include(FetchContent)
    FetchContent_Declare(
        raygui
        URL https://github.com/raysan5/raygui/archive/refs/tags/4.0.tar.gz
        URL_HASH SHA256=299c8fcabda68309a60dc858741b76c32d7d0fc533cdc2539a55988cee236812
    )
    FetchContent_MakeAvailable(raygui)
    # copy style files to binary dir / assets
    # TODO: don't copy for Web build
    file(COPY ${raygui_SOURCE_DIR}/styles DESTINATION ${CMAKE_BINARY_DIR}/assets)
    include_directories(${raygui_SOURCE_DIR}/src)
endif()

# raylib-cpp
find_package(raylib_cpp QUIET)
if (NOT raylib_cpp_FOUND)
    include(FetchContent)
    FetchContent_Declare(
        raylib_cpp
        URL      https://github.com/RobLoach/raylib-cpp/archive/refs/tags/v5.0.0.tar.gz
        URL_HASH SHA256=c152ee48b48a9114e0250ecd4e4d853ad18592d4a9563b23366ee43e89237391
    )
    FetchContent_MakeAvailable(raylib_cpp)
endif()

file(COPY "${CMAKE_CURRENT_LIST_DIR}/assets" DESTINATION "${CMAKE_BINARY_DIR}")
file(GLOB SOURCES "${CMAKE_CURRENT_LIST_DIR}/src/*.cpp" "${CMAKE_CURRENT_LIST_DIR}/src/*.c")
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
    target_link_options(${PROJECT_NAME} PRIVATE --preload-file assets)
    target_link_options(${PROJECT_NAME} PRIVATE -sASSERTIONS=1 -sASYNCIFY -sGL_ENABLE_GET_PROC_ADDRESS=1)
    target_link_options(${PROJECT_NAME} PRIVATE --shell-file ${PROJECT_SOURCE_DIR}/src/template.html)
    target_link_options(${PROJECT_NAME} PRIVATE -sEXPORTED_FUNCTIONS=['_main','_malloc'] -sEXPORTED_RUNTIME_METHODS=ccall -sUSE_GLFW=3)
endif()
