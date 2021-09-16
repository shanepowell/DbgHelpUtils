#include "dump_mini_dump_memory.h"

#include <iostream>

#include "dump_file_options.h"
#include "DbgHelpUtils/hex_dump.h"
#include "DbgHelpUtils/memory64_list_stream.h"
#include "DbgHelpUtils/memory_info_list_stream.h"
#include "DbgHelpUtils/memory_info_utils.h"
#include "DbgHelpUtils/memory_list_stream.h"
#include "DbgHelpUtils/mini_dump.h"
#include "DbgHelpUtils/range_units.h"
#include "DbgHelpUtils/size_units.h"
#include "DbgHelpUtils/stream_hex_dump.h"
#include "DbgHelpUtils/system_memory_info_stream.h"
#include "DbgHelpUtils/time_utils.h"
#include "DbgHelpUtils/time_units.h"
#include "DbgHelpUtils/vector_to_hash_set.h"

using namespace std;
using namespace dlg_help_utils::stream_hex_dump;
using namespace dlg_help_utils;

void dump_mini_dump_memory_list_stream_data(mini_dump const& mini_dump, size_t const index,
                                            dump_file_options const& options)
{
    memory_list_stream const memory_list{mini_dump, index};

    if (!memory_list.found())
    {
        wcout << L"MemoryListStream not found!\n";
        return;
    }

    auto const memory_areas = vector_to_hash_set<uint64_t>(options.filter_values(L"memory_area"s));
    auto const memory_ranges = vector_to_hash_set<range>(options.filter_values(L"memory_range"s));

    wcout << L"NumberOfMemoryRanges: " << memory_list.memory_list().NumberOfMemoryRanges << L'\n';
    for (size_t i = 0; auto const* entry : memory_list.list())
    {
        auto const any_match = memory_areas.empty() && memory_ranges.empty();
        auto const memory_areas_match = !memory_areas.empty() && ranges::find_if(memory_areas
                                                                                 , [entry](auto area)
                                                                                 {
                                                                                     return range_utils::range_contains(entry->StartOfMemoryRange, entry->Memory.DataSize, area, 1);
                                                                                 }) != memory_areas.end();
        auto const memory_ranges_match = ranges::find_if(memory_ranges,
                                                         [entry](auto const& area)
                                                         {
                                                             return range_utils::range_contains(
                                                                 entry->StartOfMemoryRange, entry->Memory.DataSize,
                                                                 area.start, 1);
                                                         });
        if (!any_match && !memory_areas_match && memory_ranges_match == memory_ranges.end())
        {
            ++i;
            continue;
        }

        using namespace size_units::base_16;
        wcout << L" [" << i << "]: " << to_hex_full(entry->StartOfMemoryRange) << L" - " <<
            to_hex_full(entry->StartOfMemoryRange + entry->Memory.DataSize) << L" (" << to_hex(entry->Memory.DataSize)
            << L") (" << bytes{entry->Memory.DataSize} << L")\n";

        if (memory_ranges_match != memory_ranges.end())
        {
            auto range = *memory_ranges_match;
            range_utils::range_union(entry->StartOfMemoryRange, entry->Memory.DataSize, range.start, range.size);
            hex_dump::hex_dump(
                wcout,
                static_cast<uint8_t const*>(mini_dump.rva32(entry->Memory)) + (range.start - entry->StartOfMemoryRange),
                range.size, 3, true, 16, range.start);
            wcout << L'\n';
        }
        else if (options.hex_dump_memory_data())
        {
            hex_dump::hex_dump(wcout, mini_dump.rva32(entry->Memory), options.hex_dump_memory_size(entry->Memory.DataSize), 3, true, 16,
                               entry->StartOfMemoryRange);
            wcout << L'\n';
        }

        ++i;
    }
    wcout << L'\n';
}

