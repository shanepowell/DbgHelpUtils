#include "pch.h"
#include "MiniDumpLocationDescriptor.h"

#if __has_include("MiniDumpLocationDescriptor.g.cpp")
// ReSharper disable once CppUnusedIncludeDirective
#include "MiniDumpLocationDescriptor.g.cpp"  // NOLINT(bugprone-suspicious-include)
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    MiniDumpLocationDescriptor::MiniDumpLocationDescriptor()
        : GlobalOptionsNotifyPropertyChangedBase(
            {
                L"Rva"
            },
            {
                L"DataSize"
            },
            {
            },
            {
            })
    {
    }

    void MiniDumpLocationDescriptor::Set(MINIDUMP_LOCATION_DESCRIPTOR const& locationDescriptor)
    {
       locationDescriptor_ = locationDescriptor;
    }
}
