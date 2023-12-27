#pragma once

#include "DataGridPreparingCellForEditEventArgs.g.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct DataGridColumn;
    struct DataGridRow;

    struct DataGridPreparingCellForEditEventArgs : DataGridPreparingCellForEditEventArgsT<DataGridPreparingCellForEditEventArgs>
    {
        DataGridPreparingCellForEditEventArgs(MiniDumpExplorer::DataGridColumn const& column, MiniDumpExplorer::DataGridRow const& row, Microsoft::UI::Xaml::RoutedEventArgs const& editingEventArgs, Microsoft::UI::Xaml::FrameworkElement const& editingElement);

        MiniDumpExplorer::DataGridColumn Column() const { return column_; }
        MiniDumpExplorer::DataGridRow Row() const { return row_; }
        Microsoft::UI::Xaml::RoutedEventArgs EditingEventArgs() const { return editingEventArgs_; }
        Microsoft::UI::Xaml::FrameworkElement EditingElement() const { return editingElement_; }

    private:
        MiniDumpExplorer::DataGridColumn column_;
        MiniDumpExplorer::DataGridRow row_;
        Microsoft::UI::Xaml::RoutedEventArgs editingEventArgs_;
        Microsoft::UI::Xaml::FrameworkElement editingElement_;
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct DataGridPreparingCellForEditEventArgs : DataGridPreparingCellForEditEventArgsT<DataGridPreparingCellForEditEventArgs, implementation::DataGridPreparingCellForEditEventArgs>
    {
    };
}