void dump_mini_dump_memory64_list_stream_data(mini_dump const& mini_dump, size_t const index,
                                              dump_file_options const& options)
{
    memory64_list_stream const memory_list{mini_dump, index};

    if (!memory_list.found())
    {
        wcout << L"Memory64ListStream not found!\n";
        return;
    }

    auto const memory_areas = vector_to_hash_set<uint64_t>(options.filter_values(L"memory_area"s));
    auto const memory_ranges = vector_to_hash_set<range>(options.filter_values(L"memory_range"s));

    wcout << L"NumberOfMemoryRanges: " << memory_list.memory_list().NumberOfMemoryRanges << L'\n';
    for (size_t i = 0; auto const& entry : memory_list.list())
    {
        auto const any_match = memory_areas.empty() && memory_ranges.empty();
        auto const memory_areas_match = !memory_areas.empty() && ranges::find_if(memory_areas
                                                                                 , [entry](auto area)
                                                                                 {
                                                                                     return range_utils::range_contains(entry.start_of_memory_range, entry.location.DataSize, area, 1);
                                                                                 }) != memory_areas.end();
        auto const memory_ranges_match = ranges::find_if(memory_ranges,
                                                         [entry](auto const& area)
                                                         {
                                                             return range_utils::range_contains(
                                                                 entry.start_of_memory_range, entry.location.DataSize,
                                                                 area.start, 1);
                                                         });
        if (!any_match && !memory_areas_match && memory_ranges_match == memory_ranges.end())
        {
            ++i;
            continue;
        }

        using namespace size_units::base_16;
        wcout << L" [" << i << "]: " << to_hex_full(entry.start_of_memory_range) << L" - " <<
            to_hex_full(entry.start_of_memory_range + entry.location.DataSize) << L" (" <<
            to_hex(entry.location.DataSize) << L") (" << bytes{entry.location.DataSize} << L")\n";

        if (memory_ranges_match != memory_ranges.end())
        {
            auto range = *memory_ranges_match;
            range_utils::range_union(entry.start_of_memory_range, entry.location.DataSize, range.start, range.size);
            hex_dump::hex_dump(
                wcout,
                static_cast<uint8_t const*>(mini_dump.rva64(entry.location)) + (range.start - entry.
                    start_of_memory_range), range.size, 3, true, 16, range.start);
            wcout << L'\n';
        }
        else if (options.hex_dump_memory_data())
        {
            hex_dump::hex_dump(wcout, mini_dump.rva64(entry.location), options.hex_dump_memory_size(entry.location.DataSize), 3, true, 16,
                               entry.start_of_memory_range);
            wcout << L'\n';
        }

        ++i;
    }
    wcout << L'\n';
}

void dump_mini_dump_memory_info_list_stream_data(mini_dump const& mini_dump, size_t const index)
{
    memory_info_list_stream const memory_info_list{mini_dump, index};

    if (!memory_info_list.found())
    {
        wcout << L"MemoryInfoListStream not found!\n";
        return;
    }

    if (!memory_info_list.is_valid())
    {
        wcout << L"MemoryInfoListStream version unknown!\n";
        return;
    }

    wcout << L"NumberOfEntries: " << memory_info_list.size() << L'\n';
    for (size_t i = 0; auto const* entry : memory_info_list.list())
    {
        using namespace size_units::base_16;
        wcout << L" [" << i << "]:\n";
        wcout << L"   BaseAddress: " << to_hex_full(entry->BaseAddress) << L'\n';
        wcout << L"   AllocationBase: " << to_hex_full(entry->AllocationBase) << L'\n';
        wcout << L"   RegionSize: " << to_hex_full(entry->RegionSize) << L" (" << bytes{entry->RegionSize} << L")\n";
        wcout << L"   State: " << memory_info_utils::memory_state_to_string(entry->State) << L" (" <<
            to_hex(entry->State) << L")\n";
        wcout << L"   AllocationProtect: " <<
            memory_info_utils::memory_protection_options_to_string(entry->AllocationProtect) << L" (" << to_hex(
                entry->AllocationProtect) << L")\n";
        wcout << L"   Protect: " << memory_info_utils::memory_protection_options_to_string(entry->Protect) << L" (" <<
            to_hex(entry->Protect) << L")\n";
        wcout << L"   Type: " << memory_info_utils::memory_type_to_string(entry->Type) << L" (" << to_hex(entry->Type)
            << L")\n";

        ++i;
    }
    wcout << L'\n';
}

