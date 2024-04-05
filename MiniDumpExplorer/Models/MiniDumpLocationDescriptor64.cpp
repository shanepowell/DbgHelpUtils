#include "pch.h"
#include "MiniDumpLocationDescriptor64.h"

#if __has_include("MiniDumpLocationDescriptor64.g.cpp")
// ReSharper disable once CppUnusedIncludeDirective
#include "MiniDumpLocationDescriptor64.g.cpp"  // NOLINT(bugprone-suspicious-include)
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    MiniDumpLocationDescriptor64::MiniDumpLocationDescriptor64()
        : GlobalOptionsNotifyPropertyChangedBase({ L"Rva" }, { L"DataSize" })
    {
    }

    void MiniDumpLocationDescriptor64::Set(MINIDUMP_LOCATION_DESCRIPTOR64 const& locationDescriptor)
    {
       locationDescriptor_ = locationDescriptor;
    }
}
