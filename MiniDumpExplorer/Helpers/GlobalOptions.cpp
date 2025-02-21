#include "pch.h"
#include "GlobalOptions.h"

#include <filesystem>

#include "DbgHelpUtils/wide_runtime_error.h"

using namespace std::string_literals;

namespace
{
    const std::wstring NumberDisplayFormatProperty = L"NumberDisplayFormat";
    const std::wstring SizeNumberDisplayFormatProperty = L"SizeNumberDisplayFormat";
    const std::wstring SizeFormatProperty = L"SizeFormat";
    const std::wstring SizeBaseProperty = L"SizeBase";
    const std::wstring TimeStampLocaleProperty = L"TimeStampLocale";
    const std::wstring TimeStampLocaleIdProperty = L"TimeStampLocaleId";
    const std::wstring DateFormatFlagsProperty = L"DateFormatFlags";
    const std::wstring DateFormatProperty = L"DateFormat";
    const std::wstring TimeFormatFlagsProperty = L"TimeFormatFlags";
    const std::wstring TimeFormatProperty = L"TimeFormat";
    const std::wstring DurationFormatProperty = L"DurationFormat";
    const std::wstring ApplicationThemeProperty = L"ApplicationTheme"s;
    const std::wstring LogLevelProperty = L"LogLevel"s;
    const std::wstring RecentFilesProperty = L"RecentFiles"s;
    const std::wstring SymbolLoadDebugProperty = L"SymbolLoadDebug"s;
    const std::wstring SymbolLoadDebugMemoryProperty = L"SymbolLoadDebugMemory"s;
}

GlobalOptions::GlobalOptions(key)
    : applicationTheme_{AppPropertiesHelper::GetEnumProperty(ApplicationThemeProperty, winrt::Microsoft::UI::Xaml::ElementTheme::Default)}
    , logLevel_{AppPropertiesHelper::GetEnumProperty<log_level>(LogLevelProperty, log_level::info)}
    , numberDisplayFormat_{AppPropertiesHelper::GetEnumProperty(NumberDisplayFormatProperty, NumberDisplayFormatType::Hexadecimal)}
    , sizeNumberDisplayFormat_{AppPropertiesHelper::GetEnumProperty(SizeNumberDisplayFormatProperty, SizeNumberDisplayFormatType::Auto)}
    , sizeFormat_{AppPropertiesHelper::GetEnumProperty(SizeFormatProperty, dlg_help_utils::size_units::print::full)}
    , sizeBase_{AppPropertiesHelper::GetEnumProperty(SizeBaseProperty, SizeDisplayNumberBaseType::Base16)}
    , timeStamp_{AppPropertiesHelper::GetEnumProperty(TimeStampLocaleProperty, TimeStampLocaleType::Local)}
    , locale_id_{AppPropertiesHelper::GetUnsignedIntProperty(TimeStampLocaleIdProperty, LOCALE_USER_DEFAULT)}
    , date_format_flags_{AppPropertiesHelper::GetUnsignedIntProperty(DateFormatFlagsProperty, DATE_LONGDATE | DATE_AUTOLAYOUT)}
    , date_format_{AppPropertiesHelper::GetStringProperty(DateFormatProperty)}
    , time_format_flags_{AppPropertiesHelper::GetUnsignedIntProperty(TimeFormatFlagsProperty, 0)}
    , time_format_{AppPropertiesHelper::GetStringProperty(TimeFormatProperty)}
    , durationFormat_{AppPropertiesHelper::GetEnumProperty(DurationFormatProperty, DurationFormatType::TimeSpan)}
    , recentFiles_{AppPropertiesHelper::GetStringVectorProperty(RecentFilesProperty)}
    , symbolLoadDebug_{AppPropertiesHelper::GetBoolProperty(SymbolLoadDebugProperty, true)}
    , symbolLoadDebugMemory_{AppPropertiesHelper::GetBoolProperty(SymbolLoadDebugMemoryProperty, false)}
{
}