void dump_mini_dump_system_memory_info_stream_data(mini_dump const& mini_dump, size_t const index)
{
    system_memory_info_stream const system_memory_info_list{mini_dump, index};

    if (!system_memory_info_list.found())
    {
        wcout << L"SystemMemoryInfoStream not found!\n";
        return;
    }

    if (!system_memory_info_list.is_valid())
    {
        wcout << L"SystemMemoryInfoStream version unknown!\n";
        return;
    }

    auto const& info = system_memory_info_list.system_memory_misc_info();


    wcout << L" Revision: " << info.Revision << L'\n';
    wcout << L" Flags: " << memory_info_utils::system_memory_info_flags_to_string(info.Flags) << L" (" <<
        to_hex(info.Flags) << L")\n";

    {
        using namespace size_units::base_16;
        wcout << L" BasicInfo:\n";
        wcout << L"  TimerResolution: " << info.BasicInfo.TimerResolution << L'\n';
        wcout << L"  PageSize: " << info.BasicInfo.PageSize << L'\n';
        wcout << L"  NumberOfPhysicalPages: " << info.BasicInfo.NumberOfPhysicalPages << L" (" << bytes{
            info.BasicInfo.NumberOfPhysicalPages * info.BasicInfo.PageSize
        } << L")\n";
        wcout << L"  LowestPhysicalPageNumber: " << info.BasicInfo.LowestPhysicalPageNumber << L'\n';
        wcout << L"  HighestPhysicalPageNumber: " << info.BasicInfo.HighestPhysicalPageNumber << L'\n';
        wcout << L"  AllocationGranularity: " << info.BasicInfo.AllocationGranularity << L" (" << bytes{
            info.BasicInfo.AllocationGranularity
        } << L")\n";
        wcout << L"  MinimumUserModeAddress: " << to_hex_full(info.BasicInfo.MinimumUserModeAddress) << L'\n';
        wcout << L"  MaximumUserModeAddress: " << to_hex_full(info.BasicInfo.MaximumUserModeAddress) << L'\n';
        wcout << L"  ActiveProcessorsAffinityMask: " << to_hex(info.BasicInfo.ActiveProcessorsAffinityMask) << L'\n';
        wcout << L"  NumberOfProcessors: " << info.BasicInfo.NumberOfProcessors << L'\n';
    }

    {
        wcout << L" FileCacheInfo:\n";
        using namespace size_units::base_16;
        wcout << L"  CurrentSize: " << info.FileCacheInfo.CurrentSize << L" (" << bytes{info.FileCacheInfo.CurrentSize}
            << L")\n";
        wcout << L"  PeakSize: " << info.FileCacheInfo.PeakSize << L" (" << bytes{info.FileCacheInfo.PeakSize} <<
            L")\n";
        wcout << L"  PageFaultCount: " << info.FileCacheInfo.PageFaultCount << L'\n';
        wcout << L"  MinimumWorkingSet: " << info.FileCacheInfo.MinimumWorkingSet << L" (" << bytes{
            info.FileCacheInfo.MinimumWorkingSet
        } << L")\n";
        wcout << L"  MaximumWorkingSet: " << info.FileCacheInfo.MaximumWorkingSet << L" (" << bytes{
            info.FileCacheInfo.MaximumWorkingSet
        } << L")\n";
        wcout << L"  CurrentSizeIncludingTransitionInPages: " << info.FileCacheInfo.
                                                                      CurrentSizeIncludingTransitionInPages << L" (" <<
            bytes{info.FileCacheInfo.CurrentSizeIncludingTransitionInPages * info.BasicInfo.PageSize} << L")\n";
        wcout << L"  PeakSizeIncludingTransitionInPages: " << info.FileCacheInfo.PeakSizeIncludingTransitionInPages <<
            L" (" << bytes{info.FileCacheInfo.PeakSizeIncludingTransitionInPages * info.BasicInfo.PageSize} << L")\n";
        if ((info.Flags & MINIDUMP_SYSMEMINFO1_FILECACHE_TRANSITIONREPURPOSECOUNT_FLAGS) ==
            MINIDUMP_SYSMEMINFO1_FILECACHE_TRANSITIONREPURPOSECOUNT_FLAGS)
        {
            wcout << L"  TransitionRePurposeCount: " << info.FileCacheInfo.TransitionRePurposeCount << L'\n';
        }
        wcout << L"  Flags: " << to_hex(info.FileCacheInfo.Flags) << L'\n';
    }

    if ((info.Flags & MINIDUMP_SYSMEMINFO1_BASICPERF) == MINIDUMP_SYSMEMINFO1_BASICPERF)
    {
        wcout << L" BasicPerfInfo:\n";
        using namespace size_units::base_16;
        wcout << L"  AvailablePages: " << info.BasicPerfInfo.AvailablePages << L" (" << bytes{
            info.BasicPerfInfo.AvailablePages * info.BasicInfo.PageSize
        } << L")\n";
        wcout << L"  CommittedPages: " << info.BasicPerfInfo.CommittedPages << L" (" << bytes{
            info.BasicPerfInfo.CommittedPages * info.BasicInfo.PageSize
        } << L")\n";
        wcout << L"  CommitLimit: " << info.BasicPerfInfo.CommitLimit << L" (" << bytes{
            info.BasicPerfInfo.CommitLimit * info.BasicInfo.PageSize
        } << L")\n";
        wcout << L"  PeakCommitment: " << info.BasicPerfInfo.PeakCommitment << L" (" << bytes{
            info.BasicPerfInfo.PeakCommitment * info.BasicInfo.PageSize
        } << L")\n";
    }

    {
        wcout << L" PerfInfo:\n";
        // ReSharper disable StringLiteralTypo
        {
            using namespace time_units;
            wcout << L"  IdleProcessTime: " << info.PerfInfo.IdleProcessTime << L" (" << time_utils::duration_to_ms(
                info.PerfInfo.IdleProcessTime) << L")\n";
        }

        {
            using namespace size_units::base_16;
            wcout << L"  IoReadTransferCount: " << info.PerfInfo.IoReadTransferCount << L'\n';
            wcout << L"  IoWriteTransferCount: " << info.PerfInfo.IoWriteTransferCount << L'\n';
            wcout << L"  IoOtherTransferCount: " << info.PerfInfo.IoOtherTransferCount << L'\n';
            wcout << L"  IoReadOperationCount: " << info.PerfInfo.IoReadOperationCount << L'\n';
            wcout << L"  IoWriteOperationCount: " << info.PerfInfo.IoWriteOperationCount << L'\n';
            wcout << L"  IoOtherOperationCount: " << info.PerfInfo.IoOtherOperationCount << L'\n';
            wcout << L"  AvailablePages: " << info.PerfInfo.AvailablePages << L" (" << bytes{
                info.PerfInfo.AvailablePages * info.BasicInfo.PageSize
            } << L")\n";
            wcout << L"  CommittedPages: " << info.PerfInfo.CommittedPages << L" (" << bytes{
                info.PerfInfo.CommittedPages * info.BasicInfo.PageSize
            } << L")\n";
            wcout << L"  CommitLimit: " << info.PerfInfo.CommitLimit << L" (" << bytes{
                info.PerfInfo.CommitLimit * info.BasicInfo.PageSize
            } << L")\n";
            wcout << L"  PeakCommitment: " << info.PerfInfo.PeakCommitment << L" (" << bytes{
                info.PerfInfo.PeakCommitment * info.BasicInfo.PageSize
            } << L")\n";
            wcout << L"  PageFaultCount: " << info.PerfInfo.PageFaultCount << L'\n';
            wcout << L"  CopyOnWriteCount: " << info.PerfInfo.CopyOnWriteCount << L'\n';
            wcout << L"  TransitionCount: " << info.PerfInfo.TransitionCount << L'\n';
            wcout << L"  CacheTransitionCount: " << info.PerfInfo.CacheTransitionCount << L'\n';
            wcout << L"  DemandZeroCount: " << info.PerfInfo.DemandZeroCount << L'\n';
            wcout << L"  PageReadCount: " << info.PerfInfo.PageReadCount << L'\n';
            wcout << L"  PageReadIoCount: " << info.PerfInfo.PageReadIoCount << L'\n';
            wcout << L"  CacheReadCount: " << info.PerfInfo.CacheReadCount << L'\n';
            wcout << L"  CacheIoCount: " << info.PerfInfo.CacheIoCount << L'\n';
            wcout << L"  DirtyPagesWriteCount: " << info.PerfInfo.DirtyPagesWriteCount << L'\n';
            wcout << L"  DirtyWriteIoCount: " << info.PerfInfo.DirtyWriteIoCount << L'\n';
            wcout << L"  MappedPagesWriteCount: " << info.PerfInfo.MappedPagesWriteCount << L'\n';
            wcout << L"  MappedWriteIoCount: " << info.PerfInfo.MappedWriteIoCount << L'\n';
            wcout << L"  PagedPoolPages: " << info.PerfInfo.PagedPoolPages << L" (" << bytes{
                info.PerfInfo.PagedPoolPages * info.BasicInfo.PageSize
            } << L")\n";
            wcout << L"  NonPagedPoolPages: " << info.PerfInfo.NonPagedPoolPages << L" (" << bytes{
                info.PerfInfo.NonPagedPoolPages * info.BasicInfo.PageSize
            } << L")\n";
            wcout << L"  PagedPoolAllocs: " << info.PerfInfo.PagedPoolAllocs << L'\n';
            wcout << L"  PagedPoolFrees: " << info.PerfInfo.PagedPoolFrees << L'\n';
            wcout << L"  NonPagedPoolFrees: " << info.PerfInfo.NonPagedPoolFrees << L'\n';
            wcout << L"  FreeSystemPtes: " << info.PerfInfo.FreeSystemPtes << L'\n';
            wcout << L"  ResidentSystemCodePage: " << info.PerfInfo.ResidentSystemCodePage << L" (" << bytes{
                info.PerfInfo.ResidentSystemCodePage * info.BasicInfo.PageSize
            } << L")\n";
            wcout << L"  TotalSystemDriverPages: " << info.PerfInfo.TotalSystemDriverPages << L" (" << bytes{
                info.PerfInfo.TotalSystemDriverPages * info.BasicInfo.PageSize
            } << L")\n";
            wcout << L"  TotalSystemCodePages: " << info.PerfInfo.TotalSystemCodePages << L" (" << bytes{
                info.PerfInfo.TotalSystemCodePages * info.BasicInfo.PageSize
            } << L")\n";
            wcout << L"  PagedPoolLookasideHits: " << info.PerfInfo.PagedPoolLookasideHits << L'\n';
            wcout << L"  AvailablePagedPoolPages: " << info.PerfInfo.AvailablePagedPoolPages << L" (" << bytes{
                info.PerfInfo.AvailablePagedPoolPages * info.BasicInfo.PageSize
            } << L")\n";
            wcout << L"  ResidentSystemCachePage: " << info.PerfInfo.ResidentSystemCachePage << L" (" << bytes{
                info.PerfInfo.ResidentSystemCachePage * info.BasicInfo.PageSize
            } << L")\n";
            wcout << L"  ResidentPagedPoolPage: " << info.PerfInfo.ResidentPagedPoolPage << L" (" << bytes{
                info.PerfInfo.ResidentPagedPoolPage * info.BasicInfo.PageSize
            } << L")\n";
            wcout << L"  ResidentSystemDriverPage: " << info.PerfInfo.ResidentSystemDriverPage << L" (" << bytes{
                info.PerfInfo.ResidentSystemDriverPage * info.BasicInfo.PageSize
            } << L")\n";
            wcout << L"  CcFastReadWait: " << info.PerfInfo.CcFastReadWait << L'\n';
            wcout << L"  CcFastReadResourceMiss: " << info.PerfInfo.CcFastReadResourceMiss << L'\n';
            wcout << L"  CcFastReadNotPossible: " << info.PerfInfo.CcFastReadNotPossible << L'\n';
            wcout << L"  CcFastMdlReadNoWait: " << info.PerfInfo.CcFastMdlReadNoWait << L'\n';
            wcout << L"  CcFastMdlReadWait: " << info.PerfInfo.CcFastMdlReadWait << L'\n';
            wcout << L"  CcFastMdlReadResourceMiss: " << info.PerfInfo.CcFastMdlReadResourceMiss << L'\n';
            wcout << L"  CcFastMdlReadNotPossible: " << info.PerfInfo.CcFastMdlReadNotPossible << L'\n';
            wcout << L"  CcMapDataWait: " << info.PerfInfo.CcMapDataWait << L'\n';
            wcout << L"  CcMapDataNoWaitMiss: " << info.PerfInfo.CcMapDataNoWaitMiss << L'\n';
            wcout << L"  CcMapDataWaitMiss: " << info.PerfInfo.CcMapDataWaitMiss << L'\n';
            wcout << L"  CcPinMappedDataCount: " << info.PerfInfo.CcPinMappedDataCount << L'\n';
            wcout << L"  CcPinReadNoWait: " << info.PerfInfo.CcPinReadNoWait << L'\n';
            wcout << L"  CcPinReadWait: " << info.PerfInfo.CcPinReadWait << L'\n';
            wcout << L"  CcPinReadNoWaitMiss: " << info.PerfInfo.CcPinReadNoWaitMiss << L'\n';
            wcout << L"  CcPinReadWaitMiss: " << info.PerfInfo.CcPinReadWaitMiss << L'\n';
            wcout << L"  CcCopyReadNoWait: " << info.PerfInfo.CcCopyReadNoWait << L'\n';
            wcout << L"  CcCopyReadWait: " << info.PerfInfo.CcCopyReadWait << L'\n';
            wcout << L"  CcCopyReadNoWaitMiss: " << info.PerfInfo.CcCopyReadNoWaitMiss << L'\n';
            wcout << L"  CcCopyReadWaitMiss: " << info.PerfInfo.CcCopyReadWaitMiss << L'\n';
            wcout << L"  CcMdlReadNoWait: " << info.PerfInfo.CcMdlReadNoWait << L'\n';
            wcout << L"  CcMdlReadWait: " << info.PerfInfo.CcMdlReadWait << L'\n';
            wcout << L"  CcMdlReadNoWaitMiss: " << info.PerfInfo.CcMdlReadNoWaitMiss << L'\n';
            wcout << L"  CcMdlReadWaitMiss: " << info.PerfInfo.CcMdlReadWaitMiss << L'\n';
            wcout << L"  CcReadAheadIos: " << info.PerfInfo.CcReadAheadIos << L'\n';
            wcout << L"  CcLazyWriteIos: " << info.PerfInfo.CcLazyWriteIos << L'\n';
            wcout << L"  CcLazyWritePages: " << info.PerfInfo.CcLazyWritePages << L" (" << bytes{
                info.PerfInfo.CcLazyWritePages * info.BasicInfo.PageSize
            } << L")\n";
            wcout << L"  CcDataFlushes: " << info.PerfInfo.CcDataFlushes << L'\n';
            wcout << L"  CcDataPages: " << info.PerfInfo.CcDataPages << L" (" << bytes{
                info.PerfInfo.CcDataPages * info.BasicInfo.PageSize
            } << L")\n";
            wcout << L"  ContextSwitches: " << info.PerfInfo.ContextSwitches << L'\n';
            wcout << L"  FirstLevelTbFills: " << info.PerfInfo.FirstLevelTbFills << L'\n';
            wcout << L"  SystemCalls: " << info.PerfInfo.SystemCalls << L'\n';

            if ((info.Flags & MINIDUMP_SYSMEMINFO1_PERF_CCTOTALDIRTYPAGES_CCDIRTYPAGETHRESHOLD) ==
                MINIDUMP_SYSMEMINFO1_PERF_CCTOTALDIRTYPAGES_CCDIRTYPAGETHRESHOLD)
            {
                wcout << L"  CcTotalDirtyPages: " << info.PerfInfo.CcTotalDirtyPages << L" (" << bytes{
                    info.PerfInfo.CcTotalDirtyPages * info.BasicInfo.PageSize
                } << L")\n";
                wcout << L"  CcDirtyPageThreshold: " << info.PerfInfo.CcDirtyPageThreshold << L'\n';
            }

            if ((info.Flags & MINIDUMP_SYSMEMINFO1_PERF_RESIDENTAVAILABLEPAGES_SHAREDCOMMITPAGES) ==
                MINIDUMP_SYSMEMINFO1_PERF_RESIDENTAVAILABLEPAGES_SHAREDCOMMITPAGES)
            {
                wcout << L"  ResidentAvailablePages: " << info.PerfInfo.ResidentAvailablePages << L" (" << bytes{
                    info.PerfInfo.ResidentAvailablePages * info.BasicInfo.PageSize
                } << L")\n";
                wcout << L"  SharedCommittedPages: " << info.PerfInfo.SharedCommittedPages << L" (" << bytes{
                    info.PerfInfo.SharedCommittedPages * info.BasicInfo.PageSize
                } << L")\n";
            }
        }
        // ReSharper restore StringLiteralTypo
    }
}
