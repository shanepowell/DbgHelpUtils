#pragma once

namespace Utility
{
    class BindingInfo
    {
    public:
        BindingInfo();
        BindingInfo(winrt::Microsoft::UI::Xaml::Data::BindingExpression const& bindingExpression, winrt::Microsoft::UI::Xaml::DependencyProperty const& bindingTarget, winrt::Microsoft::UI::Xaml::FrameworkElement const& element);

        winrt::Microsoft::UI::Xaml::Data::BindingExpression BindingExpression() const;
        void BindingExpression(winrt::Microsoft::UI::Xaml::Data::BindingExpression const& value);

        winrt::Microsoft::UI::Xaml::DependencyProperty BindingTarget() const;
        void BindingTarget(winrt::Microsoft::UI::Xaml::DependencyProperty const& value);

        winrt::Microsoft::UI::Xaml::FrameworkElement Element() const;
        void Element(winrt::Microsoft::UI::Xaml::FrameworkElement const& value);

    private:
        winrt::Microsoft::UI::Xaml::Data::BindingExpression bindingExpression_{nullptr};
        winrt::Microsoft::UI::Xaml::DependencyProperty bindingTarget_{nullptr};
        winrt::Microsoft::UI::Xaml::FrameworkElement element_{nullptr};
    };

}
