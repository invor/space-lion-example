// dear imgui: Minimalistic platform Binding for UWP
// This needs to be used along with a Renderer (e.g. DirectX11)

#include "imgui.h"
#include "imgui_impl_uwp.h"
#include <windows.h>
#include <tchar.h>

// Win32 Data
static INT64                g_Time = 0;
static INT64                g_TicksPerSecond = 0;
static ImGuiMouseCursor     g_LastMouseCursor = ImGuiMouseCursor_COUNT;
static bool                 g_HasGamepad = false;
static bool                 g_WantUpdateHasGamepad = true;

// Functions
bool ImGui_ImplUWP_Init()
{
    if (!::QueryPerformanceFrequency((LARGE_INTEGER*)&g_TicksPerSecond))
        return false;
    if (!::QueryPerformanceCounter((LARGE_INTEGER*)&g_Time))
        return false;

    // Setup back-end capabilities flags
    ImGuiIO& io = ImGui::GetIO();
    io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;         // We can honor GetMouseCursor() values (optional)
    io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;          // We can honor io.WantSetMousePos requests (optional, rarely used)
    io.BackendPlatformName = "imgui_impl_uwp";
    //io.ImeWindowHandle = hwnd;

    // Keyboard mapping. ImGui will use those indices to peek into the io.KeysDown[] array that we will update during the application lifetime.
    io.KeyMap[ImGuiKey_Tab] = VK_TAB;
    io.KeyMap[ImGuiKey_LeftArrow] = VK_LEFT;
    io.KeyMap[ImGuiKey_RightArrow] = VK_RIGHT;
    io.KeyMap[ImGuiKey_UpArrow] = VK_UP;
    io.KeyMap[ImGuiKey_DownArrow] = VK_DOWN;
    io.KeyMap[ImGuiKey_PageUp] = VK_PRIOR;
    io.KeyMap[ImGuiKey_PageDown] = VK_NEXT;
    io.KeyMap[ImGuiKey_Home] = VK_HOME;
    io.KeyMap[ImGuiKey_End] = VK_END;
    io.KeyMap[ImGuiKey_Insert] = VK_INSERT;
    io.KeyMap[ImGuiKey_Delete] = VK_DELETE;
    io.KeyMap[ImGuiKey_Backspace] = VK_BACK;
    io.KeyMap[ImGuiKey_Space] = VK_SPACE;
    io.KeyMap[ImGuiKey_Enter] = VK_RETURN;
    io.KeyMap[ImGuiKey_Escape] = VK_ESCAPE;
    io.KeyMap[ImGuiKey_KeyPadEnter] = VK_RETURN;
    io.KeyMap[ImGuiKey_A] = 'A';
    io.KeyMap[ImGuiKey_C] = 'C';
    io.KeyMap[ImGuiKey_V] = 'V';
    io.KeyMap[ImGuiKey_X] = 'X';
    io.KeyMap[ImGuiKey_Y] = 'Y';
    io.KeyMap[ImGuiKey_Z] = 'Z';

    return true;
}

void ImGui_ImplUWP_Shutdown()
{
}

