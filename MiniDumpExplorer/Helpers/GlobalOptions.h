#pragma once
#include <functional>

#include "AppPropertiesHelper.h"
#include "DbgHelpUtils/size_units.h"
#include "Utility/log_level.h"

enum class NumberDisplayFormatType
{
    Hexadecimal,
    Decimal
};

enum class SizeNumberDisplayFormatType
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

enum class SizeDisplayNumberBaseType
{
    Base10,
    Base16
};

enum class TimeStampLocaleType
{
    Local,
    UTC,
    DumpFile
};

enum class DurationFormatType
{
    TimeSpan,
    Auto,
    Milliseconds,
    Seconds,
    Minutes,
    Hours,
    Days,
    Weeks,
    Months,
    Years,
    Raw
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

    NumberDisplayFormatType NumberDisplayFormat() const { return numberDisplayFormat_; }
    void NumberDisplayFormat(NumberDisplayFormatType value);

    TimeStampLocaleType TimeStampLocale() const { return timeStamp_; }
    void TimeStampLocale(TimeStampLocaleType value);

    LCID LocaleId() const { return locale_id_; }
    void LocaleId(LCID value);

    DWORD DateFormatFlags() const { return date_format_flags_; }
    void DateFormatFlags(DWORD value);

    std::wstring const& DateFormat() const { return date_format_; }
    void DateFormat(std::wstring const&  value);

    DWORD TimeFormatFlags() const { return time_format_flags_; }
    void TimeFormatFlags(DWORD value);

    std::wstring const& TimeFormat() const { return time_format_; }
    void TimeFormat(std::wstring const&  value);

    SizeNumberDisplayFormatType SizeNumberDisplayFormat() const { return sizeNumberDisplayFormat_; }
    void SizeNumberDisplayFormat(SizeNumberDisplayFormatType value);

    dlg_help_utils::size_units::print SizeFormat() const { return sizeFormat_; }
    void SizeFormat(dlg_help_utils::size_units::print value);

    SizeDisplayNumberBaseType SizeBase() const { return sizeBase_; }
    void SizeBase(SizeDisplayNumberBaseType value);

    DurationFormatType DurationFormat() const { return durationFormat_; }
    void DurationFormat(DurationFormatType value);

    void OnNumberDisplayFormatChanged(std::function<bool(NumberDisplayFormatType)> callback);
    void OnSizeNumberDisplayFormatChanged(std::function<bool(SizeNumberDisplayFormatType, dlg_help_utils::size_units::print, SizeDisplayNumberBaseType)> callback);
    void OnTimeStampFormatChanged(std::function<bool(TimeStampLocaleType, LCID, DWORD, std::wstring const&, DWORD, std::wstring const&)> callback);
    void OnDurationFormatChanged(std::function<bool(DurationFormatType)> callback);

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
        else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, long>)
        {
            AppPropertiesHelper::SetIntProperty(valueName, existingValue);
        }
        else if constexpr (std::is_same_v<T, unsigned int> || std::is_same_v<T, unsigned long>)
        {
            AppPropertiesHelper::SetUnsignedIntProperty(valueName, existingValue);
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
    NumberDisplayFormatType numberDisplayFormat_;
    SizeNumberDisplayFormatType sizeNumberDisplayFormat_;
    dlg_help_utils::size_units::print sizeFormat_;
    SizeDisplayNumberBaseType sizeBase_;
    TimeStampLocaleType timeStamp_;
    LCID locale_id_;
    DWORD date_format_flags_;
    std::wstring date_format_;
    DWORD time_format_flags_;
    std::wstring time_format_;
    DurationFormatType durationFormat_;
    std::vector<std::wstring> recentFiles_;
    std::vector<std::function<bool(NumberDisplayFormatType)>> numberDisplayFormatCallbacks_;
    std::vector<std::function<bool(SizeNumberDisplayFormatType, dlg_help_utils::size_units::print, SizeDisplayNumberBaseType)>> sizeNumberDisplayFormatCallbacks_;
    std::vector<std::function<bool(std::vector<std::wstring> const&)>> recentFilesCallbacks_;
    std::vector<std::function<bool(TimeStampLocaleType, LCID, DWORD, std::wstring const&, DWORD, std::wstring const&)>> timeStampFormatCallbacks_;
    std::vector<std::function<bool(DurationFormatType)>> durationFormatCallbacks_;
};
