#ifndef platformUtility_hpp
#define platformUtility_hpp

#include "InputEvent.hpp"

#include <winrt/windows.system.h>

namespace Utility
{
    namespace Input {

        constexpr winrt::Windows::System::VirtualKey convertKeyToUWP(EngineCore::Common::Input::KeyboardKeys key)
            {
                switch (key)
                {
                case EngineCore::Common::Input::KEY_UNKNOWN:
                    return winrt::Windows::System::VirtualKey::None;
                    break;
                case EngineCore::Common::Input::KEY_SPACE:
                    return winrt::Windows::System::VirtualKey::Space;
                    break;
                case EngineCore::Common::Input::KEY_0:
                    return winrt::Windows::System::VirtualKey::Number0;
                    break;
                case EngineCore::Common::Input::KEY_1:
                    return winrt::Windows::System::VirtualKey::Number1;
                    break;
                case EngineCore::Common::Input::KEY_2:
                    return winrt::Windows::System::VirtualKey::Number2;
                    break;
                case EngineCore::Common::Input::KEY_3:
                    return winrt::Windows::System::VirtualKey::Number3;
                    break;
                case EngineCore::Common::Input::KEY_4:
                    return winrt::Windows::System::VirtualKey::Number4;
                    break;
                case EngineCore::Common::Input::KEY_5:
                    return winrt::Windows::System::VirtualKey::Number5;
                    break;
                case EngineCore::Common::Input::KEY_6:
                    return winrt::Windows::System::VirtualKey::Number6;
                    break;
                case EngineCore::Common::Input::KEY_7:
                    return winrt::Windows::System::VirtualKey::Number7;
                    break;
                case EngineCore::Common::Input::KEY_8:
                    return winrt::Windows::System::VirtualKey::Number8;
                    break;
                case EngineCore::Common::Input::KEY_9:
                    return winrt::Windows::System::VirtualKey::Number9;
                    break;
                case EngineCore::Common::Input::KEY_A:
                    return winrt::Windows::System::VirtualKey::A;
                    break;
                case EngineCore::Common::Input::KEY_B:
                    return winrt::Windows::System::VirtualKey::B;
                    break;
                case EngineCore::Common::Input::KEY_C:
                    return winrt::Windows::System::VirtualKey::C;
                    break;
                case EngineCore::Common::Input::KEY_D:
                    return winrt::Windows::System::VirtualKey::D;
                    break;
                case EngineCore::Common::Input::KEY_E:
                    return winrt::Windows::System::VirtualKey::E;
                    break;
                case EngineCore::Common::Input::KEY_F:
                    return winrt::Windows::System::VirtualKey::F;
                    break;
                case EngineCore::Common::Input::KEY_G:
                    return winrt::Windows::System::VirtualKey::G;
                    break;
                case EngineCore::Common::Input::KEY_H:
                    return winrt::Windows::System::VirtualKey::H;
                    break;
                case EngineCore::Common::Input::KEY_I:
                    return winrt::Windows::System::VirtualKey::I;
                    break;
                case EngineCore::Common::Input::KEY_J:
                    return winrt::Windows::System::VirtualKey::J;
                    break;
                case EngineCore::Common::Input::KEY_K:
                    return winrt::Windows::System::VirtualKey::K;
                    break;
                case EngineCore::Common::Input::KEY_L:
                    return winrt::Windows::System::VirtualKey::L;
                    break;
                case EngineCore::Common::Input::KEY_M:
                    return winrt::Windows::System::VirtualKey::M;
                    break;
                case EngineCore::Common::Input::KEY_N:
                    return winrt::Windows::System::VirtualKey::N;
                    break;
                case EngineCore::Common::Input::KEY_O:
                    return winrt::Windows::System::VirtualKey::O;
                    break;
                case EngineCore::Common::Input::KEY_P:
                    return winrt::Windows::System::VirtualKey::P;
                    break;
                case EngineCore::Common::Input::KEY_Q:
                    return winrt::Windows::System::VirtualKey::Q;
                    break;
                case EngineCore::Common::Input::KEY_R:
                    return winrt::Windows::System::VirtualKey::R;
                    break;
                case EngineCore::Common::Input::KEY_S:
                    return winrt::Windows::System::VirtualKey::S;
                    break;
                case EngineCore::Common::Input::KEY_T:
                    return winrt::Windows::System::VirtualKey::T;
                    break;
                case EngineCore::Common::Input::KEY_U:
                    return winrt::Windows::System::VirtualKey::U;
                    break;
                case EngineCore::Common::Input::KEY_V:
                    return winrt::Windows::System::VirtualKey::V;
                    break;
                case EngineCore::Common::Input::KEY_W:
                    return winrt::Windows::System::VirtualKey::W;
                    break;
                case EngineCore::Common::Input::KEY_X:
                    return winrt::Windows::System::VirtualKey::X;
                    break;
                case EngineCore::Common::Input::KEY_Y:
                    return winrt::Windows::System::VirtualKey::Y;
                    break;
                case EngineCore::Common::Input::KEY_Z:
                    return winrt::Windows::System::VirtualKey::Z;
                    break;
                case EngineCore::Common::Input::KEY_ESCAPE:
                    return winrt::Windows::System::VirtualKey::Escape;
                    break;
                case EngineCore::Common::Input::KEY_ENTER:
                    return winrt::Windows::System::VirtualKey::Enter;
                    break;
                case EngineCore::Common::Input::KEY_TAB:
                    return winrt::Windows::System::VirtualKey::Tab;
                    break;
                case EngineCore::Common::Input::KEY_INSERT:
                    return winrt::Windows::System::VirtualKey::Insert;
                    break;
                case EngineCore::Common::Input::KEY_DELETE:
                    return winrt::Windows::System::VirtualKey::Delete;
                    break;
                case EngineCore::Common::Input::KEY_RIGHT:
                    return winrt::Windows::System::VirtualKey::Right;
                    break;
                case EngineCore::Common::Input::KEY_LEFT:
                    return winrt::Windows::System::VirtualKey::Left;
                    break;
                case EngineCore::Common::Input::KEY_DOWN:
                    return winrt::Windows::System::VirtualKey::Down;
                    break;
                case EngineCore::Common::Input::KEY_UP:
                    return winrt::Windows::System::VirtualKey::Up;
                    break;
                case EngineCore::Common::Input::KEY_PAGE_UP:
                    return winrt::Windows::System::VirtualKey::PageUp;
                    break;
                case EngineCore::Common::Input::KEY_PAGE_DOWN:
                    return winrt::Windows::System::VirtualKey::PageDown;
                    break;
                case EngineCore::Common::Input::KEY_HOME:
                    return winrt::Windows::System::VirtualKey::Home;
                    break;
                case EngineCore::Common::Input::KEY_END:
                    return winrt::Windows::System::VirtualKey::End;
                    break;
                case EngineCore::Common::Input::KEY_SCROLL_LOCK:
                    return winrt::Windows::System::VirtualKey::Scroll;
                    break;
                case EngineCore::Common::Input::KEY_NUM_LOCK:
                    return winrt::Windows::System::VirtualKey::NumberKeyLock;
                    break;
                case EngineCore::Common::Input::KEY_PRINT_SCREEN:
                    return winrt::Windows::System::VirtualKey::Print;
                    break;
                case EngineCore::Common::Input::KEY_PAUSE:
                    return winrt::Windows::System::VirtualKey::Pause;
                    break;
                case EngineCore::Common::Input::KEY_F1:
                    return winrt::Windows::System::VirtualKey::F1;
                    break;
                case EngineCore::Common::Input::KEY_F2:
                    return winrt::Windows::System::VirtualKey::F2;
                    break;
                case EngineCore::Common::Input::KEY_F3:
                    return winrt::Windows::System::VirtualKey::F3;
                    break;
                case EngineCore::Common::Input::KEY_F4:
                    return winrt::Windows::System::VirtualKey::F4;
                    break;
                case EngineCore::Common::Input::KEY_F5:
                    return winrt::Windows::System::VirtualKey::F5;
                    break;
                case EngineCore::Common::Input::KEY_F6:
                    return winrt::Windows::System::VirtualKey::F6;
                    break;
                case EngineCore::Common::Input::KEY_F7:
                    return winrt::Windows::System::VirtualKey::F7;
                    break;
                case EngineCore::Common::Input::KEY_F8:
                    return winrt::Windows::System::VirtualKey::F8;
                    break;
                case EngineCore::Common::Input::KEY_F9:
                    return winrt::Windows::System::VirtualKey::F9;
                    break;
                case EngineCore::Common::Input::KEY_F10:
                    return winrt::Windows::System::VirtualKey::F10;
                    break;
                case EngineCore::Common::Input::KEY_F11:
                    return winrt::Windows::System::VirtualKey::F11;
                    break;
                case EngineCore::Common::Input::KEY_F12:
                    return winrt::Windows::System::VirtualKey::F12;
                    break;
                case EngineCore::Common::Input::KEY_F13:
                    return winrt::Windows::System::VirtualKey::F13;
                    break;
                case EngineCore::Common::Input::KEY_F14:
                    return winrt::Windows::System::VirtualKey::F14;
                    break;
                case EngineCore::Common::Input::KEY_F15:
                    return winrt::Windows::System::VirtualKey::F15;
                    break;
                case EngineCore::Common::Input::KEY_F16:
                    return winrt::Windows::System::VirtualKey::F16;
                    break;
                case EngineCore::Common::Input::KEY_F17:
                    return winrt::Windows::System::VirtualKey::F17;
                    break;
                case EngineCore::Common::Input::KEY_F18:
                    return winrt::Windows::System::VirtualKey::F18;
                    break;
                case EngineCore::Common::Input::KEY_F19:
                    return winrt::Windows::System::VirtualKey::F19;
                    break;
                case EngineCore::Common::Input::KEY_F20:
                    return winrt::Windows::System::VirtualKey::F20;
                    break;
                case EngineCore::Common::Input::KEY_F21:
                    return winrt::Windows::System::VirtualKey::F21;
                    break;
                case EngineCore::Common::Input::KEY_F22:
                    return winrt::Windows::System::VirtualKey::F22;
                    break;
                case EngineCore::Common::Input::KEY_F23:
                    return winrt::Windows::System::VirtualKey::F23;
                    break;
                case EngineCore::Common::Input::KEY_F24:
                    return winrt::Windows::System::VirtualKey::F24;
                    break;
                case EngineCore::Common::Input::KEY_KP_0:
                    return winrt::Windows::System::VirtualKey::NumberPad0;
                    break;
                case EngineCore::Common::Input::KEY_KP_1:
                    return winrt::Windows::System::VirtualKey::NumberPad1;
                    break;
                case EngineCore::Common::Input::KEY_KP_2:
                    return winrt::Windows::System::VirtualKey::NumberPad2;
                    break;
                case EngineCore::Common::Input::KEY_KP_3:
                    return winrt::Windows::System::VirtualKey::NumberPad3;
                    break;
                case EngineCore::Common::Input::KEY_KP_4:
                    return winrt::Windows::System::VirtualKey::NumberPad4;
                    break;
                case EngineCore::Common::Input::KEY_KP_5:
                    return winrt::Windows::System::VirtualKey::NumberPad5;
                    break;
                case EngineCore::Common::Input::KEY_KP_6:
                    return winrt::Windows::System::VirtualKey::NumberPad6;
                    break;
                case EngineCore::Common::Input::KEY_KP_7:
                    return winrt::Windows::System::VirtualKey::NumberPad7;
                    break;
                case EngineCore::Common::Input::KEY_KP_8:
                    return winrt::Windows::System::VirtualKey::NumberPad8;
                    break;
                case EngineCore::Common::Input::KEY_KP_9:
                    return winrt::Windows::System::VirtualKey::NumberPad9;
                    break;
                case EngineCore::Common::Input::KEY_LEFT_SHIFT:
                    return winrt::Windows::System::VirtualKey::LeftShift;
                    break;
                case EngineCore::Common::Input::KEY_LEFT_CONTROL:
                    return winrt::Windows::System::VirtualKey::LeftControl;
                    break;
                case EngineCore::Common::Input::KEY_RIGHT_SHIFT:
                    return winrt::Windows::System::VirtualKey::RightShift;
                    break;
                case EngineCore::Common::Input::KEY_RIGHT_CONTROL:
                    return winrt::Windows::System::VirtualKey::RightControl;
                    break;
                case EngineCore::Common::Input::KEY_MENU:
                    return winrt::Windows::System::VirtualKey::Menu;
                    break;
                default:
                    return winrt::Windows::System::VirtualKey::None;
                    break;
                }
            };

    }
}

#endif // !platformUtility_hpp