//      static bool ImGui_ImplUWP_UpdateMouseCursor()
//      {
//          ImGuiIO& io = ImGui::GetIO();
//          if (io.ConfigFlags & ImGuiConfigFlags_NoMouseCursorChange)
//              return false;
//      
//          ImGuiMouseCursor imgui_cursor = ImGui::GetMouseCursor();
//          if (imgui_cursor == ImGuiMouseCursor_None || io.MouseDrawCursor)
//          {
//              // Hide OS mouse cursor if imgui is drawing it or if it wants no cursor
//              ::SetCursor(NULL);
//          }
//          else
//          {
//              // Show OS mouse cursor
//              LPTSTR win32_cursor = IDC_ARROW;
//              switch (imgui_cursor)
//              {
//              case ImGuiMouseCursor_Arrow:        win32_cursor = IDC_ARROW; break;
//              case ImGuiMouseCursor_TextInput:    win32_cursor = IDC_IBEAM; break;
//              case ImGuiMouseCursor_ResizeAll:    win32_cursor = IDC_SIZEALL; break;
//              case ImGuiMouseCursor_ResizeEW:     win32_cursor = IDC_SIZEWE; break;
//              case ImGuiMouseCursor_ResizeNS:     win32_cursor = IDC_SIZENS; break;
//              case ImGuiMouseCursor_ResizeNESW:   win32_cursor = IDC_SIZENESW; break;
//              case ImGuiMouseCursor_ResizeNWSE:   win32_cursor = IDC_SIZENWSE; break;
//              case ImGuiMouseCursor_Hand:         win32_cursor = IDC_HAND; break;
//              case ImGuiMouseCursor_NotAllowed:   win32_cursor = IDC_NO; break;
//              }
//              ::SetCursor(::LoadCursor(NULL, win32_cursor));
//          }
//          return true;
//      }
//      
//      static void ImGui_ImplUWP_UpdateMousePos()
//      {
//          ImGuiIO& io = ImGui::GetIO();
//      
//          // Set OS mouse position if requested (rarely used, only when ImGuiConfigFlags_NavEnableSetMousePos is enabled by user)
//          if (io.WantSetMousePos)
//          {
//              POINT pos = { (int)io.MousePos.x, (int)io.MousePos.y };
//              ::ClientToScreen(g_hWnd, &pos);
//              ::SetCursorPos(pos.x, pos.y);
//          }
//      
//          // Set mouse position
//          io.MousePos = ImVec2(-FLT_MAX, -FLT_MAX);
//          POINT pos;
//          if (HWND active_window = ::GetForegroundWindow())
//              if (active_window == g_hWnd || ::IsChild(active_window, g_hWnd))
//                  if (::GetCursorPos(&pos) && ::ScreenToClient(g_hWnd, &pos))
//                      io.MousePos = ImVec2((float)pos.x, (float)pos.y);
//      }
//      
//      // Gamepad navigation mapping
//      static void ImGui_ImplUWP_UpdateGamepads()
//      {
//      #ifndef IMGUI_IMPL_WIN32_DISABLE_GAMEPAD
//          ImGuiIO& io = ImGui::GetIO();
//          memset(io.NavInputs, 0, sizeof(io.NavInputs));
//          if ((io.ConfigFlags & ImGuiConfigFlags_NavEnableGamepad) == 0)
//              return;
//      
//          // Calling XInputGetState() every frame on disconnected gamepads is unfortunately too slow.
//          // Instead we refresh gamepad availability by calling XInputGetCapabilities() _only_ after receiving WM_DEVICECHANGE.
//          if (g_WantUpdateHasGamepad)
//          {
//              XINPUT_CAPABILITIES caps;
//              g_HasGamepad = (XInputGetCapabilities(0, XINPUT_FLAG_GAMEPAD, &caps) == ERROR_SUCCESS);
//              g_WantUpdateHasGamepad = false;
//          }
//      
//          XINPUT_STATE xinput_state;
//          io.BackendFlags &= ~ImGuiBackendFlags_HasGamepad;
//          if (g_HasGamepad && XInputGetState(0, &xinput_state) == ERROR_SUCCESS)
//          {
//              const XINPUT_GAMEPAD& gamepad = xinput_state.Gamepad;
//              io.BackendFlags |= ImGuiBackendFlags_HasGamepad;
//      
//      #define MAP_BUTTON(NAV_NO, BUTTON_ENUM)     { io.NavInputs[NAV_NO] = (gamepad.wButtons & BUTTON_ENUM) ? 1.0f : 0.0f; }
//      #define MAP_ANALOG(NAV_NO, VALUE, V0, V1)   { float vn = (float)(VALUE - V0) / (float)(V1 - V0); if (vn > 1.0f) vn = 1.0f; if (vn > 0.0f && io.NavInputs[NAV_NO] < vn) io.NavInputs[NAV_NO] = vn; }
//              MAP_BUTTON(ImGuiNavInput_Activate, XINPUT_GAMEPAD_A);              // Cross / A
//              MAP_BUTTON(ImGuiNavInput_Cancel, XINPUT_GAMEPAD_B);              // Circle / B
//              MAP_BUTTON(ImGuiNavInput_Menu, XINPUT_GAMEPAD_X);              // Square / X
//              MAP_BUTTON(ImGuiNavInput_Input, XINPUT_GAMEPAD_Y);              // Triangle / Y
//              MAP_BUTTON(ImGuiNavInput_DpadLeft, XINPUT_GAMEPAD_DPAD_LEFT);      // D-Pad Left
//              MAP_BUTTON(ImGuiNavInput_DpadRight, XINPUT_GAMEPAD_DPAD_RIGHT);     // D-Pad Right
//              MAP_BUTTON(ImGuiNavInput_DpadUp, XINPUT_GAMEPAD_DPAD_UP);        // D-Pad Up
//              MAP_BUTTON(ImGuiNavInput_DpadDown, XINPUT_GAMEPAD_DPAD_DOWN);      // D-Pad Down
//              MAP_BUTTON(ImGuiNavInput_FocusPrev, XINPUT_GAMEPAD_LEFT_SHOULDER);  // L1 / LB
//              MAP_BUTTON(ImGuiNavInput_FocusNext, XINPUT_GAMEPAD_RIGHT_SHOULDER); // R1 / RB
//              MAP_BUTTON(ImGuiNavInput_TweakSlow, XINPUT_GAMEPAD_LEFT_SHOULDER);  // L1 / LB
//              MAP_BUTTON(ImGuiNavInput_TweakFast, XINPUT_GAMEPAD_RIGHT_SHOULDER); // R1 / RB
//              MAP_ANALOG(ImGuiNavInput_LStickLeft, gamepad.sThumbLX, -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE, -32768);
//              MAP_ANALOG(ImGuiNavInput_LStickRight, gamepad.sThumbLX, +XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE, +32767);
//              MAP_ANALOG(ImGuiNavInput_LStickUp, gamepad.sThumbLY, +XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE, +32767);
//              MAP_ANALOG(ImGuiNavInput_LStickDown, gamepad.sThumbLY, -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE, -32767);
//      #undef MAP_BUTTON
//      #undef MAP_ANALOG
//          }
//      #endif // #ifndef IMGUI_IMPL_WIN32_DISABLE_GAMEPAD
//      }

