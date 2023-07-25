#include "pch.h"
#include "WindowHelper.h"

#include <winrt/Microsoft.UI.Interop.h>
#include <winrt/Microsoft.UI.Windowing.h>

#include <Microsoft.Ui.Xaml.Window.h>

#include "Utility/logger.h"

using namespace std::string_view_literals;

std::vector<winrt::Microsoft::UI::Xaml::Window> WindowHelper::activeWindows_{};

winrt::Microsoft::UI::Xaml::Window WindowHelper::CreateNewWindow()
{
    auto window = winrt::Microsoft::UI::Xaml::Window{};
    window.SystemBackdrop(winrt::Microsoft::UI::Xaml::Media::MicaBackdrop{});
    TrackWindow(window);
    return window;
}

void WindowHelper::TrackWindow(winrt::Microsoft::UI::Xaml::Window const& window)
{
    activeWindows_.push_back(window);
    window.Closed([window](auto const&, auto const&)
    {
        activeWindows_.erase(std::ranges::find(activeWindows_, window));
        if(activeWindows_.empty())
        {
            logger::Log().LogMessage(log_level::info, L"Exiting"sv);
            logger::Log().Flush();
            logger::Log().Close();
        }
    });
}

winrt::Microsoft::UI::Windowing::AppWindow WindowHelper::GetAppWindow(winrt::Microsoft::UI::Xaml::Window const& window)
{
    HWND nativeWindow{ nullptr };
    winrt::check_hresult(window.as<IWindowNative>()->get_WindowHandle(&nativeWindow));
    winrt::Microsoft::UI::WindowId const windowId = winrt::Microsoft::UI::GetWindowIdFromWindow(nativeWindow);
    return winrt::Microsoft::UI::Windowing::AppWindow::GetFromWindowId(windowId);
}

winrt::Microsoft::UI::Xaml::Window WindowHelper::GetWindowForElement(winrt::Microsoft::UI::Xaml::UIElement const& element)
{
    if (element.XamlRoot() != nullptr)
    {
        for( auto const& window : activeWindows_)
        {
            if (element.XamlRoot() == window.Content().XamlRoot())
            {
                return window;
            }
        }
    }

    return winrt::Microsoft::UI::Xaml::Window{nullptr};
}

