#pragma once

#include <map>

#include "DataGridComboBoxColumn.g.h"

#include "DataGridBoundColumn.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct DataGridCellEditEndedEventArgs;
    struct DataGridColumn;
    struct DataGridRow;
    struct DataGridRowEventArgs;

    struct DataGridComboBoxColumn : DataGridComboBoxColumnT<DataGridComboBoxColumn, DataGridBoundColumn>
    {
        DataGridComboBoxColumn();

        static Microsoft::UI::Xaml::DependencyProperty ItemsSourceProperty();
        static Microsoft::UI::Xaml::DependencyProperty DisplayMemberPathProperty();
        static Microsoft::UI::Xaml::DependencyProperty FontFamilyProperty();

        Windows::Foundation::Collections::IObservableVector<Windows::Foundation::IInspectable> ItemsSource() const;
        void ItemsSource(Windows::Foundation::Collections::IObservableVector<Windows::Foundation::IInspectable> const& value) const;
        hstring DisplayMemberPath() const;
        void DisplayMemberPath(hstring const& value) const;
        Microsoft::UI::Xaml::Media::FontFamily FontFamily() const;
        void FontFamily(Microsoft::UI::Xaml::Media::FontFamily const& value) const;
        double FontSize() const;
        void FontSize(double value);
        Windows::UI::Text::FontStyle FontStyle() const;
        void FontStyle(Windows::UI::Text::FontStyle const& value);
        Windows::UI::Text::FontWeight FontWeight() const;
        void FontWeight(Windows::UI::Text::FontWeight const& value);
        Microsoft::UI::Xaml::Media::Brush Foreground() const;
        void Foreground(Microsoft::UI::Xaml::Media::Brush const& value);

        void RefreshForeground(Microsoft::UI::Xaml::FrameworkElement const& element, Microsoft::UI::Xaml::Media::Brush const& computedRowForeground) override;

    protected:
        void CancelCellEdit(Microsoft::UI::Xaml::FrameworkElement const& editingElement, Windows::Foundation::IInspectable const& uneditedValue) override;
        Microsoft::UI::Xaml::FrameworkElement GenerateEditingElement(MiniDumpExplorer::DataGridCell const& cell, Windows::Foundation::IInspectable const& dataItem) override;
        Microsoft::UI::Xaml::FrameworkElement GenerateElement(MiniDumpExplorer::DataGridCell const& cell, Windows::Foundation::IInspectable const& dataItem) override;
        Windows::Foundation::IInspectable PrepareCellForEdit(Microsoft::UI::Xaml::FrameworkElement const& editingElement, Microsoft::UI::Xaml::RoutedEventArgs const& editingEventArgs) override;
        void RefreshCellContent(Microsoft::UI::Xaml::FrameworkElement const& element, Microsoft::UI::Xaml::Media::Brush const& computedRowForeground, std::wstring_view const& propertyName) override;

    private:
        void OnItemsSourcePropertyChanged(Microsoft::UI::Xaml::DependencyPropertyChangedEventArgs const& e) const;
        void OnFontFamilyPropertyChanged(Microsoft::UI::Xaml::DependencyPropertyChangedEventArgs const& e) const;

        template<typename T>
        void RefreshControlForeground(T const& control, Microsoft::UI::Xaml::Media::Brush const& computedRowForeground) const
        {
            if (!foreground_)
            {
                if (computedRowForeground)
                {
                    control.Foreground(computedRowForeground);
                }
            }
            else
            {
                control.Foreground(foreground_);
            }
        }

        void SetTextFontSize(Microsoft::UI::Xaml::DependencyObject const& textElement, Microsoft::UI::Xaml::DependencyProperty const& fontSizeProperty) const;
        void UnhookOwningGrid();
        bool EnsureOwningGrid();
        void OwningGrid_LoadingRow(Windows::Foundation::IInspectable const& sender, MiniDumpExplorer::DataGridRowEventArgs const& e);
        void OwningGrid_UnloadingRow(Windows::Foundation::IInspectable const& sender, MiniDumpExplorer::DataGridRowEventArgs const& e);
        void OwningGrid_CellEditEnded(Windows::Foundation::IInspectable const& sender, MiniDumpExplorer::DataGridCellEditEndedEventArgs const& e) const;
        void Columns_CollectionChanged(Windows::Foundation::Collections::IObservableVector<MiniDumpExplorer::DataGridColumn> const& sender, Windows::Foundation::Collections::IVectorChangedEventArgs const& e);
        void SetDisplayMemberPathValue(MiniDumpExplorer::DataGridRow const& row) const;
        hstring GetDisplayValue(Windows::Foundation::IInspectable const& dataItem) const;
        void EnsureColumnBinding(Windows::Foundation::IInspectable const& dataItem) const;
        void HookDataItemPropertyChanged(Windows::Foundation::IInspectable const& dataItem);
        void UnhookDataItemPropertyChanged(Windows::Foundation::IInspectable const& dataItem);
        void DataItem_PropertyChanged(Windows::Foundation::IInspectable const& dataItem, Microsoft::UI::Xaml::Data::PropertyChangedEventArgs const& e) const;

    private:
        std::optional<double> fontSize_;
        MiniDumpExplorer::DataGrid owningGrid_{nullptr};
        event_token owningGridCollectionChangedEvent_{};
        event_token ownerGridLoadingRow_{};
        event_token ownerGridUnloadingRow_{};
        event_token ownerGridCellEditEnded_{};
        std::optional<Windows::UI::Text::FontStyle> fontStyle_{};
        std::optional<Windows::UI::Text::FontWeight> fontWeight_{};
        Microsoft::UI::Xaml::Media::Brush foreground_{nullptr};
        std::map<Windows::Foundation::IInspectable, event_token> notifyingDataItems_{};
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct DataGridComboBoxColumn : DataGridComboBoxColumnT<DataGridComboBoxColumn, implementation::DataGridComboBoxColumn>
    {
    };
}
