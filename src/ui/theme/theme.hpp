#pragma once
#include "ImGui/imgui.h"

namespace theme {

inline void apply() {
    ImGuiStyle& s = ImGui::GetStyle();
    s.WindowRounding = 8.f;
    s.FrameRounding = 4.f;
    s.GrabRounding = 4.f;
    s.ScrollbarRounding = 4.f;
    s.WindowPadding = ImVec2(12, 12);
    s.FramePadding = ImVec2(8, 4);
    s.ItemSpacing = ImVec2(8, 6);

    ImVec4* c = s.Colors;
    c[ImGuiCol_WindowBg]        = ImVec4(0.08f, 0.08f, 0.10f, 0.92f);
    c[ImGuiCol_Border]          = ImVec4(0.30f, 0.30f, 0.35f, 0.50f);
    c[ImGuiCol_FrameBg]         = ImVec4(0.15f, 0.15f, 0.18f, 1.00f);
    c[ImGuiCol_FrameBgHovered]  = ImVec4(0.22f, 0.22f, 0.27f, 1.00f);
    c[ImGuiCol_FrameBgActive]   = ImVec4(0.28f, 0.28f, 0.33f, 1.00f);
    c[ImGuiCol_TitleBg]         = ImVec4(0.10f, 0.10f, 0.12f, 1.00f);
    c[ImGuiCol_TitleBgActive]   = ImVec4(0.14f, 0.14f, 0.18f, 1.00f);
    c[ImGuiCol_CheckMark]       = ImVec4(0.40f, 0.80f, 1.00f, 1.00f);
    c[ImGuiCol_SliderGrab]      = ImVec4(0.40f, 0.80f, 1.00f, 1.00f);
    c[ImGuiCol_SliderGrabActive]= ImVec4(0.50f, 0.90f, 1.00f, 1.00f);
    c[ImGuiCol_Button]          = ImVec4(0.18f, 0.18f, 0.22f, 1.00f);
    c[ImGuiCol_ButtonHovered]   = ImVec4(0.28f, 0.50f, 0.70f, 1.00f);
    c[ImGuiCol_ButtonActive]    = ImVec4(0.30f, 0.60f, 0.85f, 1.00f);
    c[ImGuiCol_Header]          = ImVec4(0.22f, 0.22f, 0.27f, 1.00f);
    c[ImGuiCol_HeaderHovered]   = ImVec4(0.30f, 0.50f, 0.70f, 1.00f);
    c[ImGuiCol_HeaderActive]    = ImVec4(0.35f, 0.60f, 0.85f, 1.00f);
    c[ImGuiCol_Text]            = ImVec4(0.95f, 0.95f, 0.95f, 1.00f);
}

} // namespace theme
