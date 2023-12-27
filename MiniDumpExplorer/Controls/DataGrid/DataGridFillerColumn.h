#pragma once

#include "DataGridFillerColumn.g.h"
#include "DataGridColumn.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct DataGridCell;

    struct DataGridFillerColumn : DataGridFillerColumnT<DataGridFillerColumn, DataGridColumn>
    {
        DataGridFillerColumn(MiniDumpExplorer::DataGrid const& owningGrid);

        double FillerWidth() const { return fillerWidth_; }
        void FillerWidth(double const value) { fillerWidth_ = value; }

        bool IsActive() const { return FillerWidth() > 0; }

        bool IsRepresented() const { return isRepresented_; }
        void IsRepresented(bool const value) { isRepresented_ = value; }

        MiniDumpExplorer::DataGridColumnHeader CreateHeader() override;
        Microsoft::UI::Xaml::FrameworkElement GenerateEditingElement(MiniDumpExplorer::DataGridCell const& cell, Windows::Foundation::IInspectable const& dataItem) override;
        Microsoft::UI::Xaml::FrameworkElement GenerateElement(MiniDumpExplorer::DataGridCell const& cell, Windows::Foundation::IInspectable const& dataItem) override;
        Windows::Foundation::IInspectable PrepareCellForEdit(Microsoft::UI::Xaml::FrameworkElement const& editingElement, Microsoft::UI::Xaml::RoutedEventArgs const& editingEventArgs) override;

        bool IsDataGridFillerColumn() const override { return true; }

    private:
        double fillerWidth_{};
        bool isRepresented_{};
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct DataGridFillerColumn : DataGridFillerColumnT<DataGridFillerColumn, implementation::DataGridFillerColumn>
    {
    };
}
