#include "pch.h"
#include "logger.h"

#include "DbgHelpUtils/stream_hex_dump.h"
#include "DbgHelpUtils/string_conversation.h"
#include "DbgHelpUtils/system_info_utils.h"
#include "DbgHelpUtils/wide_runtime_error.h"
#include "DbgHelpUtils/windows_error.h"
#include "Helpers/GlobalOptions.h"


#include <format>

using namespace dlg_help_utils;
using namespace std::string_literals;
using namespace std::string_view_literals;

namespace
{
    constexpr std::array log_level_strings_ = {
        "NONE "sv,
        "ERROR"sv,
        "WARN "sv,
        "INFO "sv,
        "DEBUG"sv,
        "TRACE"sv
    };
}


logger::logger(key)
    : log_file_{open_log_file(GlobalOptions::Options().LogLevel())}
{
}

logger& logger::Log()
{
    static auto log{std::make_unique<logger>(key{})};
    return *log;
}

// ReSharper disable once CppMemberFunctionMayBeStatic
log_level logger::LogLevel() const
{
    return GlobalOptions::Options().LogLevel();
}

void logger::LogLevel(log_level const level)
{
    GlobalOptions::Options().LogLevel(level);

    if(level == log_level::none)
    {
        Flush();
        Close();
    }
    else if(!log_file_)
    {
        log_file_ = open_log_file(level);
    }
}

void logger::LogMessage(log_level const level, std::wstring_view const& message) const
{
    LogMessage(level, string_conversation::wstring_to_utf8(message));
}

void logger::LogMessage(log_level const level, std::string_view const& message) const
{
    if(level > GlobalOptions::Options().LogLevel() || GlobalOptions::Options().LogLevel() == log_level::none)
    {
        return;
    }

    if(!log_file_)
    {
        open_log_file(GlobalOptions::Options().LogLevel());
    }

    auto const time = std::chrono::current_zone()->to_local(std::chrono::system_clock::now());
    auto const log_line = std::format("[{0:%Y-%m-%d %X}][{1}] {2}\n", time, log_level_strings_[static_cast<int>(level)], message);

    auto const& message_bytes = reinterpret_cast<std::byte const*>(log_line.c_str());
    auto const message_size = static_cast<DWORD>(log_line.size());
    DWORD bytes_written = 0;
    if (!WriteFile(log_file_.get(), message_bytes, message_size, &bytes_written, nullptr))
    {
        windows_error::throw_windows_api_error(L"WriteFile"sv, get_log_file_name());
    }
}

void logger::Flush() const
{
    if(!log_file_)
    {
        return;
    }

   if (!FlushFileBuffers(log_file_.get()))
   {
       windows_error::throw_windows_api_error(L"FlushFileBuffers"sv, get_log_file_name());
   }
}

void logger::Close()
{
    log_file_.reset();
}

void logger::HandleUnknownException(std::source_location const& location)
{
    using namespace dlg_help_utils::string_conversation;
    try
    {
        throw;
    }
    catch(exceptions::wide_runtime_error const& e)
    {
        auto const line = std::format(L"[{}:{}:{}]:[wide_runtime_error:{}]: {}", acp_to_wstring(location.file_name()), acp_to_wstring(location.function_name()), location.line(), acp_to_wstring(typeid(e).name()), e.message());
        OutputDebugStringW(line.c_str());
        Log().LogMessage(log_level::error, line);
    }
    catch(std::exception const& e)
    {
        auto const line = std::format(L"[{}:{}:{}]:[exception:{}]: {}", acp_to_wstring(location.file_name()), acp_to_wstring(location.function_name()), location.line(), acp_to_wstring(typeid(e).name()), acp_to_wstring(e.what()));
        OutputDebugStringW(line.c_str());
        Log().LogMessage(log_level::error, line);
    }
    catch(winrt::hresult_error const& e)
    {
        auto const line = std::format(L"[{}:{}:{}]:[hresult_error:{}]: hresult_error: {} / code: {}", acp_to_wstring(location.file_name()), acp_to_wstring(location.function_name()), location.line(), acp_to_wstring(typeid(e).name()), e.message(), stream_hex_dump::to_hex(e.code().value));
        OutputDebugStringW(line.c_str());
        Log().LogMessage(log_level::error, line);
    }
    catch(...)
    {
        auto const line = std::format(L"[{}:{}:{}]: unhandled unknown exception", acp_to_wstring(location.file_name()), acp_to_wstring(location.function_name()), location.line());
        OutputDebugStringW(line.c_str());
        Log().LogMessage(log_level::error, line);
    }

}

handles::windows_handle logger::open_log_file(log_level const log_level)
{
    if(log_level == log_level::none)
    {
        return handles::make_empty_windows_handle();
    }

    auto const file_name = get_log_file_name();
    auto file_handle = handles::make_windows_handle(CreateFileW(file_name.c_str(), GENERIC_WRITE, FILE_SHARE_READ, nullptr, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr));
    if (!file_handle)
    {
        windows_error::throw_windows_api_error(L"CreateFileW"sv, file_name);
    }

    SetFilePointer(file_handle.get(), 0, nullptr, FILE_END);

    return file_handle;
}

std::wstring logger::get_log_file_name()
{
    auto const base_log_directory = system_info_utils::get_temp_directory() / L"MiniDumpExplorer"sv;
    if(!CreateDirectoryW(base_log_directory.c_str(), nullptr))
    {
        if(auto const last_error = GetLastError(); last_error != ERROR_ALREADY_EXISTS)
        {
            windows_error::throw_windows_api_error(L"CreateDirectoryW"sv, base_log_directory.wstring(), last_error);
        }
    }

    auto const file_path = base_log_directory / std::format(L"MiniDumpExplorer_{}.log", GetCurrentProcessId());
    return file_path.wstring();
}