void GlobalOptions::ApplicationTheme(winrt::Microsoft::UI::Xaml::ElementTheme const value)
{
    if(applicationTheme_ == value)
    {
        return;
    }

    applicationTheme_ = value;
    AppPropertiesHelper::SetEnumProperty(ApplicationThemeProperty, value);
}

void GlobalOptions::LogLevel(log_level const value)
{
    if(value < log_level::none || value > log_level::debug)
    {
        throw dlg_help_utils::exceptions::wide_runtime_error{std::format(L"Invalid log level value {}.", static_cast<int>(value))};
    }

    if(auto const oldLogLevel = logLevel_.exchange(value);
        oldLogLevel == value)
    {
        return;
    }

    AppPropertiesHelper::SetEnumProperty(LogLevelProperty, value);
}

void GlobalOptions::NumberDisplayFormat(NumberDisplayFormatType const value)
{
    ApplyValue(value, numberDisplayFormat_, NumberDisplayFormatProperty, numberDisplayFormatCallbacks_, numberDisplayFormatCallbacksMutex_, [](auto const& callback, auto const value) { return callback(value); });
}

void GlobalOptions::TimeStampLocale(TimeStampLocaleType const value)
{
    ApplyValue(value, timeStamp_, TimeStampLocaleProperty, timeStampFormatCallbacks_, timeStampFormatCallbacksMutex_, [this](auto const& callback, auto const value) { return callback(value, locale_id_, date_format_flags_, date_format_, time_format_flags_, time_format_); });
}

void GlobalOptions::LocaleId(LCID const value)
{
    ApplyValue(value, locale_id_, TimeStampLocaleIdProperty, timeStampFormatCallbacks_, timeStampFormatCallbacksMutex_, [this](auto const& callback, auto const value) { return callback(timeStamp_, value, date_format_flags_, date_format_, time_format_flags_, time_format_); });
}

void GlobalOptions::DateFormatFlags(DWORD const value)
{
    ApplyValue(value, date_format_flags_, DateFormatFlagsProperty, timeStampFormatCallbacks_, timeStampFormatCallbacksMutex_, [this](auto const& callback, auto const value) { return callback(timeStamp_, locale_id_, value, date_format_, time_format_flags_, time_format_); });
}

void GlobalOptions::DateFormat(std::wstring const& value)
{
    ApplyValue(value, date_format_, DateFormatProperty, timeStampFormatCallbacks_, timeStampFormatCallbacksMutex_, [this](auto const& callback, auto const& value) { return callback(timeStamp_, locale_id_, date_format_flags_, value, time_format_flags_, time_format_); });
}

void GlobalOptions::TimeFormatFlags(DWORD const value)
{
    ApplyValue(value, time_format_flags_, TimeFormatFlagsProperty, timeStampFormatCallbacks_, timeStampFormatCallbacksMutex_, [this](auto const& callback, auto const value) { return callback(timeStamp_, locale_id_, date_format_flags_, date_format_, value, time_format_); });
}

void GlobalOptions::TimeFormat(std::wstring const& value)
{
    ApplyValue(value, time_format_, TimeFormatProperty, timeStampFormatCallbacks_, timeStampFormatCallbacksMutex_, [this](auto const& callback, auto const& value) { return callback(timeStamp_, locale_id_, date_format_flags_, date_format_, time_format_flags_, value); });
}

void GlobalOptions::SizeNumberDisplayFormat(SizeNumberDisplayFormatType const value)
{
    ApplyValue(value, sizeNumberDisplayFormat_, SizeNumberDisplayFormatProperty, sizeNumberDisplayFormatCallbacks_, sizeNumberDisplayFormatCallbacksMutex_, [this](auto const& callback, auto value) { return callback(value, SizeFormat(), SizeBase()); });
}

void GlobalOptions::SizeFormat(dlg_help_utils::size_units::print const value)
{
    ApplyValue(value, sizeFormat_, SizeFormatProperty, sizeNumberDisplayFormatCallbacks_, sizeNumberDisplayFormatCallbacksMutex_, [this](auto const& callback, auto const value) { return callback(SizeNumberDisplayFormat(), value, SizeBase()); });
}

