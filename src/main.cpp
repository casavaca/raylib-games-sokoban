#include "raylib-cpp.hpp"
#include "raylib.h"
#include "raygui.h"
#include "rgestures.h"

#include "game.hpp"
#include "game_gui.hpp"
#include "game_config.hpp"

#include <CLI/CLI.hpp>
#include <chrono>
#include <thread>

using namespace GameGui;
using namespace std;

int main(int argc, char** argv) {

    CLI::App app{"Sokoban"};
    string raylibEventFile;
    auto* option_record      = app.add_option("--record", raylibEventFile, "record input event");
    auto* option_replay      = app.add_option("--replay", raylibEventFile, "replay events from file")
                                       ->excludes(option_record);
    [[maybe_unused]] auto* _ = app.add_flag  ("--fast"  , "replay but faster")
                                       ->needs(option_replay);

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
    SetTargetFPS(60);               // Set our game to run at 60 frames-per-second
    SetExitKey(KEY_NULL);           // Disable quit-on-ESC
    //---------------------------------------------------------------------------------------

    // raylib event record and replay.
    vector<AutomationEvent> raylibEvents;
    AutomationEventList     raylibEventList;
    if (app.count("--record")) {
        raylibEvents.resize(16384);
        raylibEventList.events   = raylibEvents.data();
        raylibEventList.capacity = raylibEvents.size();
        raylibEventList.count    = 0;

        SetAutomationEventList(&raylibEventList);
        StartAutomationEventRecording();
    } else if (app.count("--replay")) {
        raylibEventList = LoadAutomationEventList(raylibEventFile.c_str());
        if (app.count("--fast"))
            SetTargetFPS(0); // unlimited FPS
    }

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
    }

    // raylib event record and replay.
#if defined(DEBUG)
    if (app.count("--replay")) {
        assert(raylibEventList.count == 0);
    }
#endif
    if (app.count("--record")) {
        StopAutomationEventRecording();
        ExportAutomationEventList(raylibEventList, raylibEventFile.c_str());
    }

    return 0;
}
