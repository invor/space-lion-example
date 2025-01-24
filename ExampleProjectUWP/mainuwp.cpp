#include "CameraController.hpp"
#include "DemoScene.hpp"
#include "DirectXEngineFrontendUWP.hpp"
#include "pch.h"
#include "platformUtility.hpp"

#include <imgui.h>
#include <backends/imgui_impl_dx11.h>
#include "imgui_impl_uwp.h"

// Refer to https://github.com/walbourn/directx-vs-templates/tree/main/d3d11game_uwp_cppwinrt_dr
void GetWindowBounds(_In_::IUnknown* window, _Out_ RECT* rect);

namespace
{
    inline int ConvertDipsToPixels(float dips, float dpi) noexcept
    {
        return int(dips * dpi / 96.f + 0.5f);
    }

    inline float ConvertPixelsToDips(int pixels, float dpi) noexcept
    {
        return (float(pixels) * 96.f / dpi);
    }

    DXGI_MODE_ROTATION ComputeDisplayRotation(
        winrt::Windows::Graphics::Display::DisplayOrientations	native_orientation,
        winrt::Windows::Graphics::Display::DisplayOrientations	current_orientation) noexcept
    {
        DXGI_MODE_ROTATION rotation = DXGI_MODE_ROTATION_UNSPECIFIED;

        switch (native_orientation)
        {
        case winrt::Windows::Graphics::Display::DisplayOrientations::Landscape:
            switch (current_orientation)
            {
            case winrt::Windows::Graphics::Display::DisplayOrientations::Landscape:
                rotation = DXGI_MODE_ROTATION_IDENTITY;
                break;

            case winrt::Windows::Graphics::Display::DisplayOrientations::Portrait:
                rotation = DXGI_MODE_ROTATION_ROTATE270;
                break;

            case winrt::Windows::Graphics::Display::DisplayOrientations::LandscapeFlipped:
                rotation = DXGI_MODE_ROTATION_ROTATE180;
                break;

            case winrt::Windows::Graphics::Display::DisplayOrientations::PortraitFlipped:
                rotation = DXGI_MODE_ROTATION_ROTATE90;
                break;

            default:
                break;
            }
            break;

        case winrt::Windows::Graphics::Display::DisplayOrientations::Portrait:
            switch (current_orientation)
            {
            case winrt::Windows::Graphics::Display::DisplayOrientations::Landscape:
                rotation = DXGI_MODE_ROTATION_ROTATE90;
                break;

            case winrt::Windows::Graphics::Display::DisplayOrientations::Portrait:
                rotation = DXGI_MODE_ROTATION_IDENTITY;
                break;

            case winrt::Windows::Graphics::Display::DisplayOrientations::LandscapeFlipped:
                rotation = DXGI_MODE_ROTATION_ROTATE270;
                break;

            case winrt::Windows::Graphics::Display::DisplayOrientations::PortraitFlipped:
                rotation = DXGI_MODE_ROTATION_ROTATE180;
                break;

            default:
                break;
            }
            break;

        default:
            break;
        }

        return rotation;
    }
}

struct App : winrt::implements<App, winrt::Windows::ApplicationModel::Core::IFrameworkViewSource, winrt::Windows::ApplicationModel::Core::IFrameworkView>
{
    winrt::Windows::ApplicationModel::Core::IFrameworkView CreateView()
    {
        return *this;
    }

    void Initialize(winrt::Windows::ApplicationModel::Core::CoreApplicationView const& application_view) 
    {
        application_view.Activated({ this, &App::OnActivated });

        winrt::Windows::ApplicationModel::Core::CoreApplication::Suspending({ this, &App::OnSuspending });

        winrt::Windows::ApplicationModel::Core::CoreApplication::Resuming({ this, &App::OnResuming });

        // At this point we have access to the device. 
        // We can create the device-dependent resources.
        m_device_resources = std::make_shared<DX::DeviceResources>();
        m_engine_frontend = std::make_unique<DirectXEngineFrontendUWP>(m_device_resources);
    }

    void Uninitialize()
    {
        m_engine_frontend.reset();
    }

