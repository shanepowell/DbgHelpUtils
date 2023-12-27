#pragma once

#include "DataGridCheckBoxColumn.g.h"

#include "DataGridBoundColumn.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct DataGridCell;
    struct DataGridRowEventArgs;

    struct DataGridCheckBoxColumn : DataGridCheckBoxColumnT<DataGridCheckBoxColumn, DataGridBoundColumn>
    {
        DataGridCheckBoxColumn();

        bool IsThreeState() const { return isThreeState_; }
        void IsThreeState(bool value);

        void RefreshForeground(Microsoft::UI::Xaml::FrameworkElement const& element, Microsoft::UI::Xaml::Media::Brush const& computedRowForeground) override;

    protected:
        void CancelCellEdit(Microsoft::UI::Xaml::FrameworkElement const& editingElement, Windows::Foundation::IInspectable const& uneditedValue) override;
        Microsoft::UI::Xaml::FrameworkElement GenerateEditingElement(MiniDumpExplorer::DataGridCell const& cell, Windows::Foundation::IInspectable const& dataItem) override;
        Microsoft::UI::Xaml::FrameworkElement GenerateElement(MiniDumpExplorer::DataGridCell const& cell, Windows::Foundation::IInspectable const& dataItem) override;
        Windows::Foundation::IInspectable PrepareCellForEdit(Microsoft::UI::Xaml::FrameworkElement const& editingElement, Microsoft::UI::Xaml::RoutedEventArgs const& editingEventArgs) override;
        void RefreshCellContent(Microsoft::UI::Xaml::FrameworkElement const& element, Microsoft::UI::Xaml::Media::Brush const& computedRowForeground, std::wstring_view const& propertyName) override;

    private:
        void Columns_CollectionChanged(Windows::Foundation::IInspectable const& sender, Windows::Foundation::Collections::IVectorChangedEventArgs const& e);
        void ConfigureCheckBox(Microsoft::UI::Xaml::Controls::CheckBox const& checkBox, Microsoft::UI::Xaml::Media::Brush const& computedRowForeground) const;
        bool EnsureOwningGrid();
        void OwningGrid_CurrentCellChanged(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void OwningGrid_KeyDown(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::Input::KeyRoutedEventArgs const& e);
        void OwningGrid_LoadingRow(Windows::Foundation::IInspectable const& sender, MiniDumpExplorer::DataGridRowEventArgs const& e);

    private:
        bool beganEditWithKeyboard_{false};
        bool isThreeState_{false};
        Microsoft::UI::Xaml::Controls::CheckBox currentCheckBox_{nullptr};
        MiniDumpExplorer::DataGrid owningGrid_{nullptr};
        event_token columnsCollectionChangedToken_{};
        event_token currentCellChangedToken_{};
        event_token keyDownToken_{};
        event_token loadingRowToken_{};
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct DataGridCheckBoxColumn : DataGridCheckBoxColumnT<DataGridCheckBoxColumn, implementation::DataGridCheckBoxColumn>
    {
    };
}
