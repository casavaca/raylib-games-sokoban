#include "game_gui.hpp"
#include "game_event.hpp"
#include "game_config.hpp"
#include "raygui.h"
#include "raylib.h"

#include <cassert>

using namespace std;

unordered_map<uint8_t, raylib::Texture>*              g_textures;
unordered_map<uint8_t, std::vector<raylib::Texture*>> g_pngs;

namespace GameGui {

static GameScene scene = START_SCENE;

GameScene GetGameScene() {
    return scene;
}
void SetGameScene(GameScene newScene){
    scene = newScene;
}

void Init(GameResources* resourcePtr) {
    // g_basePng used to be a global static variable.
    // But I've see segfault in glDeleteTextures if it is destructed too late.
    // So, let's just keep a reference in main, so it's destructed timely.
    g_textures = &resourcePtr->textures;
    auto& g_basePng = *g_textures;
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
    auto& g_basePng = *g_textures;
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
    auto& g_basePng = *g_textures;
    assert(g_basePng.at(TILE_NULL).GetWidth() == g_basePng.at(TILE_NULL).GetHeight());
    return g_basePng.at(TILE_NULL).GetWidth();
}

std::pair<int,int> GetWindowSize(const Sokoban::State& state) {
    return {static_cast<int>(state[0].size()) * GetBlockPixels(),
            static_cast<int>(state.size())    * GetBlockPixels()};
}

Sokoban::Pos PixelToPos(Vector2 pos) {
    int nrow = static_cast<int>(pos.y) / GetBlockPixels();
    int ncol = static_cast<int>(pos.x) / GetBlockPixels();
    return {nrow, ncol};
}

pair<vector<GameEvent>, GuiEvent> CookInputEvent(const Sokoban& game) {
    static int lastKeyPressed = KEY_NULL;
    bool leftButton  = IsMouseButtonPressed(MOUSE_LEFT_BUTTON) ||
                       IsMouseButtonDown(MOUSE_LEFT_BUTTON);
    bool rightButton = IsMouseButtonReleased(MOUSE_RIGHT_BUTTON);
    std::vector<GameEvent> gameEvents;
    GuiEvent guiEvent = GuiEvent::EVENT_NULL;

    if (IsKeyPressed(KEY_ESCAPE)) {
        guiEvent = GuiEvent::EVENT_MENU_PAUSE;
    }

    if (GetGameScene() != MAIN_GAME_SCENE) {
        return {{}, guiEvent};
    }

    if (game.LevelCompleted()) {
        return {{}, GuiEvent::EVENT_MENU_LEVEL_FINISHED};
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

// TODO: prototyping.
struct MyGuiButton {
    bool disabled;
    const char* text;
    int shortcutKey;
    GuiEvent event;
};

GuiEvent Draw(raylib::Window& window, const Sokoban& game) {

    // TODO: get rid of the magic numbers.
    Rectangle   rects[]  = {{ 45, 115, 425, 60 }, { 45, 215, 425, 60 }, { 45, 315, 425, 60 }};
    int         numRects = 0;

    MyGuiButton nextLevelButton    { false, "Next Level (SPACE)" , KEY_SPACE, GuiEvent::EVENT_MENU_NEXT_LEVEL };
    MyGuiButton noMoreLevelsButton { true,  "All Level Completed", KEY_NULL,  GuiEvent::EVENT_NULL            };
    MyGuiButton startButton        { false, "Start (SPACE)"      , KEY_SPACE, GuiEvent::EVENT_MENU_START      };
    MyGuiButton resumeButton       { false, "Resume (SPACE)"     , KEY_SPACE, GuiEvent::EVENT_MENU_RESUME     };
    MyGuiButton restartButton      { false, "Restart (R)"        , KEY_R,     GuiEvent::EVENT_MENU_RESTART    };
    MyGuiButton quitButton         { false, "Quit (Q)"           , KEY_Q,     GuiEvent::EVENT_MENU_EXIT       };

    auto CreateButton = [&rects, &numRects](const MyGuiButton& b)->bool{
        GuiSetStyle(BUTTON, TEXT_ALIGNMENT, TEXT_ALIGN_LEFT);
        if (b.disabled) {
            GuiSetState(STATE_DISABLED);
            GuiButton(rects[numRects++], b.text);
            GuiSetState(STATE_NORMAL);
            return false;
        } else {
            return (IsKeyPressed(b.shortcutKey) || GuiButton(rects[numRects++], b.text));
        }
    };

    auto ret = GuiEvent::EVENT_NULL;

    int expectedWidth  = 800;
    int expectedHeight = 600;
    if (GetGameScene() == MAIN_GAME_SCENE) {
        tie(expectedWidth, expectedHeight) = GameGui::GetWindowSize(game.GetState());
    }
    if (GetScreenWidth () != expectedWidth ||
        GetScreenHeight() != expectedHeight) {
        window.SetSize(expectedWidth, expectedHeight);
    }

    // NOTE: There will be 1 frame tear if we return early.
    switch(GetGameScene()) {
    case START_SCENE: {
        for (auto& button : {startButton, quitButton})
            if (CreateButton(button))
                ret = button.event;
    } break;
    case ESC_SCENE: {
        for (auto& button : {resumeButton, restartButton, quitButton})
            if (CreateButton(button))
                ret = button.event;
    } break;
    case LEVEL_FINISHED_SCENE: {
        for (auto& button : {game.IsLastLevel() ? noMoreLevelsButton : nextLevelButton, restartButton, quitButton})
            if (CreateButton(button))
                ret = button.event;
    } break;
    case MAIN_GAME_SCENE: {
        DrawGameScene(game.GetState());
    } break;
    default: Unreachable(); break;
    }
    return ret;
}

// Return true if we should exit
bool ProcessGuiEvent(GuiEvent e, Sokoban& game) {
    switch (e) {
    case GuiEvent::EVENT_MENU_START: {
        SetGameScene(MAIN_GAME_SCENE);
    } break;
    case GuiEvent::EVENT_MENU_EXIT: {
        return true;
    } break;
    case GuiEvent::EVENT_MENU_RESTART: {
        SetGameScene(MAIN_GAME_SCENE);
        game.Restart();
    } break;
    case GuiEvent::EVENT_MENU_PAUSE: {
        if (GetGameScene() == ESC_SCENE)
            SetGameScene(MAIN_GAME_SCENE);
        else if (GetGameScene() == MAIN_GAME_SCENE)
            SetGameScene(ESC_SCENE);
    } break;
    case GuiEvent::EVENT_MENU_RESUME: {
        SetGameScene(MAIN_GAME_SCENE);
    } break;
    case GuiEvent::EVENT_MENU_LEVEL_FINISHED: {
        // Ignore the event (which will be issued again) if
        // any mouse button is down.
        // This is because if we switch scene too early,
        // the mouse-up will be detected as button click,
        // which mayn result the user clicking some button without
        // their intention.
        if (IsMouseButtonDown(MOUSE_LEFT_BUTTON) ||
            IsMouseButtonReleased(MOUSE_LEFT_BUTTON))
            break;
        SetGameScene(LEVEL_FINISHED_SCENE);
    } break;
    case GuiEvent::EVENT_MENU_NEXT_LEVEL: {
        SetGameScene(MAIN_GAME_SCENE);
        assert(!game.IsLastLevel());
        game.NextLevel();
    } break;
    case GuiEvent::EVENT_NULL:
        break;
    }
    return false;
}

}

