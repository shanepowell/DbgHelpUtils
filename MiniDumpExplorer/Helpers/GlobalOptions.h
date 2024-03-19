#pragma once
#include <functional>

#include "AppPropertiesHelper.h"
#include "DbgHelpUtils/size_units.h"
#include "Utility/log_level.h"

enum class NumberDisplayFormat
{
    Hexadecimal,
    Decimal
};

enum class SizeNumberDisplayFormat
{
    Auto,
    Bytes,
    Kilobytes,
    Megabytes,
    Gigabytes,
    Terabytes,
    Petabytes,
    Exabytes,
    Raw
};

enum class SizeDisplayNumberBase
{
    Base10,
    Base16
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

    SizeNumberDisplayFormat SizeNumberDisplayFormat() const { return sizeNumberDisplayFormat_; }
    void SizeNumberDisplayFormat(::SizeNumberDisplayFormat value);

    dlg_help_utils::size_units::print SizeFormat() const { return sizeFormat_; }
    void SizeFormat(dlg_help_utils::size_units::print value);

    SizeDisplayNumberBase SizeBase() const { return sizeBase_; }
    void SizeBase(SizeDisplayNumberBase value);

    void OnNumberDisplayFormatChanged(std::function<bool(::NumberDisplayFormat)> callback);
    void OnSizeNumberDisplayFormatChanged(std::function<bool(::SizeNumberDisplayFormat, dlg_help_utils::size_units::print, SizeDisplayNumberBase)> callback);

    std::vector<std::wstring> const& RecentFiles() const { return recentFiles_; }
    void RecentFiles(std::vector<std::wstring> value);
    void AddRecentFile(std::wstring const& fullPath);

    void OnRecentFiles(std::function<bool(std::vector<std::wstring> const&)> callback);

    static GlobalOptions& Options();

private:
    template<typename T, typename CVT, typename CT>
    void ApplyValue(T newValue, T& existingValue, std::wstring const& valueName, CVT& callbacks, CT callback)
    {
        if(existingValue == newValue)
        {
            return;
        }

        existingValue = std::move(newValue);
        if constexpr (std::is_enum_v<T>)
        {
            AppPropertiesHelper::SetEnumProperty(valueName, existingValue);
        }
        else if constexpr (std::is_same_v<T, std::wstring>)
        {
            AppPropertiesHelper::SetStringProperty(valueName, existingValue);
        }
        else if constexpr (std::is_same_v<T, int>)
        {
            AppPropertiesHelper::SetIntProperty(valueName, existingValue);
        }
        else if constexpr (std::is_same_v<T, std::vector<std::wstring>>)
        {
            AppPropertiesHelper::SetStringVectorProperty(valueName, existingValue);
        }
        else
        {
            static_assert(false);
        }

        for (auto it = callbacks.begin(); it != callbacks.end(); )
        {
            if(!callback(*it, existingValue))
            {
                it = callbacks.erase(it);
            }
            else
            {
                ++it;
            }
        }
    }

private:
    winrt::Microsoft::UI::Xaml::ElementTheme applicationTheme_;
    std::atomic<log_level> logLevel_;
    ::NumberDisplayFormat numberDisplayFormat_;
    ::SizeNumberDisplayFormat sizeNumberDisplayFormat_;
    dlg_help_utils::size_units::print sizeFormat_;
    SizeDisplayNumberBase sizeBase_;
    std::vector<std::wstring> recentFiles_;
    std::vector<std::function<bool(::NumberDisplayFormat)>> numberDisplayFormatCallbacks_;
    std::vector<std::function<bool(::SizeNumberDisplayFormat, dlg_help_utils::size_units::print, SizeDisplayNumberBase)>> sizeNumberDisplayFormatCallbacks_;
    std::vector<std::function<bool(std::vector<std::wstring> const&)>> recentFilesCallbacks_;
};
