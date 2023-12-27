#pragma once

#include "DataGridRowDetailsEventArgs.g.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct DataGridRow;

    struct DataGridRowDetailsEventArgs : DataGridRowDetailsEventArgsT<DataGridRowDetailsEventArgs>
    {
        DataGridRowDetailsEventArgs(MiniDumpExplorer::DataGridRow const& row, Microsoft::UI::Xaml::FrameworkElement const& detailsElement);

        MiniDumpExplorer::DataGridRow Row() const { return row_; }
        Microsoft::UI::Xaml::FrameworkElement DetailsElement() const { return detailsElement_; }

    private:
        MiniDumpExplorer::DataGridRow row_;
        Microsoft::UI::Xaml::FrameworkElement detailsElement_;
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct DataGridRowDetailsEventArgs : DataGridRowDetailsEventArgsT<DataGridRowDetailsEventArgs, implementation::DataGridRowDetailsEventArgs>
    {
    };
}
