//#include <windows.h>
#include "pch.h"
#include "DirectXEngineFrontendUWP.hpp"

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

        int outputWidth = ConvertDipsToPixels(m_logical_width, m_DPI);
        int outputHeight = ConvertDipsToPixels(m_logical_height, m_DPI);

        DXGI_MODE_ROTATION rotation = ComputeDisplayRotation(m_native_orientation, m_current_orientation);

        if (rotation == DXGI_MODE_ROTATION_ROTATE90 || rotation == DXGI_MODE_ROTATION_ROTATE270)
        {
            std::swap(outputWidth, outputHeight);
        }

        auto windowPtr = winrt::get_unknown(window);

        m_engine_frontend->init(windowPtr, outputWidth, outputHeight, rotation);
        
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
        m_engine_frontend->run();
    }

private:
    void HandleWindowSizeChanged()
    {
        int outputWidth = ConvertDipsToPixels(m_logical_width, m_DPI);
        int outputHeight = ConvertDipsToPixels(m_logical_height, m_DPI);

        DXGI_MODE_ROTATION rotation = ComputeDisplayRotation(m_current_orientation, m_native_orientation);

        if (rotation == DXGI_MODE_ROTATION_ROTATE90 || rotation == DXGI_MODE_ROTATION_ROTATE270)
        {
            std::swap(outputWidth, outputHeight);
        }

        //m_game->OnWindowSizeChanged(outputWidth, outputHeight, rotation);
        //TODO
    }

    std::shared_ptr<DX::DeviceResources>      m_device_resources;
    std::unique_ptr<DirectXEngineFrontendUWP> m_engine_frontend;

    bool  m_exit;
    bool  m_visible;
    bool  m_in_sizemove;
    float m_DPI;
    float m_logical_width;
    float m_logical_height;

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