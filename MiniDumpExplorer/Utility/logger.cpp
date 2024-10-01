#include "pch.h"
#include "logger.h"

#include "DbgHelpUtils/stream_hex_dump.h"
#include "DbgHelpUtils/string_conversation.h"
#include "DbgHelpUtils/system_info_utils.h"
#include "DbgHelpUtils/wide_runtime_error.h"
#include "DbgHelpUtils/windows_error.h"
#include "Helpers/GlobalOptions.h"


#include <format>

#include "DbgHelpUtils/file_version_info.h"
#include "DbgHelpUtils/time_utils.h"

using namespace dlg_help_utils;
using namespace std::string_literals;
using namespace std::string_view_literals;

namespace
{
    constexpr std::array log_level_strings_ = {
        "SYS  "sv,
        "NONE "sv,
        "ERROR"sv,
        "WARN "sv,
        "INFO "sv,
        "DEBUG"sv,
    };
}


logger::logger(key)
{
    MessageFilter([this](log_command const command, log_level const level, std::string log_line, log_level const current_level, std::chrono::local_time<std::chrono::seconds> const& now_day) mutable // NOLINT(performance-unnecessary-value-param)
    {
        do_run_log_command(command, level, std::move(log_line), current_level, now_day);
    });
    open_log_file(GlobalOptions::Options().LogLevel());
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
        open_log_file(GlobalOptions::Options().LogLevel());
    }
}

void logger::LogMessage(log_level const level, std::wstring_view const& message) const
{
    run_log_command(log_command::log_message, level, string_conversation::wstring_to_utf8(message), filter_);
}

void logger::LogMessage(log_level const level, std::string_view const& message) const
{
    run_log_command(log_command::log_message, level, message, filter_);
}

void logger::Flush() const
{
    if(!log_file_)
    {
        return;
    }

   if (!FlushFileBuffers(log_file_.get()))
   {
       windows_error::throw_windows_api_error(L"FlushFileBuffers"sv, get_log_file_name(log_file_timestamp_, log_instance_));
   }
}

void logger::Close() const
{
    run_log_command(log_command::close_log_file, log_level::system, "", filter_);
}

void logger::HandleUnknownException(std::source_location const& location) const
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
        LogMessage(log_level::error, line);
    }
    catch(std::exception const& e)
    {
        auto const line = std::format(L"[{}:{}:{}]:[exception:{}]: {}", acp_to_wstring(location.file_name()), acp_to_wstring(location.function_name()), location.line(), acp_to_wstring(typeid(e).name()), acp_to_wstring(e.what()));
        OutputDebugStringW(line.c_str());
        LogMessage(log_level::error, line);
    }
    catch(winrt::hresult_error const& e)
    {
        auto const line = std::format(L"[{}:{}:{}]:[hresult_error:{}]: hresult_error: {} / code: {}", acp_to_wstring(location.file_name()), acp_to_wstring(location.function_name()), location.line(), acp_to_wstring(typeid(e).name()), e.message(), stream_hex_dump::to_hex(e.code().value));
        OutputDebugStringW(line.c_str());
        LogMessage(log_level::error, line);
    }
    catch(...)
    {
        auto const line = std::format(L"[{}:{}:{}]: unhandled unknown exception", acp_to_wstring(location.file_name()), acp_to_wstring(location.function_name()), location.line());
        OutputDebugStringW(line.c_str());
        LogMessage(log_level::error, line);
    }
}

void logger::open_log_file(log_level const level, std::chrono::local_time<std::chrono::seconds> const& now)
{
    log_file_timestamp_ = now;
    while(!log_file_)
    {
        log_file_ = open_log_file(level, log_file_timestamp_, log_instance_);

        if(!log_file_)
        {
            ++log_instance_;
        }
    }

    log_header();

    if(log_instance_ > 0)
    {
        run_log_command(log_command::log_message, log_level::system, std::format("Log file already open, using instance {}", log_instance_), filter_);
    }
}

void logger::log_header()
{
    std::function<void (log_command, log_level, std::string, log_level, std::chrono::local_time<std::chrono::seconds> const&)> filter = [this](log_command const command, log_level const level, std::string log_line, log_level const current_level, std::chrono::local_time<std::chrono::seconds> const& now_day)
    {
        do_run_log_command(command, level, std::move(log_line), current_level, now_day);
    };
    run_log_command(log_command::log_message, log_level::system, "##########################LOG FILE START##########################", filter);

    auto const& path = system_info_utils::get_running_application_path();
    file_version_info const version_info{ path.wstring() };

#ifdef _DEBUG
    const std::wstring config = L"debug";
#else
    const std::wstring config = L"release";
#endif

    // binary platform (exe only)
    std::wstring binary_type_string;
    DWORD binary_type = 0;
    if (GetBinaryTypeW(path.wstring().c_str(), &binary_type) != 0)
    {
        if (binary_type == SCS_32BIT_BINARY)
        {
            binary_type_string = L"x86";
        }
        else if (binary_type == SCS_64BIT_BINARY)
        {
            binary_type_string = L"x64";
        }
    }

    // get timezone info
    _tzset();
    int daylight;
    _get_daylight( &daylight );
    size_t s;
    char tz_name[100];
    _get_tzname( &s, tz_name, sizeof(tz_name), 0 );

    // how do we know when process was started/restarted
    FILETIME creation_time, exit_time, kernel_time, user_time;
    if (!GetProcessTimes(GetCurrentProcess(), &creation_time, &exit_time, &kernel_time, &user_time))
    {
        windows_error::throw_windows_api_error(L"GetProcessTimes");
    }

    run_log_command(log_command::log_message, log_level::system, string_conversation::wstring_to_utf8(std::format(L"[Module: {0}]  [Version: {1}]  [Config: {2}] [PID: {3}] [{4}]", path.wstring(), version_info.file_version(), config, stream_hex_dump::to_hex(GetCurrentProcessId()), binary_type_string.empty() ? stream_hex_dump::to_hex(binary_type) : binary_type_string)), filter);
    run_log_command(log_command::log_message, log_level::system, std::format("[TZ: {}]  [DST: {}]", tz_name, daylight == 0 ? "off":"on"), filter);
    run_log_command(log_command::log_message, log_level::system, string_conversation::wstring_to_utf8(std::format(L"[Started: {}]", time_utils::to_local_timestamp_string(creation_time))), filter);
    run_log_command(log_command::log_message, log_level::system, "##################################################################", filter);
}

