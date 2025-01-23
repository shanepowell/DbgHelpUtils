#include "pch.h"
#include "MiniDumpMemoryDescriptor.h"

#include "MiniDumpLocationDescriptor.h"

#if __has_include("MiniDumpMemoryDescriptor.g.cpp")
// ReSharper disable once CppUnusedIncludeDirective
#include "MiniDumpMemoryDescriptor.g.cpp"  // NOLINT(bugprone-suspicious-include)
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    MiniDumpMemoryDescriptor::MiniDumpMemoryDescriptor()
        : GlobalOptionsNotifyPropertyChangedBase(
            {
                L"StartOfMemoryRange",
                L"EndOfMemoryRange"
            },
            {
                L"DataSize"
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

    void MiniDumpMemoryDescriptor::Set(MINIDUMP_MEMORY_DESCRIPTOR const& memoryDescriptor)
    {
       memoryDescriptor_ = memoryDescriptor;
       memory_.as<MiniDumpLocationDescriptor>()->Set(memoryDescriptor.Memory);
    }
}
