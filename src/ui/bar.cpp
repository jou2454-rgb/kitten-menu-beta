#include "bar.hpp"
#include "ImGui/imgui.h"

extern bool menu_open;

namespace bar {

void render(float screen_w) {
    ImGui::SetNextWindowPos(ImVec2(screen_w / 2 - 50, 0));
    ImGui::SetNextWindowSize(ImVec2(100, 30));
    ImGui::Begin("##bar", nullptr,
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoScrollbar |
        ImGuiWindowFlags_NoBackground);

    if (ImGui::Button(menu_open ? "[-]" : "[+]", ImVec2(80, 22))) {
        menu_open = !menu_open;
    }

    ImGui::End();
}

} // namespace bar
