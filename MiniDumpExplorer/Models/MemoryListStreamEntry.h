#pragma once

#include "MemoryListStreamEntry.g.h"

#include "GlobalOptionsNotifyPropertyChangedBase.h"

// ReSharper disable once CppUnusedIncludeDirective
#include "DbgHelpUtils/windows_setup.h"
#include <DbgHelp.h>

namespace winrt::MiniDumpExplorer::implementation
{
    struct MiniDumpLocationDescriptor;

    struct MemoryListStreamEntry : MemoryListStreamEntryT<MemoryListStreamEntry>, GlobalOptionsNotifyPropertyChangedBase<MemoryListStreamEntry>
    {
        MemoryListStreamEntry();

        uint32_t Index() const { return index_; }
        uint64_t StartOfMemoryRange() const { return memory_range_->StartOfMemoryRange; }
        MiniDumpExplorer::MiniDumpLocationDescriptor Memory() const { return memory_; }

        void Set(uint32_t index, MINIDUMP_MEMORY_DESCRIPTOR const* memory_range);

    private:
        uint32_t index_{};
        MINIDUMP_MEMORY_DESCRIPTOR const* memory_range_{nullptr};
        MiniDumpExplorer::MiniDumpLocationDescriptor memory_{};
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct MemoryListStreamEntry : MemoryListStreamEntryT<MemoryListStreamEntry, implementation::MemoryListStreamEntry>
    {
    };
}
