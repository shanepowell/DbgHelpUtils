#pragma once

#include "MiniDumpSystemBasicInformation.g.h"

#include "GlobalOptionsNotifyPropertyChangedBase.h"
#include "DbgHelpUtils/mini_dump.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct MiniDumpSystemBasicInformation : MiniDumpSystemBasicInformationT<MiniDumpSystemBasicInformation>, GlobalOptionsNotifyPropertyChangedBase<MiniDumpSystemBasicInformation>
    {
        MiniDumpSystemBasicInformation();

        void Set(MINIDUMP_SYSTEM_BASIC_INFORMATION const& basic_info);

        uint32_t TimerResolution() const { return basic_info_.TimerResolution;}
        uint32_t PageSize() const { return basic_info_.PageSize;}
        uint32_t NumberOfPhysicalPages() const { return basic_info_.NumberOfPhysicalPages;}
        uint64_t NumberOfPhysicalPagesSize() const { return static_cast<uint64_t>(basic_info_.NumberOfPhysicalPages) * basic_info_.PageSize;}
        uint32_t LowestPhysicalPageNumber() const { return basic_info_.LowestPhysicalPageNumber;}
        uint32_t HighestPhysicalPageNumber() const { return basic_info_.HighestPhysicalPageNumber;}
        uint32_t AllocationGranularity() const { return basic_info_.AllocationGranularity;}
        uint64_t MinimumUserModeAddress() const { return basic_info_.MinimumUserModeAddress;}
        uint64_t MaximumUserModeAddress() const { return basic_info_.MaximumUserModeAddress;}
        uint64_t ActiveProcessorsAffinityMask() const { return basic_info_.ActiveProcessorsAffinityMask;}
        uint32_t NumberOfProcessors() const { return basic_info_.NumberOfProcessors;}

    private:
        MINIDUMP_SYSTEM_BASIC_INFORMATION basic_info_{};
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct MiniDumpSystemBasicInformation : MiniDumpSystemBasicInformationT<MiniDumpSystemBasicInformation, implementation::MiniDumpSystemBasicInformation>
    {
    };
}
