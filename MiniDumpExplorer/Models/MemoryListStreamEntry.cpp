#include "pch.h"
#include "MemoryListStreamEntry.h"

#include "MiniDumpLocationDescriptor.h"

#if __has_include("MemoryListStreamEntry.g.cpp")
// ReSharper disable once CppUnusedIncludeDirective
#include "MemoryListStreamEntry.g.cpp"  // NOLINT(bugprone-suspicious-include)
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    MemoryListStreamEntry::MemoryListStreamEntry()
        : GlobalOptionsNotifyPropertyChangedBase(
            {
                L"StartOfMemoryRange"
            }, 
            {
            }, 
            {
            },
            {
            },
            {
            },
            {
            })
    {
    }

    void MemoryListStreamEntry::Set(uint32_t const index, MINIDUMP_MEMORY_DESCRIPTOR const* memory_range)
    {
        index_ = index;
        memory_range_ = memory_range;

        memory_.as<MiniDumpLocationDescriptor>()->Set(memory_range->Memory);
    }
}
