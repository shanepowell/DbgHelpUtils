#pragma once
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

    static void HandleUnknownException(std::source_location const& location = std::source_location::current());

private:
    static dlg_help_utils::handles::windows_handle open_log_file(log_level log_level);
    static std::wstring get_log_file_name();

private:
    dlg_help_utils::handles::windows_handle log_file_;
};
