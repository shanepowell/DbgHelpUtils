#pragma once

#include "Memory64ListStreamEntry.g.h"

#include "GlobalOptionsNotifyPropertyChangedBase.h"
#include "DbgHelpUtils/memory64_list_stream.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct Memory64ListStreamEntry : Memory64ListStreamEntryT<Memory64ListStreamEntry>, GlobalOptionsNotifyPropertyChangedBase<Memory64ListStreamEntry>
    {
        Memory64ListStreamEntry();

        uint32_t Index() const { return index_; }
        uint64_t StartOfMemoryRange() const { return memory_range_.start_of_memory_range; }
        uint64_t EndOfMemoryRange() const { return memory_range_.end_of_memory_range; }
        MiniDumpExplorer::MiniDumpLocationDescriptor64 Location() const { return location_; }

        void Set(uint32_t index, dlg_help_utils::memory64_entry const& memory_range);

    private:
        uint32_t index_{};
        dlg_help_utils::memory64_entry memory_range_{};
        MiniDumpExplorer::MiniDumpLocationDescriptor64 location_{};
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct Memory64ListStreamEntry : Memory64ListStreamEntryT<Memory64ListStreamEntry, implementation::Memory64ListStreamEntry>
    {
    };
}
