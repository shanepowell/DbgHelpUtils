#include "pch.h"
#include "MinidumpSystemPerformanceInformation.h"

#if __has_include("MinidumpSystemPerformanceInformation.g.cpp")
// ReSharper disable once CppUnusedIncludeDirective
#include "MinidumpSystemPerformanceInformation.g.cpp"  // NOLINT(bugprone-suspicious-include)
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace winrt::MiniDumpExplorer::implementation
{
    MinidumpSystemPerformanceInformation::MinidumpSystemPerformanceInformation()
        : GlobalOptionsNotifyPropertyChangedBase(
            {
            },
            {
                L"AvailablePagesSize",
                L"CommittedPagesSize",
                L"CommitLimitSize",
                L"PeakCommitmentSize",
                L"PagedPoolPagesSize",
                L"NonPagedPoolPagesSize",
                L"FreeSystemPtesSize",
                L"ResidentSystemCodePageSize",
                L"TotalSystemDriverPagesSize",
                L"TotalSystemCodePagesSize",
                L"AvailablePagedPoolPagesSize",
                L"ResidentSystemCachePageSize",
                L"ResidentPagedPoolPageSize",
                L"ResidentSystemDriverPageSize",
                L"CcLazyWritePagesSize",
                L"CcDataPagesSize",
                L"CcTotalDirtyPagesSize",
                L"ResidentAvailablePagesSize",
                L"SharedCommittedPagesSize",
            },
            {
            },
            {
                L"IdleProcessTime"
            },
            {
            },
            {
            })
    {
    }

    void MinidumpSystemPerformanceInformation::Set(MINIDUMP_SYSTEM_PERFORMANCE_INFORMATION const& perf_info, bool const has_cc_total_dirty_pages_cc_dirty_page_threshold, bool const has_resident_available_pages_shared_committed_pages, uint32_t const page_size)
    {
        perf_info_ = perf_info;
        has_cc_total_dirty_pages_cc_dirty_page_threshold_ = has_cc_total_dirty_pages_cc_dirty_page_threshold;
        has_resident_available_pages_shared_committed_pages_ = has_resident_available_pages_shared_committed_pages;
        page_size_ = page_size;
    }
}
