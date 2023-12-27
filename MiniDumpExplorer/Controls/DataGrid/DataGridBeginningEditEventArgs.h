#pragma once

#include "DataGridBeginningEditEventArgs.g.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct DataGridColumn;
    struct DataGridRow;

    struct DataGridBeginningEditEventArgs : DataGridBeginningEditEventArgsT<DataGridBeginningEditEventArgs>
    {
        DataGridBeginningEditEventArgs(MiniDumpExplorer::DataGridColumn const& column, MiniDumpExplorer::DataGridRow const& row, Microsoft::UI::Xaml::RoutedEventArgs const& editingEventArgs);

        MiniDumpExplorer::DataGridColumn Column() const;
        MiniDumpExplorer::DataGridRow Row() const;
        Microsoft::UI::Xaml::RoutedEventArgs EditingEventArgs();
        bool Cancel() const { return cancelled_; }
        void Cancel(bool const value) { cancelled_ = value; }

    private:
        MiniDumpExplorer::DataGridColumn column_;
        MiniDumpExplorer::DataGridRow row_;
        Microsoft::UI::Xaml::RoutedEventArgs editingEventArgs_;
        std::atomic_bool cancelled_{false};
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct DataGridBeginningEditEventArgs : DataGridBeginningEditEventArgsT<DataGridBeginningEditEventArgs, implementation::DataGridBeginningEditEventArgs>
    {
    };
}