    void SetWindow(winrt::Windows::UI::Core::CoreWindow const& window)
    {
        window.SizeChanged({ this, &App::OnWindowSizeChanged });

        try
        {
            window.ResizeStarted([this](auto&&, auto&&) { m_in_sizemove = true; });

            window.ResizeCompleted([this](auto&&, auto&&) { m_in_sizemove = false; HandleWindowSizeChanged(); });
        }
        catch (...)
        {
            // Requires Windows 10 Creators Update (10.0.15063) or later
        }

        window.VisibilityChanged({ this, &App::OnVisibilityChanged });

        window.Closed([this](auto&&, auto&&) { m_exit = true; });

        auto dispatcher = winrt::Windows::UI::Core::CoreWindow::GetForCurrentThread().Dispatcher();

        dispatcher.AcceleratorKeyActivated({ this, &App::OnAcceleratorKeyActivated });

        auto navigation = winrt::Windows::UI::Core::SystemNavigationManager::GetForCurrentView();

        // UWP on Xbox One triggers a back request whenever the B button is pressed
        // which can result in the app being suspended if unhandled
        navigation.BackRequested([](
            const winrt::Windows::Foundation::IInspectable&,
            const winrt::Windows::UI::Core::BackRequestedEventArgs& args)
            {
                args.Handled(true);
            });

        auto currentDisplayInformation = winrt::Windows::Graphics::Display::DisplayInformation::GetForCurrentView();

        currentDisplayInformation.DpiChanged({ this, &App::OnDpiChanged });

        currentDisplayInformation.OrientationChanged({ this, &App::OnOrientationChanged });

        winrt::Windows::Graphics::Display::DisplayInformation::DisplayContentsInvalidated({ this, &App::OnDisplayContentsInvalidated });

        m_DPI = currentDisplayInformation.LogicalDpi();

        m_logical_width = window.Bounds().Width;
        m_logical_height = window.Bounds().Height;

        m_native_orientation = currentDisplayInformation.NativeOrientation();
        m_current_orientation = currentDisplayInformation.CurrentOrientation();

        m_pixel_width = ConvertDipsToPixels(m_logical_width, m_DPI);
        m_pixel_height = ConvertDipsToPixels(m_logical_height, m_DPI);

        DXGI_MODE_ROTATION rotation = ComputeDisplayRotation(m_native_orientation, m_current_orientation);

        if (rotation == DXGI_MODE_ROTATION_ROTATE90 || rotation == DXGI_MODE_ROTATION_ROTATE270)
        {
            std::swap(m_pixel_width, m_pixel_height);
        }

        auto windowPtr = winrt::get_unknown(window);

        m_engine_frontend->init(windowPtr, m_pixel_width, m_pixel_height, rotation);
    }

    void Load(winrt::hstring const& /* entryPoint */)
    {
    }

    void OnActivated(
        winrt::Windows::ApplicationModel::Core::CoreApplicationView const& /* applicationView */,
        winrt::Windows::ApplicationModel::Activation::IActivatedEventArgs const& /* args */)
    {
        winrt::Windows::UI::Core::CoreWindow window = winrt::Windows::UI::Core::CoreWindow::GetForCurrentThread();
        window.Activate();
    }

    void OnSuspending(IInspectable const& /* sender */, winrt::Windows::ApplicationModel::SuspendingEventArgs const& args) 
    {

    }

    void OnResuming(IInspectable const& /* sender */, IInspectable const& /* args */) 
    {

    }

    void OnWindowSizeChanged(
        winrt::Windows::UI::Core::CoreWindow const& sender,
        winrt::Windows::UI::Core::WindowSizeChangedEventArgs const& /*args*/)
    {
        m_logical_width = sender.Bounds().Width;
        m_logical_height = sender.Bounds().Height;

        if (m_in_sizemove)
            return;

        HandleWindowSizeChanged();
    }

    void OnVisibilityChanged(
        winrt::Windows::UI::Core::CoreWindow const& /*sender*/,
        winrt::Windows::UI::Core::VisibilityChangedEventArgs const& args)
    {
        m_visible = args.Visible();
        if (m_visible) {
            //m_game->OnActivated();
            //TODO
        }
        else {
            //m_game->OnDeactivated();
            //TODO
        }
    }

    void OnAcceleratorKeyActivated(
        winrt::Windows::UI::Core::CoreDispatcher const&,
        winrt::Windows::UI::Core::AcceleratorKeyEventArgs const& args)
    {
        if (args.EventType() == winrt::Windows::UI::Core::CoreAcceleratorKeyEventType::SystemKeyDown
            && args.VirtualKey() == winrt::Windows::System::VirtualKey::Enter
            && args.KeyStatus().IsMenuKeyDown
            && !args.KeyStatus().WasKeyDown)
        {
            // Implements the classic ALT+ENTER fullscreen toggle
            auto view = winrt::Windows::UI::ViewManagement::ApplicationView::GetForCurrentView();

            if (view.IsFullScreenMode())
                view.ExitFullScreenMode();
            else
                view.TryEnterFullScreenMode();

            args.Handled(true);
        }
    }

