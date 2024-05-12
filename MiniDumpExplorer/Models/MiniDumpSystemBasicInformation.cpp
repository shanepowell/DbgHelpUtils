#include "pch.h"
#include "MiniDumpSystemBasicInformation.h"

#if __has_include("MiniDumpSystemBasicInformation.g.cpp")
// ReSharper disable once CppUnusedIncludeDirective
#include "MiniDumpSystemBasicInformation.g.cpp"  // NOLINT(bugprone-suspicious-include)
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    MiniDumpSystemBasicInformation::MiniDumpSystemBasicInformation()
        : GlobalOptionsNotifyPropertyChangedBase(
            {
                L"MinimumUserModeAddress",
                L"MaximumUserModeAddress",
                L"ActiveProcessorsAffinityMask",
            },
            {
                L"PageSize",
                L"NumberOfPhysicalPagesSize",
                L"AllocationGranularity"
            },
            {
            },
            {
            })
    {
    }

    void MiniDumpSystemBasicInformation::Set(MINIDUMP_SYSTEM_BASIC_INFORMATION const& basic_info)
    {
        basic_info_ = basic_info;
    }
}
