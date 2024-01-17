#include "raylib.h"
#include "raygui.h"
#include "rgestures.h"

#include "game.hpp"
#include "game_gui.hpp"

#include <CLI/CLI.hpp>

#include <cassert>

using namespace GameGui;
using namespace std;

int main(int argc, char** argv) {

    CLI::App app{"Sokoban"};
    string raylibEventFile;
    int FPS = 60;
#if defined(DEBUG) || defined(COVERAGE)
    auto* option_record       = app.add_option("--record", raylibEventFile, "record input event");
    [[maybe_unused]] auto* _1 = app.add_option("--replay", raylibEventFile, "replay events from file")
                                        ->excludes(option_record);
#endif
    [[maybe_unused]] auto* _2 = app.add_option("--fps",    FPS, "Set FPS (intended for testing only)")
                                        ->default_val(60);

    CLI11_PARSE(app, argc, argv);

    // Initialization
    //--------------------------------------------------------------------------------------
    raylib::Window window(800, 600, "sokoban");

    GuiLoadStyle("assets/styles/cyber/cyber.rgs");
    GuiSetStyle(DEFAULT, TEXT_SIZE, 40);
    Sokoban       game;
    GameResources gameResources;
    game.LoadDefaultLevels();

    GameGui::Init(&gameResources);
    SetTargetFPS(FPS);              // Set FPS
    SetExitKey(KEY_NULL);           // Disable quit-on-ESC
    //---------------------------------------------------------------------------------------

#if defined(DEBUG) || defined(COVERAGE)
    // raylib event record and replay, testing only.
    vector<AutomationEvent> raylibEvents;
    AutomationEventList     raylibEventList;
    if (app.count("--record")) {
        raylibEvents.resize(16384);
        raylibEventList.events   = raylibEvents.data();
        raylibEventList.capacity = raylibEvents.size();
        raylibEventList.count    = 0;

        SetAutomationEventList(&raylibEventList);
        StartAutomationEventRecording();
    }
    if (app.count("--replay")) {
        raylibEventList = LoadAutomationEventList(raylibEventFile.c_str());
    }
#endif

    // Main game loop
    bool shouldClose = false;
    while (!shouldClose) {
        //----------------------------------------------------------------------------------
        // Update
        //----------------------------------------------------------------------------------
        Sokoban::Pos pos = GameGui::PixelToPos(GetMousePosition());
        auto [gameEvents, guiEvent] = GameGui::CookInputEvent(game);
        game.ProcessEvent(gameEvents, pos);

        //----------------------------------------------------------------------------------
        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();
        window.ClearBackground(LIGHTGRAY);
        auto event = GameGui::Draw(window, game);
        EndDrawing();

        shouldClose |= GameGui::ProcessGuiEvent(guiEvent, game);
        shouldClose |= GameGui::ProcessGuiEvent(event,    game);
        shouldClose |= window.ShouldClose(); // window close button

        //----------------------------------------------------------------------------------
        // Replay input events
        //----------------------------------------------------------------------------------
#if defined(DEBUG) || defined(COVERAGE)
        if (app.count("--replay")) {
            static int current_frame = 0;
            if (raylibEventList.count == 0)
                break;
            while (raylibEventList.count) {
                if (raylibEventList.events->frame == current_frame) {
                    PlayAutomationEvent(*raylibEventList.events);
                    raylibEventList.events++;
                    raylibEventList.count--;
                } else {
                    break;
                }
            }
            current_frame++;
        }
#endif
    }

    // raylib event record and replay.
#if defined(DEBUG) || defined(COVERAGE)
    if (app.count("--replay")) {
        assert(raylibEventList.count == 0);
    }
#endif
#if defined(DEBUG)
    if (app.count("--record")) {
        StopAutomationEventRecording();
        ExportAutomationEventList(raylibEventList, raylibEventFile.c_str());
    }
#endif
    return 0;
}