    void OnDpiChanged(
        winrt::Windows::Graphics::Display::DisplayInformation const& sender,
        IInspectable const& /*args*/)
    {
        m_DPI = sender.LogicalDpi();

        HandleWindowSizeChanged();
    }

    void OnOrientationChanged(
        winrt::Windows::Graphics::Display::DisplayInformation const& sender,
        IInspectable const& /*args*/)
    {
        auto resizeManager = winrt::Windows::UI::Core::CoreWindowResizeManager::GetForCurrentView();
        resizeManager.ShouldWaitForLayoutCompletion(true);

        m_current_orientation = sender.CurrentOrientation();

        HandleWindowSizeChanged();

        resizeManager.NotifyLayoutCompleted();
    }

    void OnDisplayContentsInvalidated(
        winrt::Windows::Graphics::Display::DisplayInformation const& /*sender*/,
        IInspectable const& /*args*/)
    {
        //m_game->ValidateDevice();
        //m_game->OnDisplayChange();
    }

    void Run()
    {
        auto game_update_loop = [this](std::shared_future<void> render_exec)
        {
            // inplace construct an input action context to test the new concept
            //  auto evt_func = [](EngineCore::Common::Input::Event const& evt, EngineCore::Common::Input::HardwareState const& state) {
            //      std::cout << "Paying respect to new input system" << "\n";
            //  };
            //  EngineCore::Common::Input::EventDrivenAction evt_action = { {EngineCore::Common::Input::Device::KEYBOARD,EngineCore::Common::Input::KeyboardKeys::KEY_F,EngineCore::Common::Input::EventTrigger::PRESS}, evt_func };
            //  EngineCore::Common::Input::InputActionContext input_context = { "test_input_context", true, {evt_action}, {} };
            //  m_input_action_contexts.push_back(input_context);

            auto& world_state = m_engine_frontend->accessWorldState();
            auto& resource_manager = m_engine_frontend->accessResourceManager();
            auto& frame_manager = m_engine_frontend->accessFrameManager();

            Editor::Controls::CameraController cam_ctrl(world_state);
            m_input_action_contexts.push_back(cam_ctrl.getKeyboardInputActionContext());
            m_input_action_contexts.push_back(cam_ctrl.getGamepadInputActionContext());
            //m_engine_frontend->addInputActionContext(cam_ctrl.getInputActionContext());
            createDemoScene(world_state, resource_manager);

            while (render_exec.wait_for(std::chrono::seconds(1)) != std::future_status::ready) {
                // do nothing so far...
            }
        };

        auto engine_update_loop = [this] (std::shared_future<void> render_exec)
        {
            size_t update_frameID = 0;
            auto t_0 = std::chrono::high_resolution_clock::now();
            auto t_1 = std::chrono::high_resolution_clock::now();

            auto render_exec_status = render_exec.wait_for(std::chrono::microseconds(0));
            while (render_exec_status != std::future_status::ready)
            {
                double dt = std::chrono::duration_cast<std::chrono::duration<double>>(t_1 - t_0).count();
                t_0 = std::chrono::high_resolution_clock::now();

                auto window_resolution = this->getWindowResolution();

                m_engine_frontend->update(update_frameID++, dt, std::get<0>(window_resolution), std::get<1>(window_resolution));

                render_exec_status = render_exec.wait_for(std::chrono::microseconds(0));

                t_1 = std::chrono::high_resolution_clock::now();
            }
        };

        auto engine_render_loop = [this]()
        {
            auto window = winrt::Windows::UI::Core::CoreWindow::GetForCurrentThread();

            //struct
            //    __declspec(uuid("45D64A29-A63E-4CB6-B498-5781D298CB4F"))
            //    __declspec(novtable)
            //    ICoreWindowInterop : public IUnknown
            //{
            //    virtual HRESULT STDMETHODCALLTYPE get_WindowHandle(HWND* hwnd) = 0;
            //    virtual HRESULT STDMETHODCALLTYPE put_MessageHandled(unsigned char value) = 0;
            //};
            //
            //winrt::com_ptr<ICoreWindowInterop> interop{};
            //winrt::check_hresult(winrt::get_unknown(window)->QueryInterface(interop.put()));
            //HWND hwnd{};
            //winrt::check_hresult(interop->get_WindowHandle(&hwnd));

            // Init ImGui
            // Because we mostly want to use it for passive display at the moment, we ignore the IO part
            ImGui::CreateContext();
            ImGuiIO& io = ImGui::GetIO(); (void)io;
            ImGui_ImplUWP_Init();
            ImGui_ImplDX11_Init(m_device_resources->GetD3DDevice(), m_device_resources->GetD3DDeviceContext());

            size_t render_frameID = 0;
            auto t_0 = std::chrono::high_resolution_clock::now();
            auto t_1 = std::chrono::high_resolution_clock::now();

            while (true)
            {
                auto window_resolution = this->getWindowResolution();

                ImGui_ImplDX11_NewFrame();
                ImGui_ImplUWP_NewFrame(std::get<0>(window_resolution), std::get<1>(window_resolution));
                ImGui::NewFrame();

                double dt = std::chrono::duration_cast<std::chrono::duration<double>>(t_1 - t_0).count();
                t_0 = std::chrono::high_resolution_clock::now();

                winrt::Windows::UI::Core::CoreWindow::GetForCurrentThread().Dispatcher().ProcessEvents(
                    winrt::Windows::UI::Core::CoreProcessEventsOption::ProcessAllIfPresent
                );

                processInputActions(dt);

                m_engine_frontend->render(render_frameID++, dt, std::get<0>(window_resolution), std::get<1>(window_resolution));

                ImGui::SetNextWindowPos(ImVec2(std::get<0>(window_resolution) - 375.0f, std::get<1>(window_resolution) - 100.0f));
                bool p_open = true;
                if (!ImGui::Begin("FPS", &p_open, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings))
                {
                    ImGui::End();
                    return;
                }
                ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
                ImGui::End();
                ImGui::Render();
                ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
                ImGui::EndFrame();

                m_device_resources->Present();

                t_1 = std::chrono::high_resolution_clock::now();
            }
        };

        // Deferred starting and running of rendering on current thread (window and context usually live here)
        auto engine_render_exec = std::async(std::launch::deferred, engine_render_loop).share();

        // Start and run engine update in seperate thread
        auto engine_update_exec = std::async(std::launch::async, engine_update_loop, engine_render_exec);

        // Start "game"
        auto game_update_exec = std::async(std::launch::async, game_update_loop, engine_render_exec);

        // trigger deferred start of render loop
        engine_render_exec.get();
    }

