#pragma once
#include "DataGridAutoGeneratingColumnEventArgs.g.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct DataGridColumn;

    struct DataGridAutoGeneratingColumnEventArgs : DataGridAutoGeneratingColumnEventArgsT<DataGridAutoGeneratingColumnEventArgs>
    {
        DataGridAutoGeneratingColumnEventArgs(hstring const& propertyName, MiniDumpExplorer::DataGridColumn const& column);

        MiniDumpExplorer::DataGridColumn Column() const { return column_; }
        void Column(MiniDumpExplorer::DataGridColumn const& value) { column_ = value; }
        hstring PropertyName() const { return propertyName_; }

        bool Cancel() const { return cancelled_; }
        void Cancel(bool const value) { cancelled_ = value; }

    private:
        hstring propertyName_;
        MiniDumpExplorer::DataGridColumn column_;
        std::atomic_bool cancelled_{false};
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct DataGridAutoGeneratingColumnEventArgs : DataGridAutoGeneratingColumnEventArgsT<DataGridAutoGeneratingColumnEventArgs, implementation::DataGridAutoGeneratingColumnEventArgs>
    {
    };
}
