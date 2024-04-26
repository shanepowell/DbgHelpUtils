#pragma once

class WindowHelper
{
public:
    static winrt::Microsoft::UI::Xaml::Window CreateNewWindow();
    static void TrackWindow(winrt::Microsoft::UI::Xaml::Window const& window);
    static winrt::Microsoft::UI::Windowing::AppWindow GetAppWindow(winrt::Microsoft::UI::Xaml::Window const& window);
    static winrt::Microsoft::UI::Xaml::Window GetWindowForElement(winrt::Microsoft::UI::Xaml::UIElement const& element);

    static std::vector<winrt::Microsoft::UI::Xaml::Window> ActiveWindows() { return activeWindows_; };
    static bool IsExiting() { return exiting_; }

private:
    static std::vector<winrt::Microsoft::UI::Xaml::Window> activeWindows_;
    static std::atomic_bool exiting_;
};
