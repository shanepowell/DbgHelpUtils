#pragma once

#include "DataGridRowHeader.g.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct DataGrid;
    struct DataGridRow;
    struct DataGridRowGroupHeader;

    struct DataGridRowHeader : DataGridRowHeaderT<DataGridRowHeader>
    {
        static constexpr auto InvalidIndex = int32_t{-1};

        DataGridRowHeader();

        static Microsoft::UI::Xaml::DependencyProperty SeparatorBrushProperty();
        static Microsoft::UI::Xaml::DependencyProperty SeparatorVisibilityProperty();

        Microsoft::UI::Xaml::Media::Brush SeparatorBrush() const;
        void SeparatorBrush(Microsoft::UI::Xaml::Media::Brush const& value) const;
        Microsoft::UI::Xaml::Visibility SeparatorVisibility() const;
        void SeparatorVisibility(Microsoft::UI::Xaml::Visibility value) const;

        void OnApplyTemplate(); // override
        // ReSharper disable once CppHidingFunction
        Windows::Foundation::Size MeasureOverride(Windows::Foundation::Size const& availableSize) const; // override
        // ReSharper disable once CppHidingFunction
        Microsoft::UI::Xaml::Automation::Peers::AutomationPeer OnCreateAutomationPeer() const; // override

        void ApplyOwnerState(bool animate) const;
        void EnsureStyle(Microsoft::UI::Xaml::Style const& previousStyle) const;

        Microsoft::UI::Xaml::Controls::Control const& Owner() const { return owner_; }
        void Owner(Microsoft::UI::Xaml::Controls::Control const& value) { owner_ = value; }

    private:
        MiniDumpExplorer::DataGrid OwningGrid() const;
        MiniDumpExplorer::DataGridRow OwningRow() const;
        MiniDumpExplorer::DataGridRowGroupHeader OwningRowGroupHeader() const;

        int32_t Slot() const;

        void DataGridRowHeader_Tapped(Microsoft::UI::Xaml::Input::TappedRoutedEventArgs const& e) const;

    private:
        Microsoft::UI::Xaml::Controls::Control owner_{nullptr};
        Microsoft::UI::Xaml::FrameworkElement rootElement_{nullptr};
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct DataGridRowHeader : DataGridRowHeaderT<DataGridRowHeader, implementation::DataGridRowHeader>
    {
    };
}