    std::pair<int, int> getWindowResolution() {
        //std::lock_guard<std::mutex> lk(m_window_mutex);
        return { m_pixel_width, m_pixel_height };
    }

    void processInputActions(float dt) {
        for (auto& input_context : m_input_action_contexts)
        {
            if (input_context.m_is_active)
            {
                for (auto& state_action : input_context.m_state_actions)
                {
                    std::vector<EngineCore::Common::Input::HardwareState> states(state_action.m_state_query.size());

                    size_t part_idx = 0;
                    for (auto& part : state_action.m_state_query)
                    {
                        if (std::get<0>(part) == EngineCore::Common::Input::Device::KEYBOARD)
                        {
                            auto key_state = winrt::Windows::UI::Core::CoreWindow::GetForCurrentThread().GetKeyState(Utility::Input::convertKeyToUWP(static_cast<EngineCore::Common::Input::KeyboardKeys>(std::get<1>(part))));

                            states[part_idx] = (static_cast<int>(key_state & winrt::Windows::UI::Core::CoreVirtualKeyStates::Down) > 0 ? 1.0f : 0.0);
                        }
                        else if (std::get<0>(part) == EngineCore::Common::Input::Device::MOUSE_AXES)
                        {
                            auto pointer_position = winrt::Windows::UI::Core::CoreWindow::GetForCurrentThread().PointerPosition();

                            if (std::get<1>(part) == EngineCore::Common::Input::MouseAxes::MOUSE_CURSOR_X)
                            {
                                states[part_idx] = pointer_position.X;
                            }
                            else if (std::get<1>(part) == EngineCore::Common::Input::MouseAxes::MOUSE_CURSOR_Y)
                            {
                                states[part_idx] = pointer_position.Y;
                            }
                        }
                        else if (std::get<0>(part) == EngineCore::Common::Input::Device::MOUSE_BUTTON)
                        {
                            if (std::get<1>(part) == EngineCore::Common::Input::MouseButtons::MOUSE_BUTTON_RIGHT)
                            {
                                auto key_state = winrt::Windows::UI::Core::CoreWindow::GetForCurrentThread().GetKeyState(winrt::Windows::System::VirtualKey::RightButton);
                                states[part_idx] = (static_cast<int>(key_state & winrt::Windows::UI::Core::CoreVirtualKeyStates::Down) > 0 ? 1.0f : 0.0);
                            }
                        }

                        if (std::get<0>(part) == EngineCore::Common::Input::Device::GAMEPAD_AXES)
                        {

                            auto game_pads = winrt::Windows::Gaming::Input::Gamepad::Gamepads();

                            if (game_pads.Size() > 0)
                            {
                                auto const& game_pad_reading = game_pads.GetAt(0).GetCurrentReading();

                                if (std::get<1>(part) == EngineCore::Common::Input::GamepadAxes::GAMEPAD_AXIS_LEFT_X) {
                                    states.emplace_back(game_pad_reading.LeftThumbstickX);
                                }
                                else if (std::get<1>(part) == EngineCore::Common::Input::GamepadAxes::GAMEPAD_AXIS_LEFT_Y) {
                                    states.emplace_back(game_pad_reading.LeftThumbstickY);
                                }
                                else if (std::get<1>(part) == EngineCore::Common::Input::GamepadAxes::GAMEPAD_AXIS_RIGHT_X) {
                                    states.emplace_back(game_pad_reading.RightThumbstickX);
                                }
                                else if (std::get<1>(part) == EngineCore::Common::Input::GamepadAxes::GAMEPAD_AXIS_RIGHT_Y) {
                                    states.emplace_back(game_pad_reading.RightThumbstickY);
                                }
                            }
                        }

                        ++part_idx;
                    }

                    if (!states.empty()) {
                        state_action.m_action(state_action.m_state_query, states, dt);
                    }
                }
            }
        }
    }

private:
    void HandleWindowSizeChanged()
    {
        m_pixel_width = ConvertDipsToPixels(m_logical_width, m_DPI);
        m_pixel_height = ConvertDipsToPixels(m_logical_height, m_DPI);

        DXGI_MODE_ROTATION rotation = ComputeDisplayRotation(m_current_orientation, m_native_orientation);

        if (rotation == DXGI_MODE_ROTATION_ROTATE90 || rotation == DXGI_MODE_ROTATION_ROTATE270)
        {
            std::swap(m_pixel_width, m_pixel_height);
        }

        m_device_resources->WindowSizeChanged(m_pixel_width, m_pixel_height, rotation);
        //m_game->OnWindowSizeChanged(outputWidth, outputHeight, rotation);
        //TODO
    }

