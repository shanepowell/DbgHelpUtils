#pragma once

#include "DataGridRowClipboardEventArgs.g.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct DataGridClipboardCellContent;

    struct DataGridRowClipboardEventArgs : DataGridRowClipboardEventArgsT<DataGridRowClipboardEventArgs>
    {
        DataGridRowClipboardEventArgs(Windows::Foundation::IInspectable const& item, bool isColumnHeadersRow);

        Windows::Foundation::IInspectable Item() const { return item_; }
        bool IsColumnHeadersRow() const { return isColumnHeadersRow_; }
        Windows::Foundation::Collections::IVector<MiniDumpExplorer::DataGridClipboardCellContent> ClipboardRowContent() const { return clipboardRowContent_; }

    private:
        Windows::Foundation::IInspectable item_;
        bool isColumnHeadersRow_;
        Windows::Foundation::Collections::IVector<MiniDumpExplorer::DataGridClipboardCellContent> clipboardRowContent_{single_threaded_vector<MiniDumpExplorer::DataGridClipboardCellContent>()};
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct DataGridRowClipboardEventArgs : DataGridRowClipboardEventArgsT<DataGridRowClipboardEventArgs, implementation::DataGridRowClipboardEventArgs>
    {
    };
}
