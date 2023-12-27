#include "pch.h"
#include "App.xaml.h"

#include "DbgHelpUtils/stream_hex_dump.h"
#include "Helpers/AppPropertiesHelper.h"
#include "Helpers/ThemeHelper.h"
#include "Helpers/TypeHelper.h"
#include "Helpers/WindowHelper.h"
#include "ViewModels/MainWindow.xaml.h"
#include "Utility/logger.h"

using namespace winrt;
using namespace Windows::Foundation;
using namespace Microsoft::UI::Xaml;
using namespace Microsoft::UI::Xaml::Controls;
using namespace Microsoft::UI::Xaml::Navigation;
using namespace MiniDumpExplorer;
using namespace MiniDumpExplorer::implementation;
using namespace std::string_literals;
using namespace std::string_view_literals;

/// <summary>
/// Initializes the singleton application object.  This is the first line of authored code
/// executed, and as such is the logical equivalent of main() or WinMain().
/// </summary>
App::App()
{
    InitializeComponent();

    UnhandledException([this](IInspectable const&, UnhandledExceptionEventArgs const& e)
    {
        try
        {
            auto const hresult = e.Exception();
            logger::Log().LogMessage(log_level::error, std::format(L"Unhandled Exception: [{0}] - hresult[{1}]", e.Message(), dlg_help_utils::stream_hex_dump::to_hex(hresult.value)));
            logger::Log().Flush();
        }
        catch (...)  // NOLINT(bugprone-empty-catch)
        {
        }
#if defined _DEBUG && !defined DISABLE_XAML_GENERATED_BREAK_ON_UNHANDLED_EXCEPTION
        if (IsDebuggerPresent())
        {
            auto errorMessage = e.Message();
            __debugbreak();
        }
#endif
    });
}

/// <summary>
/// Invoked when the application is launched.
/// </summary>
/// <param name="e">Details about the launch request and process.</param>
void App::OnLaunched([[maybe_unused]]LaunchActivatedEventArgs const& e)
{
    AppPropertiesHelper::Initialize(L"MiniDumpExplorer"s);
    logger::Log().LogMessage(log_level::info, L"Starting"sv);

    // this is required for the startup of the all application dependency properties
    TypeHelper::LoadAddApplicationDependencyProperties();

    window = make<MainWindow>();
    ThemeHelper::SetDefaultTheme(window);
    WindowHelper::TrackWindow(window);
    window.Activate();
}
