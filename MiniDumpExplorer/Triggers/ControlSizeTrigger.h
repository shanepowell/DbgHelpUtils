#pragma once

// ReSharper disable once CppUnusedIncludeDirective
#include <App.xaml.g.h>

#include "ControlSizeTrigger.g.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct ControlSizeTrigger : ControlSizeTriggerT<ControlSizeTrigger>
    {
        ControlSizeTrigger();

        bool CanTrigger() const;
        void CanTrigger(bool value) const;
        static Microsoft::UI::Xaml::DependencyProperty CanTriggerProperty();
        double MaxWidth() const;
        void MaxWidth(double value) const;
        static Microsoft::UI::Xaml::DependencyProperty MaxWidthProperty();
        double MinWidth() const;
        void MinWidth(double value) const;
        static Microsoft::UI::Xaml::DependencyProperty MinWidthProperty();
        double MaxHeight() const;
        void MaxHeight(double value) const;
        static Microsoft::UI::Xaml::DependencyProperty MaxHeightProperty();
        double MinHeight() const;
        void MinHeight(double value) const;
        static Microsoft::UI::Xaml::DependencyProperty MinHeightProperty();
        Microsoft::UI::Xaml::FrameworkElement TargetElement() const;
        void TargetElement(Microsoft::UI::Xaml::FrameworkElement const& value) const;
        static Microsoft::UI::Xaml::DependencyProperty TargetElementProperty();
        bool IsActive() const { return isActive_; }

    private:
        void UpdateTrigger();
        static void OnTargetElementPropertyChanged(Microsoft::UI::Xaml::DependencyObject const& d, Microsoft::UI::Xaml::DependencyPropertyChangedEventArgs const& e);
        void UpdateTargetElement(Microsoft::UI::Xaml::FrameworkElement const& oldValue, Microsoft::UI::Xaml::FrameworkElement const& newValue);

    private:
        bool isActive_{ false };
        event_token sizeChangedToken_;
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct ControlSizeTrigger : ControlSizeTriggerT<ControlSizeTrigger, implementation::ControlSizeTrigger>
    {
    };
}