void GlobalOptions::SizeBase(SizeDisplayNumberBaseType const value)
{
    ApplyValue(value, sizeBase_, SizeBaseProperty, sizeNumberDisplayFormatCallbacks_, sizeNumberDisplayFormatCallbacksMutex_, [this](auto const& callback, auto const value) { return callback(SizeNumberDisplayFormat(), SizeFormat(), value); });
}

void GlobalOptions::DurationFormat(DurationFormatType const value)
{
    ApplyValue(value, durationFormat_, DurationFormatProperty, durationFormatCallbacks_, durationFormatCallbacksMutex_, [this](auto const& callback, auto const value) { return callback(value); });
}

void GlobalOptions::SymbolLoadDebug(bool const value)
{
    if (symbolLoadDebug_ == value)
    {
        return;
    }

    symbolLoadDebug_ = value;
    AppPropertiesHelper::SetBoolProperty(SymbolLoadDebugProperty, value);
}

void GlobalOptions::SymbolLoadDebugMemory(bool const value)
{
    if (symbolLoadDebugMemory_ == value)
    {
        return;
    }

    symbolLoadDebugMemory_ = value;
    AppPropertiesHelper::SetBoolProperty(SymbolLoadDebugMemoryProperty, value);
}

void GlobalOptions::OnNumberDisplayFormatChanged(std::function<bool(NumberDisplayFormatType)> callback)
{
    std::lock_guard lock{numberDisplayFormatCallbacksMutex_};
    numberDisplayFormatCallbacks_.push_back(std::move(callback));
}

void GlobalOptions::OnSizeNumberDisplayFormatChanged(std::function<bool(SizeNumberDisplayFormatType, dlg_help_utils::size_units::print, SizeDisplayNumberBaseType)> callback)
{
    std::lock_guard lock{sizeNumberDisplayFormatCallbacksMutex_};
    sizeNumberDisplayFormatCallbacks_.push_back(std::move(callback));
}

void GlobalOptions::OnTimeStampFormatChanged(std::function<bool(TimeStampLocaleType, LCID, DWORD, std::wstring const&, DWORD, std::wstring const&)> callback)
{
    std::lock_guard lock{timeStampFormatCallbacksMutex_};
    timeStampFormatCallbacks_.push_back(std::move(callback));
}

void GlobalOptions::OnDurationFormatChanged(std::function<bool(DurationFormatType)> callback)
{
    std::lock_guard lock{durationFormatCallbacksMutex_};
    durationFormatCallbacks_.push_back(std::move(callback));
}

void GlobalOptions::RecentFiles(std::vector<std::wstring> value)
{
    ApplyValue(std::move(value), recentFiles_, RecentFilesProperty, recentFilesCallbacks_, recentFilesCallbacksMutex_, [](auto const& callback, auto const& value) { return callback(value); });
}

void GlobalOptions::AddRecentFile(std::wstring const& fullPath)
{
    std::filesystem::path const path{fullPath.c_str()};
    if(!exists(path))
    {
        return;
    }

    std::vector<std::wstring> recentFiles;
    recentFiles.reserve(recentFiles_.size() + 1);

    recentFiles.push_back(fullPath);

    for (auto const& recentFile : recentFiles_)
    {
        if(path == recentFile)
        {
            continue;
        }

        if(!std::filesystem::exists(recentFile))
        {
            continue;
        }

        recentFiles.push_back(recentFile);
    }

    RecentFiles(std::move(recentFiles));
}

void GlobalOptions::OnRecentFiles(std::function<bool(std::vector<std::wstring> const&)> callback)
{
    std::lock_guard lock{recentFilesCallbacksMutex_};
    recentFilesCallbacks_.push_back(std::move(callback));
}

GlobalOptions& GlobalOptions::Options()
{
    static auto const options{std::make_unique<GlobalOptions>(key{})};
    return *options;
}
