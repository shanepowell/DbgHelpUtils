#pragma once

#include "DataGridColumnHeader.g.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct DataGrid;
    struct DataGridColumn;

    struct DataGridColumnHeader : DataGridColumnHeaderT<DataGridColumnHeader>
    {
        DataGridColumnHeader();

        static Microsoft::UI::Xaml::DependencyProperty SeparatorBrushProperty();
        static Microsoft::UI::Xaml::DependencyProperty SeparatorVisibilityProperty();

        Microsoft::UI::Xaml::Media::Brush SeparatorBrush() const;
        void SeparatorBrush(Microsoft::UI::Xaml::Media::Brush const& value) const;
        Microsoft::UI::Xaml::Visibility SeparatorVisibility() const;
        void SeparatorVisibility(Microsoft::UI::Xaml::Visibility value) const;

        int32_t ColumnIndex() const;
        MiniDumpExplorer::DataGrid OwningGrid() const;
        MiniDumpExplorer::DataGridColumn const& OwningColumn() const { return owningColumn_; }
        void OwningColumn(MiniDumpExplorer::DataGridColumn const& value) { owningColumn_ = value; }
        bool HasFocus() const;

        // ReSharper disable CppHidingFunction
        void OnApplyTemplate() const; // override
        void OnContentChanged(Windows::Foundation::IInspectable const& oldContent, Windows::Foundation::IInspectable const& newContent) const; // override
        Microsoft::UI::Xaml::Automation::Peers::AutomationPeer OnCreateAutomationPeer() const; // override
        // ReSharper restore CppHidingFunction

        void ApplyState(bool useTransitions) const;
        void EnsureStyle(Microsoft::UI::Xaml::Style const& previousStyle) const;
        void InvokeProcessSort();
        void UpdateSeparatorVisibility(MiniDumpExplorer::DataGridColumn const& lastVisibleColumn) const;

    private:
        void OnSeparatorVisibilityPropertyChanged(Microsoft::UI::Xaml::DependencyObject const& d, Microsoft::UI::Xaml::DependencyPropertyChangedEventArgs const& e);

        void ProcessSort() const;
        bool TrySetResizeColumn(uint32_t pointerId, MiniDumpExplorer::DataGridColumn const& column) const;
        static bool CanReorderColumn(MiniDumpExplorer::DataGridColumn const& column);
        void CancelPointer(Microsoft::UI::Xaml::Input::PointerRoutedEventArgs const& e);
        MiniDumpExplorer::DataGridColumn GetReorderingTargetColumn(Windows::Foundation::Point& pointerPositionHeaders, bool scroll, double& scrollAmount) const;
        int32_t GetReorderingTargetDisplayIndex(Windows::Foundation::Point& pointerPositionHeaders) const;

        void DataGridColumnHeader_PointerCanceled(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::Input::PointerRoutedEventArgs const& e);
        void DataGridColumnHeader_PointerCaptureLost(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::Input::PointerRoutedEventArgs const& e);
        void DataGridColumnHeader_PointerPressed(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::Input::PointerRoutedEventArgs const& e);
        void DataGridColumnHeader_PointerReleased(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::Input::PointerRoutedEventArgs const& e);
        void DataGridColumnHeader_PointerMoved(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::Input::PointerRoutedEventArgs const& e);
        void DataGridColumnHeader_PointerEntered(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::Input::PointerRoutedEventArgs const& e);
        void DataGridColumnHeader_PointerExited(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::Input::PointerRoutedEventArgs const& e);
        void DataGridColumnHeader_IsEnabledChanged(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::DependencyPropertyChangedEventArgs const& e);

        void DataGridColumnHeader_DoubleTapped(Microsoft::UI::Xaml::Input::DoubleTappedRoutedEventArgs const& e) const;

        void OnPointerMove_BeginReorder(uint32_t pointerId, Windows::Foundation::Point const& pointerPosition) const;
        void OnPointerMove_Reorder(bool& handled, Microsoft::UI::Xaml::Input::Pointer const& pointer, Windows::Foundation::Point const& pointerPosition, Windows::Foundation::Point& pointerPositionHeaders, double distanceFromLeft, double distanceFromRight) const;
        void OnPointerMove_Resize(bool& handled, Windows::Foundation::Point const& pointerPositionHeaders) const;
        void SetOriginalCursor() const;
        void SetResizeCursor(Microsoft::UI::Xaml::Input::Pointer const& pointer, Windows::Foundation::Point const& pointerPosition) const;

        static bool CanResizeColumn(MiniDumpExplorer::DataGridColumn const& column);

        // ReSharper disable CppFunctionIsNotImplemented
        auto InternalOwningGrid() const;
        auto InternalOwningColumn() const;
        // ReSharper restore CppFunctionIsNotImplemented

    private:
        Microsoft::UI::Xaml::Visibility desiredSeparatorVisibility_{};
        event_token pointerCanceledEvent_{};
        event_token pointerCaptureLostEvent_{};
        event_token pointerPressedEvent_{};
        event_token pointerReleasedEvent_{};
        event_token pointerMovedEvent_{};
        event_token pointerEnteredEvent_{};
        event_token pointerExitedEvent_{};
        event_token isEnabledChangedEvent_{};
        bool isPointerOver_{};
        bool isPressed_{};
        MiniDumpExplorer::DataGridColumn owningColumn_{nullptr};
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct DataGridColumnHeader : DataGridColumnHeaderT<DataGridColumnHeader, implementation::DataGridColumnHeader>
    {
    };
}
