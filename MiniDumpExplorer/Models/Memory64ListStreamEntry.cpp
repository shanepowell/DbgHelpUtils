#include "pch.h"
#include "Memory64ListStreamEntry.h"

#include "MiniDumpLocationDescriptor64.h"

#if __has_include("Memory64ListStreamEntry.g.cpp")
// ReSharper disable once CppUnusedIncludeDirective
#include "Memory64ListStreamEntry.g.cpp"  // NOLINT(bugprone-suspicious-include)
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    Memory64ListStreamEntry::Memory64ListStreamEntry()
        : GlobalOptionsNotifyPropertyChangedBase(
            {
                L"StartOfMemoryRange",
                L"EndOfMemoryRange"
            }, 
            {
            })
    {
    }

    void Memory64ListStreamEntry::Set(uint32_t const index, dlg_help_utils::memory64_entry const& memory_range)
    {
        index_ = index;
        memory_range_ = memory_range;

        location_.as<MiniDumpLocationDescriptor64>()->Set(memory_range_.location);
    }
}
