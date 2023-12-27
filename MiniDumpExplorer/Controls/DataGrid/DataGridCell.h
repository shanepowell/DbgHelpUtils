#pragma once

#include "DataGridCell.g.h"

#include "DataGridInteractionInfo.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct DataGrid;
    struct DataGridColumn;
    struct DataGridRow;

    struct DataGridCell : DataGridCellT<DataGridCell>
    {
        DataGridCell();

        static constexpr auto InvalidIndex = int32_t{-1};

        static Microsoft::UI::Xaml::DependencyProperty IsValidProperty();

        bool IsValid() const;
        void IsValid(bool value) const;

        double ActualRightGridLineWidth() const;
        int32_t ColumnIndex() const;
        bool IsCurrent() const;
        bool IsPointerOver() const;
        void IsPointerOver(bool value);

        MiniDumpExplorer::DataGridColumn const& OwningColumn() const { return owningColumn_; }
        void OwningColumn(MiniDumpExplorer::DataGridColumn const& owningColumn);
        MiniDumpExplorer::DataGridRow const& OwningRow() const { return owningRow_; }
        void OwningRow(MiniDumpExplorer::DataGridRow const& owningRow);
        MiniDumpExplorer::DataGrid OwningGrid() const;

        // ReSharper disable CppFunctionIsNotImplemented
        auto InternalOwningGrid() const;
        auto InternalOwningColumn() const;
        auto InternalOwningRow() const;
        // ReSharper restore CppFunctionIsNotImplemented

        int32_t RowIndex() const;

        void OnApplyTemplate(); // override
        // ReSharper disable once CppHidingFunction
        Microsoft::UI::Xaml::Automation::Peers::AutomationPeer OnCreateAutomationPeer() const; // override

        void ApplyCellState(bool animate) const;
        void EnsureGridLine(MiniDumpExplorer::DataGridColumn const& lastVisibleColumn) const;
        void EnsureStyle(Microsoft::UI::Xaml::Style const& previousStyle) const;
        void Recycle();

    private:
        bool IsEdited() const;
        void CancelPointer(Microsoft::UI::Xaml::Input::PointerRoutedEventArgs const& e);

        void OnIsValidPropertyChanged(Microsoft::UI::Xaml::DependencyPropertyChangedEventArgs const& e) const;
        void DataGridCell_PointerTapped(Microsoft::UI::Xaml::Input::TappedRoutedEventArgs const& e) const;
        void DataGridCell_PointerCanceled(Microsoft::UI::Xaml::Input::PointerRoutedEventArgs const& e);
        void DataGridCell_PointerCaptureLost(Microsoft::UI::Xaml::Input::PointerRoutedEventArgs const& e);
        void DataGridCell_PointerPressed(Microsoft::UI::Xaml::Input::PointerRoutedEventArgs const& e);
        void DataGridCell_PointerReleased(Microsoft::UI::Xaml::Input::PointerRoutedEventArgs const& e) const;
        void DataGridCell_PointerEntered(Microsoft::UI::Xaml::Input::PointerRoutedEventArgs const& e);
        void DataGridCell_PointerExited(Microsoft::UI::Xaml::Input::PointerRoutedEventArgs const& e);
        void DataGridCell_PointerMoved(Microsoft::UI::Xaml::Input::PointerRoutedEventArgs const& e);

        void UpdateIsPointerOver(bool isPointerOver);

    private:
        MiniDumpExplorer::DataGridColumn owningColumn_{ nullptr };
        MiniDumpExplorer::DataGridRow owningRow_{ nullptr };
        Microsoft::UI::Xaml::Shapes::Rectangle rightGridLine_{ nullptr };
        std::optional<DataGridInternal::DataGridInteractionInfo> interactionInfo_{};
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct DataGridCell : DataGridCellT<DataGridCell, implementation::DataGridCell>
    {
    };
}
