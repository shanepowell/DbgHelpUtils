#pragma once

#include "MiniDumpSystemBasicPerformanceInformation.g.h"

#include "GlobalOptionsNotifyPropertyChangedBase.h"
#include "DbgHelpUtils/mini_dump.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct MiniDumpSystemBasicPerformanceInformation : MiniDumpSystemBasicPerformanceInformationT<MiniDumpSystemBasicPerformanceInformation>, GlobalOptionsNotifyPropertyChangedBase<MiniDumpSystemBasicPerformanceInformation>
    {
        MiniDumpSystemBasicPerformanceInformation();

        void Set(MINIDUMP_SYSTEM_BASIC_PERFORMANCE_INFORMATION const& basic_perf_info, bool has_basic_perf_info, uint32_t page_size);

        bool HasBasicPerfInfo() const { return has_basic_perf_info_; }
        uint64_t AvailablePages() const { return basic_perf_info_.AvailablePages; }
        uint64_t AvailablePagesSize() const { return basic_perf_info_.AvailablePages * page_size_; }
        uint64_t CommittedPages() const { return basic_perf_info_.CommittedPages; }
        uint64_t CommittedPagesSize() const { return basic_perf_info_.CommittedPages * page_size_; }
        uint64_t CommitLimit() const { return basic_perf_info_.CommitLimit; }
        uint64_t CommitLimitSize() const { return basic_perf_info_.CommitLimit * page_size_; }
        uint64_t PeakCommitment() const { return basic_perf_info_.PeakCommitment; }
        uint64_t PeakCommitmentSize() const { return basic_perf_info_.PeakCommitment * page_size_; }

    private:
        MINIDUMP_SYSTEM_BASIC_PERFORMANCE_INFORMATION basic_perf_info_{};
        bool has_basic_perf_info_{};
        uint32_t page_size_{};
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct MiniDumpSystemBasicPerformanceInformation : MiniDumpSystemBasicPerformanceInformationT<MiniDumpSystemBasicPerformanceInformation, implementation::MiniDumpSystemBasicPerformanceInformation>
    {
    };
}
