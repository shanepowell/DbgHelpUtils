#pragma once

#include "MinidumpSystemPerformanceInformation.g.h"

#include "GlobalOptionsNotifyPropertyChangedBase.h"
#include "DbgHelpUtils/mini_dump.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct MinidumpSystemPerformanceInformation : MinidumpSystemPerformanceInformationT<MinidumpSystemPerformanceInformation>, GlobalOptionsNotifyPropertyChangedBase<MinidumpSystemPerformanceInformation>
    {
        MinidumpSystemPerformanceInformation();

        void Set(MINIDUMP_SYSTEM_PERFORMANCE_INFORMATION const& perf_info, bool has_cc_total_dirty_pages_cc_dirty_page_threshold, bool has_resident_available_pages_shared_committed_pages, uint32_t page_size);

        uint64_t IdleProcessTime() const { return perf_info_.IdleProcessTime; }
        uint64_t IoReadTransferCount() const { return perf_info_.IoReadTransferCount; }
        uint64_t IoWriteTransferCount() const { return perf_info_.IoWriteTransferCount; }
        uint64_t IoOtherTransferCount() const { return perf_info_.IoOtherTransferCount; }
        uint32_t IoReadOperationCount() const { return perf_info_.IoReadOperationCount; }
        uint32_t IoWriteOperationCount() const { return perf_info_.IoWriteOperationCount; }
        uint32_t IoOtherOperationCount() const { return perf_info_.IoOtherOperationCount; }
        uint32_t AvailablePages() const { return perf_info_.AvailablePages; }
        uint64_t AvailablePagesSize() const { return static_cast<uint64_t>(perf_info_.AvailablePages) * page_size_; }
        uint32_t CommittedPages() const { return perf_info_.CommittedPages; }
        uint64_t CommittedPagesSize() const { return static_cast<uint64_t>(perf_info_.CommittedPages) * page_size_; }
        uint32_t CommitLimit() const { return perf_info_.CommitLimit; }
        uint64_t CommitLimitSize() const { return static_cast<uint64_t>(perf_info_.CommitLimit) * page_size_; }
        uint32_t PeakCommitment() const { return perf_info_.PeakCommitment; }
        uint64_t PeakCommitmentSize() const { return static_cast<uint64_t>(perf_info_.PeakCommitment) * page_size_; }
        uint32_t PageFaultCount() const { return perf_info_.PageFaultCount; }
        uint32_t CopyOnWriteCount() const { return perf_info_.CopyOnWriteCount; }
        uint32_t TransitionCount() const { return perf_info_.TransitionCount; }
        uint32_t CacheTransitionCount() const { return perf_info_.CacheTransitionCount; }
        uint32_t DemandZeroCount() const { return perf_info_.DemandZeroCount; }
        uint32_t PageReadCount() const { return perf_info_.PageReadCount; }
        uint32_t PageReadIoCount() const { return perf_info_.PageReadIoCount; }
        uint32_t CacheReadCount() const { return perf_info_.CacheReadCount; }
        uint32_t CacheIoCount() const { return perf_info_.CacheIoCount; }
        uint32_t DirtyPagesWriteCount() const { return perf_info_.DirtyPagesWriteCount; }
        uint32_t DirtyWriteIoCount() const { return perf_info_.DirtyWriteIoCount; }
        uint32_t MappedPagesWriteCount() const { return perf_info_.MappedPagesWriteCount; }
        uint32_t MappedWriteIoCount() const { return perf_info_.MappedWriteIoCount; }
        uint32_t PagedPoolPages() const { return perf_info_.PagedPoolPages; }
        uint64_t PagedPoolPagesSize() const { return static_cast<uint64_t>(perf_info_.PagedPoolPages) * page_size_; }
        uint32_t NonPagedPoolPages() const { return perf_info_.NonPagedPoolPages; }
        uint64_t NonPagedPoolPagesSize() const { return static_cast<uint64_t>(perf_info_.NonPagedPoolPages) * page_size_; }
        uint32_t PagedPoolAllocs() const { return perf_info_.PagedPoolAllocs; }
        uint32_t PagedPoolFrees() const { return perf_info_.PagedPoolFrees; }
        uint32_t NonPagedPoolAllocs() const { return perf_info_.NonPagedPoolAllocs; }
        uint32_t NonPagedPoolFrees() const { return perf_info_.NonPagedPoolFrees; }
        uint32_t FreeSystemPtes() const { return perf_info_.FreeSystemPtes; }
        uint64_t FreeSystemPtesSize() const { return static_cast<uint64_t>(perf_info_.FreeSystemPtes) * page_size_; }
        uint32_t ResidentSystemCodePage() const { return perf_info_.ResidentSystemCodePage; }
        uint64_t ResidentSystemCodePageSize() const { return static_cast<uint64_t>(perf_info_.ResidentSystemCodePage) * page_size_; }
        uint32_t TotalSystemDriverPages() const { return perf_info_.TotalSystemDriverPages; }
        uint64_t TotalSystemDriverPagesSize() const { return static_cast<uint64_t>(perf_info_.TotalSystemDriverPages) * page_size_; }
        uint32_t TotalSystemCodePages() const { return perf_info_.TotalSystemCodePages; }
        uint64_t TotalSystemCodePagesSize() const { return static_cast<uint64_t>(perf_info_.TotalSystemCodePages) * page_size_; }
        uint32_t NonPagedPoolLookasideHits() const { return perf_info_.NonPagedPoolLookasideHits; }
        uint32_t PagedPoolLookasideHits() const { return perf_info_.PagedPoolLookasideHits; }
        uint32_t AvailablePagedPoolPages() const { return perf_info_.AvailablePagedPoolPages; }
        uint64_t AvailablePagedPoolPagesSize() const { return static_cast<uint64_t>(perf_info_.AvailablePagedPoolPages) * page_size_; }
        uint32_t ResidentSystemCachePage() const { return perf_info_.ResidentSystemCachePage; }
        uint64_t ResidentSystemCachePageSize() const { return static_cast<uint64_t>(perf_info_.ResidentSystemCachePage) * page_size_; }
        uint32_t ResidentPagedPoolPage() const { return perf_info_.ResidentPagedPoolPage; }
        uint64_t ResidentPagedPoolPageSize() const { return static_cast<uint64_t>(perf_info_.ResidentPagedPoolPage) * page_size_; }
        uint32_t ResidentSystemDriverPage() const { return perf_info_.ResidentSystemDriverPage; }
        uint64_t ResidentSystemDriverPageSize() const { return static_cast<uint64_t>(perf_info_.ResidentSystemDriverPage) * page_size_; }
        uint32_t CcFastReadNoWait() const { return perf_info_.CcFastReadNoWait; }
        uint32_t CcFastReadWait() const { return perf_info_.CcFastReadWait; }
        uint32_t CcFastReadResourceMiss() const { return perf_info_.CcFastReadResourceMiss; }
        uint32_t CcFastReadNotPossible() const { return perf_info_.CcFastReadNotPossible; }
        uint32_t CcFastMdlReadNoWait() const { return perf_info_.CcFastMdlReadNoWait; }
        uint32_t CcFastMdlReadWait() const { return perf_info_.CcFastMdlReadWait; }
        uint32_t CcFastMdlReadResourceMiss() const { return perf_info_.CcFastMdlReadResourceMiss; }
        uint32_t CcFastMdlReadNotPossible() const { return perf_info_.CcFastMdlReadNotPossible; }
        uint32_t CcMapDataNoWait() const { return perf_info_.CcMapDataNoWait; }
        uint32_t CcMapDataWait() const { return perf_info_.CcMapDataWait; }
        uint32_t CcMapDataNoWaitMiss() const { return perf_info_.CcMapDataNoWaitMiss; }
        uint32_t CcMapDataWaitMiss() const { return perf_info_.CcMapDataWaitMiss; }
        uint32_t CcPinMappedDataCount() const { return perf_info_.CcPinMappedDataCount; }
        uint32_t CcPinReadNoWait() const { return perf_info_.CcPinReadNoWait; }
        uint32_t CcPinReadWait() const { return perf_info_.CcPinReadWait; }
        uint32_t CcPinReadNoWaitMiss() const { return perf_info_.CcPinReadNoWaitMiss; }
        uint32_t CcPinReadWaitMiss() const { return perf_info_.CcPinReadWaitMiss; }
        uint32_t CcCopyReadNoWait() const { return perf_info_.CcCopyReadNoWait; }
        uint32_t CcCopyReadWait() const { return perf_info_.CcCopyReadWait; }
        uint32_t CcCopyReadNoWaitMiss() const { return perf_info_.CcCopyReadNoWaitMiss; }
        uint32_t CcCopyReadWaitMiss() const { return perf_info_.CcCopyReadWaitMiss; }
        uint32_t CcMdlReadNoWait() const { return perf_info_.CcMdlReadNoWait; }
        uint32_t CcMdlReadWait() const { return perf_info_.CcMdlReadWait; }
        uint32_t CcMdlReadNoWaitMiss() const { return perf_info_.CcMdlReadNoWaitMiss; }
        uint32_t CcMdlReadWaitMiss() const { return perf_info_.CcMdlReadWaitMiss; }
        uint32_t CcReadAheadIos() const { return perf_info_.CcReadAheadIos; }
        uint32_t CcLazyWriteIos() const { return perf_info_.CcLazyWriteIos; }
        uint32_t CcLazyWritePages() const { return perf_info_.CcLazyWritePages; }
        uint64_t CcLazyWritePagesSize() const { return static_cast<uint64_t>(perf_info_.CcLazyWritePages) * page_size_; }
        uint32_t CcDataFlushes() const { return perf_info_.CcDataFlushes; }
        uint32_t CcDataPages() const { return perf_info_.CcDataPages; }
        uint64_t CcDataPagesSize() const { return static_cast<uint64_t>(perf_info_.CcDataPages) * page_size_; }
        uint32_t ContextSwitches() const { return perf_info_.ContextSwitches; }
        uint32_t FirstLevelTbFills() const { return perf_info_.FirstLevelTbFills; }
        uint32_t SecondLevelTbFills() const { return perf_info_.SecondLevelTbFills; }
        uint32_t SystemCalls() const { return perf_info_.SystemCalls; }

        bool HasCcTotalDirtyPagesCcDirtyPageThreshold() const { return has_cc_total_dirty_pages_cc_dirty_page_threshold_; }
        uint64_t CcTotalDirtyPages() const { return perf_info_.CcTotalDirtyPages; }
        uint64_t CcTotalDirtyPagesSize() const { return perf_info_.CcTotalDirtyPages * page_size_; }
        uint64_t CcDirtyPageThreshold() const { return perf_info_.CcDirtyPageThreshold; }

        bool HasResidentAvailablePagesSharedCommittedPages() const { return has_resident_available_pages_shared_committed_pages_; }
        int64_t ResidentAvailablePages() const { return perf_info_.ResidentAvailablePages; }
        int64_t ResidentAvailablePagesSize() const { return perf_info_.ResidentAvailablePages * page_size_; }
        uint64_t SharedCommittedPages() const { return perf_info_.SharedCommittedPages; }
        uint64_t SharedCommittedPagesSize() const { return perf_info_.SharedCommittedPages * page_size_; }
 
    private:
        MINIDUMP_SYSTEM_PERFORMANCE_INFORMATION perf_info_{};
        bool has_cc_total_dirty_pages_cc_dirty_page_threshold_{};
        bool has_resident_available_pages_shared_committed_pages_{};
        uint32_t page_size_{};
   };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct MinidumpSystemPerformanceInformation : MinidumpSystemPerformanceInformationT<MinidumpSystemPerformanceInformation, implementation::MinidumpSystemPerformanceInformation>
    {
    };
}
