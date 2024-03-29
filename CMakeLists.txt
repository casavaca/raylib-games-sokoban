cmake_minimum_required(VERSION 3.14)
project(sokoban)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Avoid warning about DOWNLOAD_EXTRACT_TIMESTAMP in CMake 3.24:
# This is a rare case and doesn't actually matter here.
# https://github.com/ethereum/solidity/pull/13429
if (CMAKE_VERSION VERSION_GREATER_EQUAL "3.24.0")
    cmake_policy(SET CMP0135 NEW)
endif()

################################################################################
# Dependencies
################################################################################

include(FetchContent)

################################################################################
# raylib
################################################################################
find_package(raylib QUIET)
if (NOT raylib_FOUND)
    FetchContent_Declare(
        raylib

        # URL      https://github.com/raysan5/raylib/archive/refs/tags/5.0.tar.gz
        # URL_HASH SHA256=98f049b9ea2a9c40a14e4e543eeea1a7ec3090ebdcd329c4ca2cf98bc9793482

        # Note that due to a long-unresolved bug in cmake
        # (still open and no one is working on it)
        # https://gitlab.kitware.com/cmake/cmake/-/issues/17770
        # GIT_SHALLOW is not shallow enough.
        # So, prefer URL if possible.
        # TODO: use 5.1.tar.gz when we got a new release.
        GIT_REPOSITORY https://github.com/raysan5/raylib.git
        GIT_TAG master
    )
    FetchContent_MakeAvailable(raylib)
endif()

################################################################################
# raylib-gui
################################################################################
find_package(raygui QUIET)
if (NOT raygui_FOUND)
    FetchContent_Declare(
        raygui
        URL https://github.com/raysan5/raygui/archive/refs/tags/4.0.tar.gz
        URL_HASH SHA256=299c8fcabda68309a60dc858741b76c32d7d0fc533cdc2539a55988cee236812
    )
    FetchContent_MakeAvailable(raygui)
    include_directories(${raygui_SOURCE_DIR}/src)
endif()

################################################################################
# raylib-cpp
################################################################################
find_package(raylib_cpp QUIET)
if (NOT raylib_cpp_FOUND)
    FetchContent_Declare(
        raylib_cpp
        URL      https://github.com/RobLoach/raylib-cpp/archive/refs/tags/v5.0.0.tar.gz
        URL_HASH SHA256=c152ee48b48a9114e0250ecd4e4d853ad18592d4a9563b23366ee43e89237391
    )
    FetchContent_MakeAvailable(raylib_cpp)
endif()

################################################################################
# CLI11
################################################################################
FetchContent_Declare(
    cli11_proj
    QUIET
    GIT_REPOSITORY https://github.com/CLIUtils/CLI11.git
    GIT_TAG v2.3.2
)
FetchContent_MakeAvailable(cli11_proj)

################################################################################
# End Dependencies
################################################################################

################################################################################
# main target
################################################################################

# find all source files.
file(GLOB SOURCES "${CMAKE_CURRENT_LIST_DIR}/src/*.cpp" "${CMAKE_CURRENT_LIST_DIR}/src/*.c")

# add the main target.
add_executable(${PROJECT_NAME} ${SOURCES})

set_target_properties (${PROJECT_NAME} PROPERTIES CXX_STANDARD 17)
target_compile_options(${PROJECT_NAME} PRIVATE    $<$<CONFIG:Debug>:-DDEBUG>)
target_link_libraries (${PROJECT_NAME} PRIVATE    raylib raylib_cpp CLI11::CLI11)

if(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
  target_compile_options(${PROJECT_NAME} PRIVATE $<$<COMPILE_LANGUAGE:CXX>:-pedantic -Wimplicit-fallthrough -Wswitch-enum -Wall -Wextra -Wno-unused-function -Wno-sign-compare -Werror>)
endif()

if (WIN32)
    # For release build, hide the console.
    # Oh, but windows defender is not happy with this hack.
    # I believe one way to work around it is to make it a proper windows app.
    # removing ENTRY:mainCRTStartup and define WinMain.
    # change main to something like MyMain and call it from WinMain.
    # But I've wasted enough time tweaking with windows, so leave a console there for now.
    # target_link_options(${PROJECT_NAME} PRIVATE $<$<CONFIG:Release>:/SUBSYSTEM:WINDOWS>)
    # target_link_options(${PROJECT_NAME} PRIVATE $<$<CONFIG:Release>:/ENTRY:mainCRTStartup>)
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

################################################################################
# copy assets
################################################################################

# copy assets and "${raygui_SOURCE_DIR}/styles" into BIN_DIR/assets.
#
# To correctly get ${BIN_DIR} is not trivial.
#
# https://cmake.org/cmake/help/latest/manual/cmake-buildsystem.7.html#build-configurations
# The cmake build system is not very intuitive.
# cmake generates files for backend to consume, e.g., GNU Makefile, Ninja, Visual Studio, Xcode.
# Of these, some are single-config, other are multi-config.
# e.g., GNU Makefile is single-config. So, cmake generate different Makefiles for different config.
# but Visual Studio is multi-config, So, cmake just generate single project.sln for all configs.
# $CMAKE_CONFIGURATION_TYPES is defined for multi-config  generators, which contains a list of possible configs.
# $CMAKE_BUILD_TYPE          is defined for single-config generators.
#
# IMPORTANT NOTES:
#
# For multi-configuration generators like Visual Studio, Xcode, and Ninja Multi-Config,
# the configuration is chosen by the user at build time and CMAKE_BUILD_TYPE is ignored.
# ... the actual configuration used cannot be known until the build stage.
#
# So, BIN_DIR can't be determined at cmake generation time.
#
# (except you overwrite it and build all config to single location, see [this](https://discourse.cmake.org/t/changing-output-directories/8829), but not good.)
#
# So, we can't just use file(COPY ... DESTINATION ...) because it is too early to decide
# where the actual destination is.
#
# Now we have some options: PRE_BUILD, PRE_LINK, POST_BUILD.
# POST_BUILD is too late for emscripten, which need to pack the resources.
# PRE_BUILD should [generally be avoided](https://cmake.org/cmake/help/latest/command/add_custom_command.html#build-events), So we use PRE_LINK command
if(CMAKE_CONFIGURATION_TYPES)
    # Multi-config generator (e.g., Visual Studio)
    set(BIN_DIR ${CMAKE_BINARY_DIR}/$<CONFIG>)
else()
    # Single-config generator (e.g., Makefile, Ninja)
    set(BIN_DIR ${CMAKE_BINARY_DIR}/)
endif()
add_custom_command(TARGET ${PROJECT_NAME} PRE_LINK
    COMMENT "Copying assets to the destination directory"
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_LIST_DIR}/assets" ${BIN_DIR}/assets
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${raygui_SOURCE_DIR}/styles"      ${BIN_DIR}/assets/styles
)

################################################################################
# test
################################################################################

enable_testing()
add_test(NAME smoketest COMMAND xvfb-run -s "+extension GLX" ${BIN_DIR}/${PROJECT_NAME} --fps 0 --replay ${CMAKE_CURRENT_LIST_DIR}/test/test.events)
set_tests_properties(smoketest PROPERTIES TIMEOUT 30)

################################################################################
# coverage using llvm-cov
################################################################################

# Include the custom build type configuration file
# add -DNDEBUG because the coverage tool is still not accurate around assert, even at -O0
set(CMAKE_CXX_FLAGS_COVERAGE "-fprofile-instr-generate -fcoverage-mapping -g -O0 -DCOVERAGE -DNDEBUG")