void logger::do_close()
{
    log_file_.reset();
}

handles::windows_handle logger::open_log_file(log_level const level, std::chrono::local_time<std::chrono::seconds> const& now, int const log_instance)
{
    if(level == log_level::none)
    {
        return handles::make_empty_windows_handle();
    }

    auto const file_name = get_log_file_name(now, log_instance);
    auto file_handle = handles::make_windows_handle(CreateFileW(file_name.c_str(), GENERIC_WRITE, FILE_SHARE_READ, nullptr, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr));
    if (!file_handle)
    {
        auto const ec = GetLastError();

        // if the file is already open, try to open a new file with a different instance number
        if(ec == ERROR_SHARING_VIOLATION)
        {
            return file_handle;
        }

        windows_error::throw_windows_api_error(L"CreateFileW"sv, file_name, ec);
    }

    SetFilePointer(file_handle.get(), 0, nullptr, FILE_END);

    return file_handle;
}

void logger::run_log_command(log_command const command, log_level const level, std::string_view const& message, std::function<void(log_command, log_level, std::string, log_level, std::chrono::local_time<std::chrono::seconds> const&)> const& filter)
{
    auto const current_level = GlobalOptions::Options().LogLevel();
    if((level != log_level::system && level > current_level) || current_level == log_level::none)
    {
        return;
    }

    auto const now = std::chrono::current_zone()->to_local(std::chrono::system_clock::now());
    auto const now_day = std::chrono::floor<std::chrono::days>(now);
    filter(command, level, std::format("[{0:%T}][{1}][T:{2}] {3}\n", now, log_level_strings_[static_cast<size_t>(static_cast<int>(level) + 1)], GetCurrentThreadId(), message), current_level, now_day);
}

// ReSharper disable once CppPassValueParameterByConstReference
void logger::do_run_log_command(log_command const command, [[maybe_unused]] log_level const level, std::string const log_line, log_level const current_level, std::chrono::local_time<std::chrono::seconds> const& now_day)  // NOLINT(performance-unnecessary-value-param)
{
    switch(command)
    {
    case log_command::log_message:
        write_log_message(log_line, current_level, now_day);
        break;
    case log_command::close_log_file:
        do_close();
        break;
    }
}

void logger::write_log_message(std::string const& log_line, log_level const current_level, std::chrono::local_time<std::chrono::seconds> const& now_day)
{
    if(log_file_ && close_log_file(now_day))
    {
        Flush();
        do_close();
    }

    if(!log_file_)
    {
        open_log_file(current_level, now_day);
    }

    auto const& message_bytes = reinterpret_cast<std::byte const*>(log_line.c_str());
    auto const message_size = static_cast<DWORD>(log_line.size());
    DWORD bytes_written = 0;
    if (!WriteFile(log_file_.get(), message_bytes, message_size, &bytes_written, nullptr))
    {
        windows_error::throw_windows_api_error(L"WriteFile"sv, get_log_file_name(log_file_timestamp_, log_instance_));
    }
}

bool logger::close_log_file(std::chrono::local_time<std::chrono::seconds> const& now) const
{
    return log_file_ && log_file_timestamp_ != now;
}

std::wstring logger::get_log_file_name(std::chrono::local_time<std::chrono::seconds> const& now, int const log_instance)
{
    auto const base_log_directory = system_info_utils::get_temp_directory() / L"MiniDumpExplorer"sv;
    if(!CreateDirectoryW(base_log_directory.c_str(), nullptr))
    {
        if(auto const last_error = GetLastError(); last_error != ERROR_ALREADY_EXISTS)
        {
            windows_error::throw_windows_api_error(L"CreateDirectoryW"sv, base_log_directory.wstring(), last_error);
        }
    }

    auto const file_path = base_log_directory / std::format(L"MiniDumpExplorer_{0:%Y_%m_%d}{1}.log", now, log_instance > 0 ? std::format(L"_{}", log_instance) : L""s);
    return file_path.wstring();
}
