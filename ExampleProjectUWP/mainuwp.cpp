#include <windows.h>
#include <winrt/Windows.ApplicationModel.Core.h>
#include <winrt/windows.applicationmodel.activation.h>
#include <winrt/windows.foundation.h>
#include <winrt/windows.ui.core.h>


struct App : winrt::implements<App, winrt::Windows::ApplicationModel::Core::IFrameworkViewSource, winrt::Windows::ApplicationModel::Core::IFrameworkView>
{
    winrt::Windows::ApplicationModel::Core::IFrameworkView CreateView()
    {
        return *this;
    }

    void Initialize(winrt::Windows::ApplicationModel::Core::CoreApplicationView const& application_view) {
        application_view.Activated({ this, &App::OnActivated });

        winrt::Windows::ApplicationModel::Core::CoreApplication::Suspending({ this, &App::OnSuspending });

        winrt::Windows::ApplicationModel::Core::CoreApplication::Resuming({ this, &App::OnResuming });
    }

    void SetWindow(winrt::Windows::UI::Core::CoreWindow const& window)
    {

    }

    void Load(winrt::hstring const& /* entryPoint */) {

    }

    void OnActivated(
        winrt::Windows::ApplicationModel::Core::CoreApplicationView const& /* applicationView */,
        winrt::Windows::ApplicationModel::Activation::IActivatedEventArgs const& /* args */)
    {
        winrt::Windows::UI::Core::CoreWindow window = winrt::Windows::UI::Core::CoreWindow::GetForCurrentThread();
        window.Activate();
    }

    void OnSuspending(IInspectable const& /* sender */, winrt::Windows::ApplicationModel::SuspendingEventArgs const& args) {

    }

    void OnResuming(IInspectable const& /* sender */, IInspectable const& /* args */) {

    }

    void Run()
    {
    }

    void Uninitialize()
    {
    }
};

int __stdcall wWinMain(HINSTANCE, HINSTANCE, PWSTR, int)
{
    winrt::Windows::ApplicationModel::Core::CoreApplication::Run(winrt::make<App>());
}