    std::shared_ptr<DX::DeviceResources>      m_device_resources;
    std::unique_ptr<DirectXEngineFrontendUWP> m_engine_frontend;

    /** List of input contexts */
    std::list<EngineCore::Common::Input::InputActionContext> m_input_action_contexts;

    bool  m_exit;
    bool  m_visible;
    bool  m_in_sizemove;
    float m_DPI;
    float m_logical_width;
    float m_logical_height;

    int m_pixel_width;
    int m_pixel_height;

    winrt::Windows::Graphics::Display::DisplayOrientations m_native_orientation;
    winrt::Windows::Graphics::Display::DisplayOrientations m_current_orientation;
};

int __stdcall wWinMain(HINSTANCE, HINSTANCE, PWSTR, int)
{
    winrt::Windows::ApplicationModel::Core::CoreApplication::Run(winrt::make<App>());
}

// Window size helper
_Use_decl_annotations_
void GetWindowBounds(::IUnknown* window, RECT* rect)
{
    if (!rect)
        return;

    *rect = {};

    if (!window)
        return;

    winrt::Windows::UI::Core::CoreWindow cw{ nullptr };
    try {
        winrt::check_hresult(window->QueryInterface(winrt::guid_of<winrt::Windows::UI::Core::CoreWindow>(), winrt::put_abi(cw)));
    }
    catch (...) {
        return;
    }

    auto b = cw.Bounds();

    auto currentDisplayInformation = winrt::Windows::Graphics::Display::DisplayInformation::GetForCurrentView();
    float dpi = currentDisplayInformation.LogicalDpi();

    const int x = ConvertDipsToPixels(b.X, dpi);
    const int y = ConvertDipsToPixels(b.Y, dpi);
    const int w = ConvertDipsToPixels(b.Width, dpi);
    const int h = ConvertDipsToPixels(b.Height, dpi);

    rect->left = static_cast<long>(x);
    rect->top = static_cast<long>(y);
    rect->right = static_cast<long>(x + w);
    rect->bottom = static_cast<long>(y + h);
}