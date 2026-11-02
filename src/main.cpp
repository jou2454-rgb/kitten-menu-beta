#include "Android_draw/draw.h"
#include "native_surface/ANativeWindowCreator.h"
#include "ImGui/imgui.h"
#include "ImGui/imgui_internal.h"
#include "ImGui/backends/imgui_impl_opengl3.h"
#include "ImGui/backends/imgui_impl_android.h"
#include "Android_touch/Touch.hpp"
#include "ui/theme/theme.hpp"
#include "ui/menu.hpp"
#include "ui/bar.hpp"
#include "other/memory.hpp"
#include "other/diag.hpp"
#include "game/game.hpp"
#include "func/visuals.hpp"

#include <unistd.h>
#include <cstdio>
#include <thread>

// globals
bool esp_enabled = true;
bool esp_box = true;
bool esp_line = false;
bool esp_health = true;
bool esp_name = true;
bool menu_open = true;
int local_team = -1;

static float screen_w = 0;
static float screen_h = 0;

static bool wait_for_game() {
    LOGI("waiting for %s...", game::PACKAGE);
    for (int i = 0; i < 120; i++) {
        if (game::init()) {
            LOGI("game found, pid=%d libunity=0x%lx", mem::game_pid, game::lib_base);
            return true;
        }
        sleep(2);
    }
    LOGE("game not found after 240s");
    return false;
}

int main() {
    LOGI("standoff2-esp starting");

    if (!wait_for_game()) return 1;

    auto di = android::ANativeWindowCreator::GetDisplayInfo();
    screen_w = (float)di.width;
    screen_h = (float)di.height;

    if (screen_w == 0 || screen_h == 0) {
        screen_w = 2400;
        screen_h = 1080;
        LOGI("fallback resolution %dx%d", (int)screen_w, (int)screen_h);
    }

    LOGI("screen: %dx%d", (int)screen_w, (int)screen_h);

    if (!initGUI_draw((uint32_t)screen_w, (uint32_t)screen_h, true)) {
        LOGE("failed to init overlay");
        return 1;
    }

    theme::apply();

    while (true) {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplAndroid_NewFrame();
        ImGui::NewFrame();

        updateTouch();

        bar::render(screen_w);
        menu::render();
        visuals::render(screen_w, screen_h);

        ImGui::Render();
        glClearColor(0, 0, 0, 0);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        eglSwapBuffers(display, surface);
    }

    return 0;
}
