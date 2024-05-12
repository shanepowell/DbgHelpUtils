#include "pch.h"
#include "MiniDumpSystemBasicPerformanceInformation.h"

#if __has_include("MiniDumpSystemBasicPerformanceInformation.g.cpp")
// ReSharper disable once CppUnusedIncludeDirective
#include "MiniDumpSystemBasicPerformanceInformation.g.cpp"  // NOLINT(bugprone-suspicious-include)
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    MiniDumpSystemBasicPerformanceInformation::MiniDumpSystemBasicPerformanceInformation()
        : GlobalOptionsNotifyPropertyChangedBase(
            {
            },
            {
                L"AvailablePagesSize",
                L"CommittedPagesSize",
                L"CommitLimitSize",
                L"PeakCommitmentSize"
            },
            {
            },
            {
            })
    {
    }

    void MiniDumpSystemBasicPerformanceInformation::Set(MINIDUMP_SYSTEM_BASIC_PERFORMANCE_INFORMATION const& basic_perf_info, bool const has_basic_perf_info, uint32_t const page_size)
    {
        basic_perf_info_ = basic_perf_info;
        has_basic_perf_info_ = has_basic_perf_info;
        page_size_ = page_size;
    }
}
