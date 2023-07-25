#pragma once
#include <functional>

#include "Utility/log_level.h"

enum class NumberDisplayFormat
{
    Hexadecimal,
    Decimal
};

class GlobalOptions
{
private:
    struct key{};

public:
    GlobalOptions(key);

    winrt::Microsoft::UI::Xaml::ElementTheme ApplicationTheme() const { return applicationTheme_; }
    void ApplicationTheme(winrt::Microsoft::UI::Xaml::ElementTheme value);

    log_level LogLevel() const { return logLevel_; }
    void LogLevel(log_level value);

    NumberDisplayFormat NumberDisplayFormat() const { return numberDisplayFormat_; }
    void NumberDisplayFormat(::NumberDisplayFormat value);

    void OnNumberDisplayFormatChanged(std::function<bool(::NumberDisplayFormat)> callback);

    std::vector<std::wstring> const& RecentFiles() const { return recentFiles_; }
    void RecentFiles(std::vector<std::wstring> value);

    static GlobalOptions& Options();

private:
    winrt::Microsoft::UI::Xaml::ElementTheme applicationTheme_;
    log_level logLevel_;
    ::NumberDisplayFormat numberDisplayFormat_;
    std::vector<std::wstring> recentFiles_;
    std::vector<std::function<bool(::NumberDisplayFormat)>> numberDisplayFormatCallbacks_;
};