void ImGui_ImplUWP_NewFrame(int width, int height)
{
    ImGuiIO& io = ImGui::GetIO();
    IM_ASSERT(io.Fonts->IsBuilt() && "Font atlas not built! It is generally built by the renderer back-end. Missing call to renderer _NewFrame() function? e.g. ImGui_ImplOpenGL3_NewFrame().");

    // Setup display size (every frame to accommodate for window resizing)
    io.DisplaySize = ImVec2((float)(width), (float)(height));

    // Setup time step
    INT64 current_time;
    ::QueryPerformanceCounter((LARGE_INTEGER*)&current_time);
    io.DeltaTime = (float)(current_time - g_Time) / g_TicksPerSecond;
    g_Time = current_time;

    // Read keyboard modifiers inputs
    //io.KeyCtrl = (::GetKeyState(VK_CONTROL) & 0x8000) != 0;
    //io.KeyShift = (::GetKeyState(VK_SHIFT) & 0x8000) != 0;
    //io.KeyAlt = (::GetKeyState(VK_MENU) & 0x8000) != 0;
    //io.KeySuper = false;
    // io.KeysDown[], io.MousePos, io.MouseDown[], io.MouseWheel: filled by the WndProc handler below.

    // Update OS mouse position
    //ImGui_ImplUWP_UpdateMousePos();

    // Update OS mouse cursor with the cursor requested by imgui
    ImGuiMouseCursor mouse_cursor = io.MouseDrawCursor ? ImGuiMouseCursor_None : ImGui::GetMouseCursor();
    if (g_LastMouseCursor != mouse_cursor)
    {
        g_LastMouseCursor = mouse_cursor;
        //ImGui_ImplUWP_UpdateMouseCursor();
    }

    // Update game controllers (if enabled and available)
    //ImGui_ImplUWP_UpdateGamepads();
}
