#pragma once

#include "DataGridTemplateColumn.g.h"

#include "DataGridColumn.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct DataGridCell;

    struct DataGridTemplateColumn : DataGridTemplateColumnT<DataGridTemplateColumn, DataGridColumn>
    {
        DataGridTemplateColumn();

        Microsoft::UI::Xaml::DataTemplate CellEditingTemplate() const { return cellEditingTemplate_; }
        void CellEditingTemplate(Microsoft::UI::Xaml::DataTemplate const& value);

        Microsoft::UI::Xaml::DataTemplate CellTemplate() const { return cellTemplate_; }
        void CellTemplate(Microsoft::UI::Xaml::DataTemplate const& value);

        bool HasDistinctTemplates() const;

    protected:
        void CancelCellEdit(Microsoft::UI::Xaml::FrameworkElement const& editingElement, Windows::Foundation::IInspectable const& uneditedValue) override;
        Microsoft::UI::Xaml::FrameworkElement GenerateEditingElement(MiniDumpExplorer::DataGridCell const& cell, Windows::Foundation::IInspectable const& dataItem) override;
        Microsoft::UI::Xaml::FrameworkElement GenerateElement(MiniDumpExplorer::DataGridCell const& cell, Windows::Foundation::IInspectable const& dataItem) override;
        Windows::Foundation::IInspectable PrepareCellForEdit(Microsoft::UI::Xaml::FrameworkElement const& editingElement, Microsoft::UI::Xaml::RoutedEventArgs const& editingEventArgs) override;

    private:
        Microsoft::UI::Xaml::DataTemplate cellEditingTemplate_{nullptr};
        Microsoft::UI::Xaml::DataTemplate cellTemplate_{nullptr};
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct DataGridTemplateColumn : DataGridTemplateColumnT<DataGridTemplateColumn, implementation::DataGridTemplateColumn>
    {
    };
}
