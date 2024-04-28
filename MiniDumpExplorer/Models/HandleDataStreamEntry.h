#pragma once

#include "HandleDataStreamEntry.g.h"

#include "NotifyPropertyChangedBase.h"
#include "DbgHelpUtils/handle_data_stream.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct HandleDataStreamEntry : HandleDataStreamEntryT<HandleDataStreamEntry>, NotifyPropertyChangedBase<HandleDataStreamEntry>
    {
        HandleDataStreamEntry();

        uint32_t Index() const { return index_; }
        hstring Type() const { return type_; }
        uint32_t TotalHandles() const { return totalHandles_; }

        void Set(uint32_t index, dlg_help_utils::handle_data_stream const& handle_data, std::wstring_view const& type, uint32_t total_handles);

    private:
        uint32_t index_{};
        dlg_help_utils::handle_data_stream handle_data_{};
        hstring type_{};
        uint32_t totalHandles_{};
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct HandleDataStreamEntry : HandleDataStreamEntryT<HandleDataStreamEntry, implementation::HandleDataStreamEntry>
    {
    };
}
