#include "pch.h"
#include "MiniDumpSystemFileCacheInformation.h"

#if __has_include("MiniDumpSystemFileCacheInformation.g.cpp")
// ReSharper disable once CppUnusedIncludeDirective
#include "MiniDumpSystemFileCacheInformation.g.cpp"  // NOLINT(bugprone-suspicious-include)
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    MiniDumpSystemFileCacheInformation::MiniDumpSystemFileCacheInformation()
        : GlobalOptionsNotifyPropertyChangedBase(
            {
                L"Flags"
            },
            {
                L"CurrentSize",
                L"PeakSize",
                L"MinimumWorkingSet",
                L"MaximumWorkingSet",
                L"CurrentSizeIncludingTransitionInPagesSize",
                L"PeakSizeIncludingTransitionInPagesSize"
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

    void MiniDumpSystemFileCacheInformation::Set(MINIDUMP_SYSTEM_FILECACHE_INFORMATION const& file_cache_info, bool const has_transition_repurpose_count, uint32_t const page_size)
    {
        file_cache_info_ = file_cache_info;
        has_transition_repurpose_count_ = has_transition_repurpose_count;
        page_size_ = page_size;
    }
}
