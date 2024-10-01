#pragma once

class WindowHelper
{
public:
    static winrt::Microsoft::UI::Xaml::Window CreateNewWindow();
    static void TrackWindow(winrt::Microsoft::UI::Xaml::Window const& window);
    static winrt::Microsoft::UI::Windowing::AppWindow GetAppWindow(winrt::Microsoft::UI::Xaml::Window const& window);
    static winrt::Microsoft::UI::Xaml::Window GetWindowForElement(winrt::Microsoft::UI::Xaml::UIElement const& element);

    static std::vector<winrt::Microsoft::UI::Xaml::Window> ActiveWindows();
    static bool IsExiting();
};
