#pragma once
#include <functional>

#include "DbgHelpUtils/handles.h"
#include "log_level.h"

class logger
{
private:
    struct key{};

public:
    logger(key);

    static logger& Log();
    log_level LogLevel() const;
    void LogLevel(log_level level);
    void LogMessage(log_level level, std::wstring_view const& message) const;
    void LogMessage(log_level level, std::string_view const& message) const;
    void Flush() const;
    void Close();

    void HandleUnknownException(std::source_location const& location = std::source_location::current()) const;

    enum class log_command
    {
        log_message,
        close_log_file
    };

    void MessageFilter(std::function<void (log_command command, log_level level, std::string log_line, log_level current_level, std::chrono::local_time<std::chrono::seconds> const& now_day)> filter) { filter_ = std::move(filter); }
    std::function<void (log_command command, log_level level, std::string log_line, log_level current_level, std::chrono::local_time<std::chrono::seconds> const& now_day)> MessageFilter() const { return filter_; }

private:
    static void run_log_command(log_command command, log_level level, std::string_view const& message, std::function<void (log_command, log_level, std::string, log_level, std::chrono::local_time<std::chrono::seconds> const&)> const& filter);
    void do_run_log_command(log_command command, log_level level, std::string log_line, log_level current_level, std::chrono::local_time<std::chrono::seconds> const& now_day);
    void write_log_message(std::string const& log_line, log_level const current_level, std::chrono::local_time<std::chrono::seconds> const& now_day);

    bool close_log_file(std::chrono::local_time<std::chrono::seconds> const& now) const;
    void open_log_file(log_level level, std::chrono::local_time<std::chrono::seconds> const& now = std::chrono::floor<std::chrono::days>(std::chrono::current_zone()->to_local(std::chrono::system_clock::now())));
    void log_header();
    void do_close();
    static dlg_help_utils::handles::windows_handle open_log_file(log_level level, std::chrono::local_time<std::chrono::seconds> const& now, int log_instance);
    static std::wstring get_log_file_name(std::chrono::local_time<std::chrono::seconds> const& now, int log_instance);

private:
    std::chrono::local_time<std::chrono::seconds> log_file_timestamp_{};
    int log_instance_{0};
    dlg_help_utils::handles::windows_handle log_file_{dlg_help_utils::handles::make_empty_windows_handle()};
    std::function<void (log_command command, log_level level, std::string log_line, log_level current_level, std::chrono::local_time<std::chrono::seconds> const& now_day)> filter_;
};
