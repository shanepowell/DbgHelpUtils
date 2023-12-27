#pragma once

namespace SuspendedHandlers
{
    bool IsHandlerSuspended(winrt::Microsoft::UI::Xaml::DependencyObject const& dependencyObject, winrt::Microsoft::UI::Xaml::DependencyProperty const& dependencyProperty);
    void SetValueNoCallback(winrt::Microsoft::UI::Xaml::DependencyObject const& obj, winrt::Microsoft::UI::Xaml::DependencyProperty const& dependencyProperty, winrt::Windows::Foundation::IInspectable const& value);
};
