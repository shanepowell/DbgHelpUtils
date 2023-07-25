#include "pch.h"
#include "ControlSizeTrigger.h"

#include "App.xaml.h"

#if __has_include("ControlSizeTrigger.g.cpp")
// ReSharper disable once CppUnusedIncludeDirective
#include "ControlSizeTrigger.g.cpp"  // NOLINT(bugprone-suspicious-include)
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    ControlSizeTrigger::ControlSizeTrigger() = default;

    bool ControlSizeTrigger::CanTrigger() const
    {
        return unbox_value<bool>(GetValue(CanTriggerProperty()));
    }

    void ControlSizeTrigger::CanTrigger(bool const value) const
    {
        SetValue(CanTriggerProperty(), box_value(value));
    }

    DependencyProperty ControlSizeTrigger::CanTriggerProperty()
    {
        static DependencyProperty s_canTriggerProperty =
            DependencyProperty::Register(
                L"CanTrigger",
                winrt::xaml_typename<bool>(),
                winrt::xaml_typename<MiniDumpExplorer::ControlSizeTrigger>(),
                PropertyMetadata{ box_value(true), [](auto const& d, [[maybe_unused]] auto const& e) { d.template as<ControlSizeTrigger>()->UpdateTrigger(); } });

        return s_canTriggerProperty;
    }

    double ControlSizeTrigger::MaxWidth() const
    {
        return unbox_value<double>(GetValue(MaxWidthProperty()));
    }

    void ControlSizeTrigger::MaxWidth(double const value) const
    {
        SetValue(MaxWidthProperty(), box_value(value));
    }

    DependencyProperty ControlSizeTrigger::MaxWidthProperty()
    {
        static DependencyProperty s_maxWidthProperty =
            DependencyProperty::Register(
                L"MaxWidth",
                winrt::xaml_typename<double>(),
                winrt::xaml_typename<MiniDumpExplorer::ControlSizeTrigger>(),
                PropertyMetadata{ box_value(0.0), [](auto const& d, [[maybe_unused]] auto const& e) { d.template as<ControlSizeTrigger>()->UpdateTrigger(); } });

        return s_maxWidthProperty;
    }

    double ControlSizeTrigger::MinWidth() const
    {
        return unbox_value<double>(GetValue(MinWidthProperty()));
    }

    void ControlSizeTrigger::MinWidth(double const value) const
    {
        SetValue(MinWidthProperty(), box_value(value));
    }

    DependencyProperty ControlSizeTrigger::MinWidthProperty()
    {
        static DependencyProperty s_minWidthProperty =
            DependencyProperty::Register(
                L"MinWidth",
                winrt::xaml_typename<double>(),
                winrt::xaml_typename<MiniDumpExplorer::ControlSizeTrigger>(),
                PropertyMetadata{ box_value(0.0), [](auto const& d, [[maybe_unused]] auto const& e) { d.template as<ControlSizeTrigger>()->UpdateTrigger(); } });

        return s_minWidthProperty;
    }

    double ControlSizeTrigger::MaxHeight() const
    {
        return unbox_value<double>(GetValue(MaxHeightProperty()));
    }

    void ControlSizeTrigger::MaxHeight(double const value) const
    {
        SetValue(MaxHeightProperty(), box_value(value));
    }

    DependencyProperty ControlSizeTrigger::MaxHeightProperty()
    {
        static DependencyProperty s_maxHeightProperty =
            DependencyProperty::Register(
                L"MaxHeight",
                winrt::xaml_typename<double>(),
                winrt::xaml_typename<MiniDumpExplorer::ControlSizeTrigger>(),
                PropertyMetadata{ box_value(0.0), [](auto const& d, [[maybe_unused]] auto const& e) { d.template as<ControlSizeTrigger>()->UpdateTrigger(); } });

        return s_maxHeightProperty;
    }

    double ControlSizeTrigger::MinHeight() const
    {
        return unbox_value<double>(GetValue(MinHeightProperty()));
    }

    void ControlSizeTrigger::MinHeight(double const value) const
    {
        SetValue(MinHeightProperty(), box_value(value));
    }

    DependencyProperty ControlSizeTrigger::MinHeightProperty()
    {
        static DependencyProperty s_minHeightProperty =
            DependencyProperty::Register(
                L"MinHeight",
                winrt::xaml_typename<double>(),
                winrt::xaml_typename<MiniDumpExplorer::ControlSizeTrigger>(),
                PropertyMetadata{ box_value(0.0), [](auto const& d, [[maybe_unused]] auto const& e) { d.template as<ControlSizeTrigger>()->UpdateTrigger(); } });

        return s_minHeightProperty;
    }

    FrameworkElement ControlSizeTrigger::TargetElement() const
    {
        return GetValue(TargetElementProperty()).as<FrameworkElement>();
    }

    void ControlSizeTrigger::TargetElement(FrameworkElement const& value) const
    {
        SetValue(TargetElementProperty(), value);
    }

    DependencyProperty ControlSizeTrigger::TargetElementProperty()
    {
        static DependencyProperty s_targetElementProperty =
            DependencyProperty::Register(
                L"TargetElement",
                winrt::xaml_typename<FrameworkElement>(),
                winrt::xaml_typename<MiniDumpExplorer::ControlSizeTrigger>(),
                PropertyMetadata{ nullptr, OnTargetElementPropertyChanged });

        return s_targetElementProperty;
    }

    void ControlSizeTrigger::OnTargetElementPropertyChanged(DependencyObject const& d, DependencyPropertyChangedEventArgs const& e)
    {
        d.as<ControlSizeTrigger>()->UpdateTargetElement(e.OldValue().as<FrameworkElement>(), e.NewValue().as<FrameworkElement>());
    }

    void ControlSizeTrigger::UpdateTargetElement(FrameworkElement const& oldValue, FrameworkElement const& newValue)
    {
        if (oldValue != nullptr)
        {
            oldValue.SizeChanged(sizeChangedToken_);
        }

        if (newValue != nullptr)
        {
            sizeChangedToken_ = newValue.SizeChanged([this]([[maybe_unused]] auto const& sender, [[maybe_unused]] auto const& e) { UpdateTrigger(); });
        }

        UpdateTrigger();
    }

    // Logic to evaluate and apply trigger value
    void ControlSizeTrigger::UpdateTrigger()
    {
        auto const& targetElement = TargetElement();
        if (targetElement == nullptr || !CanTrigger())
        {
            SetActive(false);
            return;
        }

        bool const activate = MinWidth() <= targetElement.ActualWidth() &&
                        targetElement.ActualWidth() < MaxWidth() &&
                        MinHeight() <= targetElement.ActualHeight() &&
                        targetElement.ActualHeight() < MaxHeight();

        isActive_ = activate;
        SetActive(activate);
    }
}
