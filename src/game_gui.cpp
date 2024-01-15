#include "game_gui.hpp"
#include "game_event.hpp"
#include "game_config.hpp"
#include "raygui.h"
#include "raylib-cpp.hpp"
#include "raylib.h"

#include <unordered_map>
#include <cstdint>

using namespace std;

unordered_map<uint8_t, raylib::Texture> g_basePng;
unordered_map<uint8_t, std::vector<raylib::Texture*>> g_pngs;

namespace GameGui {

static GameScene scene = START_SCENE;

GameScene GetGameScene() {
    return scene;
}
void SetGameScene(GameScene newScene){
    scene = newScene;
}

void Init() {
#if defined(PLATFORM_WEB)
    std::string prefix = "assets/kenney_sokoban-pack/PNG/Default size";
#else
    std::string prefix = "assets/kenney_sokoban-pack/PNG/Retina";
#endif

    g_basePng[TILE_NULL            ] = raylib::Texture(prefix + "/Ground/ground_03.png");
    g_basePng[TILE_WALL            ] = raylib::Texture(prefix + "/Blocks/block_08.png");
    g_basePng[TILE_SPACE           ] = raylib::Texture(prefix + "/Ground/ground_04.png");
    g_basePng[TILE_TARGET          ] = raylib::Texture(prefix + "/Environment/environment_12.png");
    g_basePng[TILE_BOX             ] = raylib::Texture(prefix + "/Crates/crate_42.png");
    g_basePng[TILE_BOX_ON_TARGET   ] = raylib::Texture(prefix + "/Crates/crate_45.png");
    g_basePng[TILE_PLAYER_N        ] = raylib::Texture(prefix + "/Player/player_06.png");
    g_basePng[TILE_PLAYER_E        ] = raylib::Texture(prefix + "/Player/player_20.png");
    g_basePng[TILE_PLAYER_S        ] = raylib::Texture(prefix + "/Player/player_03.png");
    g_basePng[TILE_PLAYER_W        ] = raylib::Texture(prefix + "/Player/player_17.png");

    g_pngs[TILE_NULL               ] = {&g_basePng[TILE_NULL]};
    g_pngs[TILE_WALL               ] = {&g_basePng[TILE_NULL], &g_basePng[TILE_WALL]};
    g_pngs[TILE_SPACE              ] = {&g_basePng[TILE_SPACE]};
    g_pngs[TILE_TARGET             ] = {&g_basePng[TILE_SPACE], &g_basePng[TILE_TARGET]};
    g_pngs[TILE_BOX                ] = {&g_basePng[TILE_SPACE], &g_basePng[TILE_BOX]};
    g_pngs[TILE_BOX_ON_TARGET      ] = {&g_basePng[TILE_SPACE], &g_basePng[TILE_BOX_ON_TARGET]};
    g_pngs[TILE_PLAYER_N           ] = {&g_basePng[TILE_SPACE], &g_basePng[TILE_PLAYER_N]};
    g_pngs[TILE_PLAYER_E           ] = {&g_basePng[TILE_SPACE], &g_basePng[TILE_PLAYER_E]};
    g_pngs[TILE_PLAYER_S           ] = {&g_basePng[TILE_SPACE], &g_basePng[TILE_PLAYER_S]};
    g_pngs[TILE_PLAYER_W           ] = {&g_basePng[TILE_SPACE], &g_basePng[TILE_PLAYER_W]};
    g_pngs[TILE_PLAYER_N_ON_TARGET ] = {&g_basePng[TILE_SPACE], &g_basePng[TILE_PLAYER_N]};
    g_pngs[TILE_PLAYER_E_ON_TARGET ] = {&g_basePng[TILE_SPACE], &g_basePng[TILE_PLAYER_E]};
    g_pngs[TILE_PLAYER_S_ON_TARGET ] = {&g_basePng[TILE_SPACE], &g_basePng[TILE_PLAYER_S]};
    g_pngs[TILE_PLAYER_W_ON_TARGET ] = {&g_basePng[TILE_SPACE], &g_basePng[TILE_PLAYER_W]};
}

static void DrawGameScene(const Sokoban::State& state) {
    const int blockPixels = g_basePng[TILE_NULL].GetWidth();
    for (int i=0; i<state.size(); i++) {
        for (int j=0; j<state[i].size(); j++) {
            auto c = state[i][j];
            for (auto png:g_pngs[c]) {
                png->Draw(j*blockPixels, i*blockPixels);
            }
        }
    }
}

static int GetBlockPixels() {
#if defined(DEBUG)
    assert(g_basePng.at(TILE_NULL).GetWidth() == g_basePng.at(TILE_NULL).GetHeight());
#endif
    return g_basePng.at(TILE_NULL).GetWidth();
}

std::pair<int,int> GetWindowSize(const Sokoban::State& state) {
    return {static_cast<int>(state.size()) * GetBlockPixels(),
            static_cast<int>(state[0].size()) * GetBlockPixels()};
}

Sokoban::Pos PixelToPos(Vector2 pos) {
    int nrow = static_cast<int>(pos.y) / GetBlockPixels();
    int ncol = static_cast<int>(pos.x) / GetBlockPixels();
    return {nrow, ncol};
}

pair<vector<GameEvent>, GameEvent> CookInputEvent(void) {
    static int lastKeyPressed = KEY_NULL;
    bool leftButton  = IsMouseButtonPressed(MOUSE_LEFT_BUTTON) ||
                       IsMouseButtonDown(MOUSE_LEFT_BUTTON);
    bool rightButton = IsMouseButtonReleased(MOUSE_RIGHT_BUTTON);
    std::vector<GameEvent> gameEvents;
    GameEvent guiEvent = GameEvent::EVENT_NULL;

    if (IsKeyPressed(KEY_ESCAPE)) {
        guiEvent = GameEvent::EVENT_MENU_PAUSE;
    }

    if(GetGameScene() != MAIN_GAME_SCENE) {
        return {{}, guiEvent};
    }

    // TODO: add option for keyboard layout / AWERTY?
    if (auto key = GetKeyPressed())
        lastKeyPressed = key;

    if (leftButton) {
        gameEvents.push_back(GameEvent::EVENT_MOVE_CLICK);
    }
    if (rightButton) {
        gameEvents.push_back(GameEvent::EVENT_MOVE_REGRET);
    } else if (IsKeyPressed(lastKeyPressed) || IsKeyPressedRepeat(lastKeyPressed)) {
        auto key = lastKeyPressed;
        if (GameConfig::IsUp     (key)) { gameEvents.push_back(GameEvent::EVENT_MOVE_UP);     }
        if (GameConfig::IsDown   (key)) { gameEvents.push_back(GameEvent::EVENT_MOVE_DOWN);   }
        if (GameConfig::IsRight  (key)) { gameEvents.push_back(GameEvent::EVENT_MOVE_RIGHT);  }
        if (GameConfig::IsLeft   (key)) { gameEvents.push_back(GameEvent::EVENT_MOVE_LEFT);   }
        if (GameConfig::IsRestart(key)) { gameEvents.push_back(GameEvent::EVENT_MOVE_RESTART);}
        if (GameConfig::IsRegret (key)) { gameEvents.push_back(GameEvent::EVENT_MOVE_REGRET); }
    }
    return {gameEvents,guiEvent};
}

GameEvent Draw(raylib::Window& window, const Sokoban& game) {
    Rectangle Rects[] = {{ 45, 115, 325, 60 }, { 45, 215, 325, 60 }, { 45, 315, 325, 60 }};

    // TODO: de-duplicate these codes.
    //     : better shortcut support.
    // TODO: should use IsKeyPressed
    switch(GetGameScene()) {
    case START_SCENE: {
        auto key = GetKeyPressed();
        char textBoxText[64] = "Start (SPACE)";
        bool textBoxEditMode = false;
        if (key == ' ' || GuiTextBox(Rects[0], textBoxText, 64, textBoxEditMode)) {
            return GameEvent::EVENT_MENU_START;
        }
        char exitBoxText[64] = "Exit (Q)";
        bool exitBoxEditMode = false;
        if (key == KEY_Q || GuiTextBox(Rects[1], exitBoxText, 64, exitBoxEditMode)) {
            return GameEvent::EVENT_MENU_EXIT;
        }
    } break;
    case ESC_SCENE: {
        auto key = GetKeyPressed();
        char resumeBoxText[64] = "Resume (SPACE)";
        bool resumeBoxEditMode = false;
        if (key == ' ' || GuiTextBox(Rects[0], resumeBoxText, 64, resumeBoxEditMode)) {
            return GameEvent::EVENT_MENU_RESUME;
        }
        char textBoxText[64] = "Restart (R)";
        bool textBoxEditMode = false;
        if (key == KEY_R || GuiTextBox(Rects[1], textBoxText, 64, textBoxEditMode)) {
            SetGameScene(MAIN_GAME_SCENE);
            return GameEvent::EVENT_MENU_RESTART;
        }
        char exitBoxText[64] = "Exit (Q)";
        bool exitBoxEditMode = false;
        if (key == KEY_Q || GuiTextBox(Rects[2], exitBoxText, 64, exitBoxEditMode)) {
            return GameEvent::EVENT_MENU_EXIT;
        }
    } break;
    case MAIN_GAME_SCENE: {
        auto expectedsize = GameGui::GetWindowSize(game.GetState());
        if (GetScreenWidth () != expectedsize.first ||
            GetScreenHeight() != expectedsize.second) {
            window.SetSize(expectedsize.first, expectedsize.second);
        }
        DrawGameScene(game.GetState());
    } break;
    default: break;
    }
    return GameEvent::EVENT_NULL;
}

void ProcessGuiEvent(GameEvent e, Sokoban& game) {
    switch (e) {
    case GameEvent::EVENT_MENU_START: {
        SetGameScene(MAIN_GAME_SCENE);
    } break;
    case GameEvent::EVENT_MENU_EXIT: {
        exit(0);
    } break;
    case GameEvent::EVENT_MENU_RESTART: {
        SetGameScene(MAIN_GAME_SCENE);
        game.Restart();
    } break;
    case GameEvent::EVENT_MENU_PAUSE: {
        SetGameScene(ESC_SCENE);
    } break;
    case GameEvent::EVENT_MENU_RESUME: {
        SetGameScene(MAIN_GAME_SCENE);
    } break;
    case GameEvent::EVENT_NULL:
        break;
    default: {
#if defined(DEBUG)
        assert(false);
#endif
    }
    }
}

}

