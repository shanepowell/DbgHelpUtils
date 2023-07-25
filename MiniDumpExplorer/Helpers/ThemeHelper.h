#pragma once

class ThemeHelper
{
public:
    static winrt::Microsoft::UI::Xaml::ElementTheme WindowActualTheme();
    static winrt::Microsoft::UI::Xaml::ElementTheme ActualTheme();
    static winrt::Microsoft::UI::Xaml::ElementTheme RootTheme();
    static void RootTheme(winrt::Microsoft::UI::Xaml::ElementTheme value);
    static void SetDefaultTheme(const winrt::Microsoft::UI::Xaml::Window& window);
};
