#include "menu.hpp"
#include "theme/theme.hpp"
#include "ImGui/imgui.h"

extern bool esp_enabled;
extern bool esp_box;
extern bool esp_line;
extern bool esp_health;
extern bool esp_name;
extern bool menu_open;

namespace menu {

static int current_tab = 0;

void render() {
    if (!menu_open) return;

    ImGui::SetNextWindowSize(ImVec2(340, 400), ImGuiCond_FirstUseEver);
    ImGui::Begin("Standoff 2", &menu_open, ImGuiWindowFlags_NoCollapse);

    const char* tabs[] = {"ESP", "Misc"};
    for (int i = 0; i < 2; i++) {
        if (i > 0) ImGui::SameLine();
        bool selected = (current_tab == i);
        if (selected) ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyleColorVec4(ImGuiCol_ButtonActive));
        if (ImGui::Button(tabs[i], ImVec2(100, 30))) current_tab = i;
        if (selected) ImGui::PopStyleColor();
    }

    ImGui::Separator();

    switch (current_tab) {
        case 0: // ESP
            ImGui::Checkbox("ESP enabled", &esp_enabled);
            if (esp_enabled) {
                ImGui::Checkbox("Box", &esp_box);
                ImGui::Checkbox("Snapline", &esp_line);
                ImGui::Checkbox("Health bar", &esp_health);
                ImGui::Checkbox("Distance", &esp_name);
            }
            break;

        case 1: // Misc
            ImGui::Text("Standoff 2 ESP");
            ImGui::Text("pid: %d", 0); // заменится на реальный
            ImGui::Text("il2cpp: 0x%lx", 0UL);
            ImGui::Separator();
            ImGui::TextWrapped(
                "Offsets = 0x0 -> not filled.\n"
                "Dump libil2cpp.so through\n"
                "il2cppdumper and fill offsets\n"
                "in src/game/game.hpp"
            );
            break;
    }

    ImGui::End();
}

} // namespace menu
