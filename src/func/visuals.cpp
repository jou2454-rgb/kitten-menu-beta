#include "visuals.hpp"
#include "../game/game.hpp"
#include "ImGui/imgui.h"
#include <cmath>

extern bool esp_enabled;
extern bool esp_box;
extern bool esp_line;
extern bool esp_health;
extern bool esp_name;
extern int local_team;

namespace visuals {

static void draw_box(ImDrawList* dl, float x, float y, float w, float h, ImU32 col) {
    dl->AddRect(ImVec2(x, y), ImVec2(x + w, y + h), col, 0.f, 0, 2.f);
}

static void draw_health_bar(ImDrawList* dl, float x, float y, float h, float hp, float max_hp) {
    float ratio = hp / max_hp;
    if (ratio > 1.f) ratio = 1.f;
    if (ratio < 0.f) ratio = 0.f;
    float bar_h = h * ratio;
    ImU32 col = IM_COL32(
        (int)((1.f - ratio) * 255),
        (int)(ratio * 255),
        0, 255
    );
    dl->AddRectFilled(ImVec2(x - 6, y + h - bar_h), ImVec2(x - 2, y + h), col);
    dl->AddRect(ImVec2(x - 6, y), ImVec2(x - 2, y + h), IM_COL32(0, 0, 0, 200));
}

void render(float screen_w, float screen_h) {
    if (!esp_enabled) return;

    auto vm = game::get_view_matrix();
    ImDrawList* dl = ImGui::GetBackgroundDrawList();
    int count = game::get_player_count();

    for (int i = 0; i < count && i < 32; i++) {
        uintptr_t player = game::get_player(i);
        if (!player) continue;
        if (!game::is_alive(player)) continue;

        int team = game::get_team(player);
        if (team == local_team) continue;

        game::Vector3 pos = game::get_position(player);
        game::Vector3 head = pos;
        head.y += 1.8f;

        game::Vector3 screen_pos, screen_head;
        if (!game::world_to_screen(pos, vm, screen_w, screen_h, screen_pos)) continue;
        if (!game::world_to_screen(head, vm, screen_w, screen_h, screen_head)) continue;

        float box_h = screen_pos.y - screen_head.y;
        float box_w = box_h * 0.5f;
        float box_x = screen_head.x - box_w / 2.f;
        float box_y = screen_head.y;

        ImU32 col = IM_COL32(255, 50, 50, 255);

        if (esp_box) {
            draw_box(dl, box_x, box_y, box_w, box_h, col);
        }

        if (esp_line) {
            dl->AddLine(
                ImVec2(screen_w / 2.f, screen_h),
                ImVec2(screen_pos.x, screen_pos.y),
                col, 1.5f
            );
        }

        if (esp_health) {
            float hp = game::get_health(player);
            float max_hp = game::get_max_health(player);
            draw_health_bar(dl, box_x, box_y, box_h, hp, max_hp);
        }

        if (esp_name) {
            char label[32];
            snprintf(label, sizeof(label), "[%dm]", (int)pos.length());
            dl->AddText(ImVec2(screen_head.x, screen_head.y - 14), IM_COL32(255, 255, 255, 255), label);
        }
    }
}

} // namespace visuals
