#include "pch.h"
#include "ThemeHelper.h"

#include "GlobalOptions.h"
#include "WindowHelper.h"

winrt::Microsoft::UI::Xaml::ElementTheme ThemeHelper::WindowActualTheme()
{
    for(auto const& window : WindowHelper::ActiveWindows())
    {
        if (auto const& rootElement = window.Content().try_as<winrt::Microsoft::UI::Xaml::FrameworkElement>(); rootElement)
        {
            if(rootElement.ActualTheme() != winrt::Microsoft::UI::Xaml::ElementTheme::Default)
            {
                return rootElement.ActualTheme();
            }
        }
    }

    return GlobalOptions::Options().ApplicationTheme() == winrt::Microsoft::UI::Xaml::ElementTheme::Default ? winrt::Microsoft::UI::Xaml::ElementTheme::Light : GlobalOptions::Options().ApplicationTheme();
}

winrt::Microsoft::UI::Xaml::ElementTheme ThemeHelper::ActualTheme()
{
    for(auto const& window : WindowHelper::ActiveWindows())
    {
        if (auto const& rootElement = window.Content().try_as<winrt::Microsoft::UI::Xaml::FrameworkElement>(); rootElement)
        {
            if(rootElement.RequestedTheme() != winrt::Microsoft::UI::Xaml::ElementTheme::Default)
            {
                return rootElement.RequestedTheme();
            }
        }
    }

    return GlobalOptions::Options().ApplicationTheme();
}

winrt::Microsoft::UI::Xaml::ElementTheme ThemeHelper::RootTheme()
{
    for(auto const& window : WindowHelper::ActiveWindows())
    {
        if (auto const& rootElement = window.Content().try_as<winrt::Microsoft::UI::Xaml::FrameworkElement>(); rootElement)
        {
            return rootElement.RequestedTheme();
        }
    }

    return GlobalOptions::Options().ApplicationTheme();
}

void ThemeHelper::RootTheme(winrt::Microsoft::UI::Xaml::ElementTheme const value)
{
    GlobalOptions::Options().ApplicationTheme(value);

    for(auto const& window : WindowHelper::ActiveWindows())
    {
        if (auto const& rootElement = window.Content().try_as<winrt::Microsoft::UI::Xaml::FrameworkElement>(); rootElement)
        {
            rootElement.RequestedTheme(value);
        }
    }
}

void ThemeHelper::SetDefaultTheme(const winrt::Microsoft::UI::Xaml::Window& window)
{
    if (auto const& rootElement = window.Content().try_as<winrt::Microsoft::UI::Xaml::FrameworkElement>(); rootElement)
    {
        rootElement.RequestedTheme(GlobalOptions::Options().ApplicationTheme());
    }
}
