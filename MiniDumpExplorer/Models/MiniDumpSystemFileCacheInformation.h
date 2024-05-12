#pragma once

#include "MiniDumpSystemFileCacheInformation.g.h"

#include "GlobalOptionsNotifyPropertyChangedBase.h"
#include "DbgHelpUtils/mini_dump.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct MiniDumpSystemFileCacheInformation : MiniDumpSystemFileCacheInformationT<MiniDumpSystemFileCacheInformation>, GlobalOptionsNotifyPropertyChangedBase<MiniDumpSystemFileCacheInformation>
    {
        MiniDumpSystemFileCacheInformation();

        void Set(MINIDUMP_SYSTEM_FILECACHE_INFORMATION const& file_cache_info, bool has_transition_repurpose_count, uint32_t page_size);

        uint64_t CurrentSize() const { return file_cache_info_.CurrentSize; }
        uint64_t PeakSize() const { return file_cache_info_.PeakSize; }
        uint32_t PageFaultCount() const { return file_cache_info_.PageFaultCount; }
        uint64_t MinimumWorkingSet() const { return file_cache_info_.MinimumWorkingSet; }
        uint64_t MaximumWorkingSet() const { return file_cache_info_.MaximumWorkingSet; }
        uint64_t CurrentSizeIncludingTransitionInPages() const { return file_cache_info_.CurrentSizeIncludingTransitionInPages; }
        uint64_t CurrentSizeIncludingTransitionInPagesSize() const { return file_cache_info_.CurrentSizeIncludingTransitionInPages * page_size_; }
        uint64_t PeakSizeIncludingTransitionInPages() const { return file_cache_info_.PeakSizeIncludingTransitionInPages; }
        uint64_t PeakSizeIncludingTransitionInPagesSize() const { return file_cache_info_.PeakSizeIncludingTransitionInPages * page_size_; }
        bool HasTransitionRePurposeCount() const { return has_transition_repurpose_count_; }
        uint32_t TransitionRePurposeCount() const { return file_cache_info_.TransitionRePurposeCount; }
        uint32_t Flags() const { return file_cache_info_.Flags; }

    private:
        MINIDUMP_SYSTEM_FILECACHE_INFORMATION file_cache_info_{};
        bool has_transition_repurpose_count_{};
        uint32_t page_size_{};
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct MiniDumpSystemFileCacheInformation : MiniDumpSystemFileCacheInformationT<MiniDumpSystemFileCacheInformation, implementation::MiniDumpSystemFileCacheInformation>
    {
    };
}
