// dear imgui: Minimalistic platform binding for UWP
// This needs to be used along with a Renderer (e.g. DirectX11)

// Implemented features:
//  [ ] Platform: Clipboard support (for Win32 this is actually part of core dear imgui)
//  [ ] Platform: Mouse cursor shape and visibility. Disable with 'io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange'.
//  [ ] Platform: Keyboard arrays indexed using VK_* Virtual Key Codes, e.g. ImGui::IsKeyPressed(VK_SPACE).
//  [ ] Platform: Gamepad support. Enabled with 'io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad'.

#pragma once
#include "imgui.h"      // IMGUI_IMPL_API

IMGUI_IMPL_API bool     ImGui_ImplUWP_Init();
IMGUI_IMPL_API void     ImGui_ImplUWP_Shutdown();
IMGUI_IMPL_API void     ImGui_ImplUWP_NewFrame(int width, int height);