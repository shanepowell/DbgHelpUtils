#include "pch.h"
#include "App.xaml.h"

#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.System.h>

#include "DbgHelpUtils/stream_hex_dump.h"
#include "DbgHelpUtils/string_conversation.h"
#include "DbgHelpUtils/wide_runtime_error.h"
#include "Helpers/AppPropertiesHelper.h"
#include "Helpers/ThemeHelper.h"
#include "Helpers/TypeHelper.h"
#include "Helpers/WindowHelper.h"
#include "ViewModels/MainWindow.xaml.h"
#include "Utility/logger.h"

namespace dlg_help_utils::exceptions
{
    class wide_runtime_error;
}

using namespace winrt;
using namespace Windows::Foundation;
using namespace Microsoft::UI::Xaml;
using namespace Microsoft::UI::Xaml::Controls;
using namespace Microsoft::UI::Xaml::Navigation;
using namespace MiniDumpExplorer;
using namespace MiniDumpExplorer::implementation;
using namespace std::string_literals;
using namespace std::string_view_literals;
using namespace dlg_help_utils::string_conversation;
using namespace dlg_help_utils;

namespace
{
    fire_and_forget QueueLogMessage(
        Windows::System::DispatcherQueueController const& log_queue,
        // required to keep alive the app when switched to the logging queue thread
        [[maybe_unused]] com_ptr<App> const lifetime,
        logger::log_command const command,
        log_level const level,
        std::string log_line,  // NOLINT(performance-unnecessary-value-param)
        log_level const current_level,
        std::chrono::system_clock::time_point const now_day,
        std::function<void(logger::log_command, log_level, std::string, log_level, std::chrono::system_clock::time_point const&)> const& filter)
    {
        try
        {
            // switch to logging queue thread
            co_await resume_foreground(log_queue.DispatcherQueue());

            // log message on logging queue thread
            filter(command, level, std::move(log_line), current_level, now_day);
        }
        catch(exceptions::wide_runtime_error const& e)
        {
            auto const line = std::format(L"[QueueLogMessage][wide_runtime_error:{}]: {}", acp_to_wstring(typeid(e).name()), e.message());
            OutputDebugStringW(line.c_str());
        }
        catch(std::exception const& e)
        {
            auto const line = std::format(L"[QueueLogMessage][exception:{}]: {}", acp_to_wstring(typeid(e).name()), acp_to_wstring(e.what()));
            OutputDebugStringW(line.c_str());
        }
        catch(hresult_error const& e)
        {
            auto const line = std::format(L"[QueueLogMessage][hresult_error:{}]: hresult_error: {} / code: {}", acp_to_wstring(typeid(e).name()), e.message(), stream_hex_dump::to_hex(e.code().value));
            OutputDebugStringW(line.c_str());
        }
        catch(...)
        {
            OutputDebugStringW(L"[QueueLogMessage]: unhandled unknown exception");
        }
    }
}

/// <summary>
/// Initializes the singleton application object.  This is the first line of authored code
/// executed, and as such is the logical equivalent of main() or WinMain().
/// </summary>
App::App()
    : log_queue_{Windows::System::DispatcherQueueController::CreateOnDedicatedThread()}
{
    UnhandledException([this](IInspectable const&, UnhandledExceptionEventArgs const& e)
    {
        try
        {
            auto const hresult = e.Exception();
            logger::Log().LogMessage(log_level::error, std::format(L"Unhandled Exception: [{0}] - hresult[{1}]", e.Message(), stream_hex_dump::to_hex(hresult.value)));
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
    logger::Log().MessageFilter(
        [weakPtr = get_weak(), filter = logger::Log().MessageFilter()]
        (logger::log_command const command, log_level const level, std::string log_line, log_level const current_level, std::chrono::system_clock::time_point const& now_day)  // NOLINT(performance-unnecessary-value-param)
        {
            if(auto const self = weakPtr.get())
            {
                QueueLogMessage(self->log_queue_, self->get_strong(), command, level, std::move(log_line), current_level, now_day, filter);
            }
        });
    logger::Log().LogMessage(log_level::info, L"Starting"sv);

    // this is required for the startup of the all application dependency properties
    TypeHelper::LoadAddApplicationDependencyProperties();

    window = make<MainWindow>();
    ThemeHelper::SetDefaultTheme(window);
    WindowHelper::TrackWindow(window);
    window.Activate();
}
