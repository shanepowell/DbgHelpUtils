#pragma once

#include "MiniDumpMemoryDescriptor.g.h"

#include "GlobalOptionsNotifyPropertyChangedBase.h"
#include "DbgHelpUtils/mini_dump.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct MiniDumpLocationDescriptor;

    struct MiniDumpMemoryDescriptor : MiniDumpMemoryDescriptorT<MiniDumpMemoryDescriptor>, GlobalOptionsNotifyPropertyChangedBase<MiniDumpMemoryDescriptor>
    {
        MiniDumpMemoryDescriptor();

        void Set(MINIDUMP_MEMORY_DESCRIPTOR const& memoryDescriptor);

        uint64_t StartOfMemoryRange() const { return memoryDescriptor_.StartOfMemoryRange; }
        uint64_t EndOfMemoryRange() const { return memoryDescriptor_.StartOfMemoryRange + memoryDescriptor_.Memory.DataSize; }
        uint32_t DataSize() const { return memoryDescriptor_.Memory.DataSize; }
        MiniDumpExplorer::MiniDumpLocationDescriptor Memory() const { return memory_; }

    private:
        MINIDUMP_MEMORY_DESCRIPTOR memoryDescriptor_{};
        MiniDumpExplorer::MiniDumpLocationDescriptor memory_;
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct MiniDumpMemoryDescriptor : MiniDumpMemoryDescriptorT<MiniDumpMemoryDescriptor, implementation::MiniDumpMemoryDescriptor>
    {
    };
}
