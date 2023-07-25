#include "pch.h"
#include "GlobalOptions.h"

#include "AppPropertiesHelper.h"

using namespace std::string_literals;

namespace
{
    const std::wstring NumberDisplayFormatProperty = L"NumberDisplayFormat";
    const std::wstring ApplicationThemeProperty = L"ApplicationTheme"s;
    const std::wstring LogLevelProperty = L"LogLevel"s;
    const std::wstring RecentFilesProperty = L"RecentFiles"s;
}

GlobalOptions::GlobalOptions(key)
    : applicationTheme_{AppPropertiesHelper::GetEnumProperty(ApplicationThemeProperty, winrt::Microsoft::UI::Xaml::ElementTheme::Default)}
    , logLevel_{AppPropertiesHelper::GetEnumProperty<log_level>(LogLevelProperty, log_level::info)}
    , numberDisplayFormat_{AppPropertiesHelper::GetEnumProperty(NumberDisplayFormatProperty, NumberDisplayFormat::Hexadecimal)}
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
    if(numberDisplayFormat_ == value)
    {
        return;
    }

    numberDisplayFormat_ = value;
    AppPropertiesHelper::SetEnumProperty(NumberDisplayFormatProperty, value);
    for (auto it = numberDisplayFormatCallbacks_.begin(); it != numberDisplayFormatCallbacks_.end(); )
    {
        if(!(*it)(value))
        {
            it = numberDisplayFormatCallbacks_.erase(it);
        }
        else
        {
            ++it;
        }
    }
}

void GlobalOptions::OnNumberDisplayFormatChanged(std::function<bool(::NumberDisplayFormat)> callback)
{
    numberDisplayFormatCallbacks_.push_back(std::move(callback));
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
