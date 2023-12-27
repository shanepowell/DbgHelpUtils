#include "pch.h"
#include "GlobalOptions.h"

using namespace std::string_literals;

namespace
{
    const std::wstring NumberDisplayFormatProperty = L"NumberDisplayFormat";
    const std::wstring SizeNumberDisplayFormatProperty = L"SizeNumberDisplayFormat";
    const std::wstring SizeFormatProperty = L"SizeFormat";
    const std::wstring SizeBaseProperty = L"SizeBase";
    const std::wstring ApplicationThemeProperty = L"ApplicationTheme"s;
    const std::wstring LogLevelProperty = L"LogLevel"s;
    const std::wstring RecentFilesProperty = L"RecentFiles"s;
}

GlobalOptions::GlobalOptions(key)
    : applicationTheme_{AppPropertiesHelper::GetEnumProperty(ApplicationThemeProperty, winrt::Microsoft::UI::Xaml::ElementTheme::Default)}
    , logLevel_{AppPropertiesHelper::GetEnumProperty<log_level>(LogLevelProperty, log_level::info)}
    , numberDisplayFormat_{AppPropertiesHelper::GetEnumProperty(NumberDisplayFormatProperty, NumberDisplayFormat::Hexadecimal)}
    , sizeNumberDisplayFormat_{AppPropertiesHelper::GetEnumProperty(SizeNumberDisplayFormatProperty, SizeNumberDisplayFormat::Auto)}
    , sizeFormat_{AppPropertiesHelper::GetEnumProperty(SizeFormatProperty, dlg_help_utils::size_units::print::full)}
    , sizeBase_{AppPropertiesHelper::GetEnumProperty(SizeBaseProperty, SizeDisplayNumberBase::Base16)}
    , recentFiles_{AppPropertiesHelper::GetStringVectorProperty(RecentFilesProperty)}
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
    if(logLevel_ == value)
    {
        return;
    }

    logLevel_ = value;
    AppPropertiesHelper::SetEnumProperty(LogLevelProperty, value);
}

void GlobalOptions::NumberDisplayFormat(::NumberDisplayFormat const value)
{
    ApplyValue(value, numberDisplayFormat_, NumberDisplayFormatProperty, numberDisplayFormatCallbacks_, [](auto const& callback, auto const value) { return callback(value); });
}

void GlobalOptions::SizeNumberDisplayFormat(::SizeNumberDisplayFormat const value)
{
    ApplyValue(value, sizeNumberDisplayFormat_, SizeNumberDisplayFormatProperty, sizeNumberDisplayFormatCallbacks_, [this](auto const& callback, auto value) { return callback(value, SizeFormat(), SizeBase()); });
}

void GlobalOptions::SizeFormat(dlg_help_utils::size_units::print const value)
{
    ApplyValue(value, sizeFormat_, SizeFormatProperty, sizeNumberDisplayFormatCallbacks_, [this](auto const& callback, auto const value) { return callback(SizeNumberDisplayFormat(), value, SizeBase()); });
}

void GlobalOptions::SizeBase(SizeDisplayNumberBase const value)
{
    ApplyValue(value, sizeBase_, SizeBaseProperty, sizeNumberDisplayFormatCallbacks_, [this](auto const& callback, auto const value) { return callback(SizeNumberDisplayFormat(), SizeFormat(), value); });
}

void GlobalOptions::OnNumberDisplayFormatChanged(std::function<bool(::NumberDisplayFormat)> callback)
{
    numberDisplayFormatCallbacks_.push_back(std::move(callback));
}

void GlobalOptions::OnSizeNumberDisplayFormatChanged(std::function<bool(::SizeNumberDisplayFormat, dlg_help_utils::size_units::print, SizeDisplayNumberBase)> callback)
{
    sizeNumberDisplayFormatCallbacks_.push_back(std::move(callback));
}

void GlobalOptions::RecentFiles(std::vector<std::wstring> value)
{
    recentFiles_ = std::move(value);
    AppPropertiesHelper::SetStringVectorProperty(RecentFilesProperty, value);
}

GlobalOptions& GlobalOptions::Options()
{
    static auto const options{std::make_unique<GlobalOptions>(key{})};
    return *options;
}
