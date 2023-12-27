#pragma once

#include "DataGridClipboardCellContent.g.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct DataGridColumn;

    struct DataGridClipboardCellContent : DataGridClipboardCellContentT<DataGridClipboardCellContent>
    {
        DataGridClipboardCellContent(Windows::Foundation::IInspectable const& item, MiniDumpExplorer::DataGridColumn const& column, Windows::Foundation::IInspectable const& content);

        Windows::Foundation::IInspectable Item() const { return item_; }
        MiniDumpExplorer::DataGridColumn Column() const { return column_; }
        Windows::Foundation::IInspectable Content() const { return content_; }

        [[nodiscard]] bool operator==(DataGridClipboardCellContent const& other) const noexcept
        {
            return item_ == other.item_ && column_ == other.column_ && content_ == other.content_;
        }

        [[nodiscard]] bool operator!=(DataGridClipboardCellContent const& other) const noexcept
        {
            if (column_ == other.column_ && content_ == other.content_)
            {
                return item_ != other.item_;
            }

            return true;
        }

    private:
        Windows::Foundation::IInspectable item_;
        MiniDumpExplorer::DataGridColumn column_;
        Windows::Foundation::IInspectable content_;
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct DataGridClipboardCellContent : DataGridClipboardCellContentT<DataGridClipboardCellContent, implementation::DataGridClipboardCellContent>
    {
    };
}
