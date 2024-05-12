#include "dump_mini_dump_memory.h"

#include "dump_file_options.h"
#include "DbgHelpUtils/hex_dump.h"
#include "DbgHelpUtils/locale_number_formatting.h"
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

void dump_mini_dump_memory_list_stream_data(std::wostream& log, mini_dump const& mini_dump, size_t const index,
                                            dump_file_options const& options)
{
    memory_list_stream const memory_list{mini_dump, index};

    if (!memory_list.found())
    {
        log << L"MemoryListStream not found!\n";
        return;
    }

    auto const memory_areas = vector_to_hash_set<uint64_t>(options.filter_values(L"memory_area"s));
    auto const memory_ranges = vector_to_hash_set<user_range>(options.filter_values(L"memory_range"s));

    log << std::format(L"NumberOfMemoryRanges: {}\n", locale_formatting::to_wstring(memory_list.memory_list().NumberOfMemoryRanges));
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
        log << std::format(L" [{0}]: {1} - {2} ({3}) ({4})\n"
            , locale_formatting::to_wstring(i)
            , to_hex_full(entry->StartOfMemoryRange)
            , to_hex_full(entry->StartOfMemoryRange + entry->Memory.DataSize)
            , to_hex(entry->Memory.DataSize)
            , to_wstring(bytes{entry->Memory.DataSize}));

        if (memory_ranges_match != memory_ranges.end())
        {
            auto range = *memory_ranges_match;
            range_utils::range_union(entry->StartOfMemoryRange, entry->Memory.DataSize, range.start, range.size);
            hex_dump::hex_dump(log
                , static_cast<uint8_t const*>(mini_dump.rva32(entry->Memory)) + (range.start - entry->StartOfMemoryRange)
                , range.size
                , 3
                , write_header_t{true}
                , 16
                , range.start);
            log << L'\n';
        }
        else if (options.hex_dump_memory_data())
        {
            hex_dump::hex_dump(log
                , mini_dump.rva32(entry->Memory)
                , options.hex_dump_memory_size(entry->Memory.DataSize)
                , 3
                , write_header_t{true}
                , 16
                , entry->StartOfMemoryRange);
            log << L'\n';
        }

        ++i;
    }
    log << L'\n';
}

void dump_mini_dump_memory64_list_stream_data(std::wostream& log, mini_dump const& mini_dump, size_t const index,
                                              dump_file_options const& options)
{
    memory64_list_stream const memory_list{mini_dump, index};

    if (!memory_list.found())
    {
        log << L"Memory64ListStream not found!\n";
        return;
    }

    auto const memory_areas = vector_to_hash_set<uint64_t>(options.filter_values(L"memory_area"s));
    auto const memory_ranges = vector_to_hash_set<user_range>(options.filter_values(L"memory_range"s));

    log << std::format(L"NumberOfMemoryRanges: {}\n", locale_formatting::to_wstring(memory_list.memory_list().NumberOfMemoryRanges));
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
        log << std::format(L" [{0}]: {1} - {2} ({3}) ({4})\n"
            , locale_formatting::to_wstring(i)
            , to_hex_full(entry.start_of_memory_range)
            , to_hex_full(entry.start_of_memory_range + entry.location.DataSize)
            , to_hex(entry.location.DataSize)
            , to_wstring(bytes{entry.location.DataSize}));

        if (memory_ranges_match != memory_ranges.end())
        {
            auto range = *memory_ranges_match;
            range_utils::range_union(entry.start_of_memory_range, entry.location.DataSize, range.start, range.size);
            hex_dump::hex_dump(log
                , static_cast<uint8_t const*>(mini_dump.rva64(entry.location)) + (range.start - entry.start_of_memory_range)
                , range.size
                , 3
                , write_header_t{true}
                , 16
                , range.start);
            log << L'\n';
        }
        else if (options.hex_dump_memory_data())
        {
            hex_dump::hex_dump(log
                , mini_dump.rva64(entry.location)
                , options.hex_dump_memory_size(entry.location.DataSize)
                , 3
                , write_header_t{true}
                , 16
                , entry.start_of_memory_range);
            log << L'\n';
        }

        ++i;
    }
    log << L'\n';
}

void dump_mini_dump_memory_info_list_stream_data(std::wostream& log, mini_dump const& mini_dump, size_t const index)
{
    memory_info_list_stream const memory_info_list{mini_dump, index};

    if (!memory_info_list.found())
    {
        log << L"MemoryInfoListStream not found!\n";
        return;
    }

    if (!memory_info_list.is_valid())
    {
        log << L"MemoryInfoListStream version unknown!\n";
        return;
    }

    log << std::format(L"NumberOfEntries: {}\n", locale_formatting::to_wstring(memory_info_list.size()));
    for (size_t i = 0; auto const* entry : memory_info_list.list())
    {
        using namespace size_units::base_16;
        log << std::format(L" [{}]:\n", locale_formatting::to_wstring(i));
        log << std::format(L"   BaseAddress: {}\n", to_hex_full(entry->BaseAddress));
        log << std::format(L"   AllocationBase: {}\n", to_hex_full(entry->AllocationBase));
        log << std::format(L"   RegionSize: {0} ({1})\n", to_hex_full(entry->RegionSize), to_wstring(bytes{entry->RegionSize}));
        log << std::format(L"   State: {0} ({1})\n", memory_info_utils::memory_state_to_string(entry->State), to_hex(entry->State));
        log << std::format(L"   AllocationProtect: {0} ({1})\n", memory_info_utils::memory_protection_options_to_string(entry->AllocationProtect), to_hex(entry->AllocationProtect));
        log << std::format(L"   Protect: {0} ({1})\n", memory_info_utils::memory_protection_options_to_string(entry->Protect), to_hex(entry->Protect));
        log << std::format(L"   Type: {0} ({1})\n", memory_info_utils::memory_type_to_string(entry->Type), to_hex(entry->Type));

        ++i;
    }
    log << L'\n';
}

void dump_mini_dump_system_memory_info_stream_data(std::wostream& log, mini_dump const& mini_dump, size_t const index)
{
    system_memory_info_stream const system_memory_info_list{mini_dump, index};

    if (!system_memory_info_list.found())
    {
        log << L"SystemMemoryInfoStream not found!\n";
        return;
    }

    if (!system_memory_info_list.is_valid())
    {
        log << L"SystemMemoryInfoStream version unknown!\n";
        return;
    }

    auto const& info = system_memory_info_list.system_memory_misc_info();


    log << std::format(L" Revision: {}\n", locale_formatting::to_wstring(info.Revision));
    log << std::format(L" Flags: {0} ({1})\n", memory_info_utils::system_memory_info_flags_to_string(info.Flags), to_hex(info.Flags));

    {
        using namespace size_units::base_16;
        log << L" BasicInfo:\n";
        log << std::format(L"  TimerResolution: {}\n", locale_formatting::to_wstring(info.BasicInfo.TimerResolution));
        log << std::format(L"  PageSize: {}\n", locale_formatting::to_wstring(info.BasicInfo.PageSize));
        log << std::format(L"  NumberOfPhysicalPages: {0} ({1})\n", locale_formatting::to_wstring(info.BasicInfo.NumberOfPhysicalPages), to_wstring(bytes{info.BasicInfo.NumberOfPhysicalPages * info.BasicInfo.PageSize}));
        log << std::format(L"  LowestPhysicalPageNumber: {}\n", locale_formatting::to_wstring(info.BasicInfo.LowestPhysicalPageNumber));
        log << std::format(L"  HighestPhysicalPageNumber: {}\n", locale_formatting::to_wstring(info.BasicInfo.HighestPhysicalPageNumber));
        log << std::format(L"  AllocationGranularity: {0} ({1})\n", locale_formatting::to_wstring(info.BasicInfo.AllocationGranularity), to_wstring(bytes{info.BasicInfo.AllocationGranularity}));
        log << std::format(L"  MinimumUserModeAddress: {}\n", to_hex_full(info.BasicInfo.MinimumUserModeAddress));
        log << std::format(L"  MaximumUserModeAddress: {}\n", to_hex_full(info.BasicInfo.MaximumUserModeAddress));
        log << std::format(L"  ActiveProcessorsAffinityMask: {}\n", to_hex(info.BasicInfo.ActiveProcessorsAffinityMask));
        log << std::format(L"  NumberOfProcessors: {}\n", locale_formatting::to_wstring(info.BasicInfo.NumberOfProcessors));
    }

    {
        log << L" FileCacheInfo:\n";
        using namespace size_units::base_16;
        log << std::format(L"  CurrentSize: {0} ({1})\n", locale_formatting::to_wstring(info.FileCacheInfo.CurrentSize), to_wstring(bytes{info.FileCacheInfo.CurrentSize}));
        log << std::format(L"  PeakSize: {0} ({1})\n", locale_formatting::to_wstring(info.FileCacheInfo.PeakSize), to_wstring(bytes{info.FileCacheInfo.PeakSize}));
        log << std::format(L"  PageFaultCount: {}\n", locale_formatting::to_wstring(info.FileCacheInfo.PageFaultCount));
        log << std::format(L"  MinimumWorkingSet: {0} ({1})\n", locale_formatting::to_wstring(info.FileCacheInfo.MinimumWorkingSet), to_wstring(bytes{info.FileCacheInfo.MinimumWorkingSet}));
        log << std::format(L"  MaximumWorkingSet: {0} ({1})\n", locale_formatting::to_wstring(info.FileCacheInfo.MaximumWorkingSet), to_wstring(bytes{info.FileCacheInfo.MaximumWorkingSet}));
        log << std::format(L"  CurrentSizeIncludingTransitionInPages: {0} ({1})\n", locale_formatting::to_wstring(info.FileCacheInfo.CurrentSizeIncludingTransitionInPages), to_wstring(bytes{info.FileCacheInfo.CurrentSizeIncludingTransitionInPages * info.BasicInfo.PageSize}));
        log << std::format(L"  PeakSizeIncludingTransitionInPages: {0} ({1})\n", locale_formatting::to_wstring(info.FileCacheInfo.PeakSizeIncludingTransitionInPages), to_wstring(bytes{info.FileCacheInfo.PeakSizeIncludingTransitionInPages * info.BasicInfo.PageSize}));
        if ((info.Flags & MINIDUMP_SYSMEMINFO1_FILECACHE_TRANSITIONREPURPOSECOUNT_FLAGS) == MINIDUMP_SYSMEMINFO1_FILECACHE_TRANSITIONREPURPOSECOUNT_FLAGS)
        {
            log << std::format(L"  TransitionRePurposeCount: {}\n", locale_formatting::to_wstring(info.FileCacheInfo.TransitionRePurposeCount));
        }
        log << std::format(L"  Flags: {}\n", to_hex(info.FileCacheInfo.Flags));
    }

    if ((info.Flags & MINIDUMP_SYSMEMINFO1_BASICPERF) == MINIDUMP_SYSMEMINFO1_BASICPERF)
    {
        log << L" BasicPerfInfo:\n";
        using namespace size_units::base_16;
        log << std::format(L"  AvailablePages: {0} ({1})\n", locale_formatting::to_wstring(info.BasicPerfInfo.AvailablePages), to_wstring(bytes{info.BasicPerfInfo.AvailablePages * info.BasicInfo.PageSize}));
        log << std::format(L"  CommittedPages: {0} ({1})\n", locale_formatting::to_wstring(info.BasicPerfInfo.CommittedPages), to_wstring(bytes{info.BasicPerfInfo.CommittedPages * info.BasicInfo.PageSize}));
        log << std::format(L"  CommitLimit: {0} ({1})\n", locale_formatting::to_wstring(info.BasicPerfInfo.CommitLimit), to_wstring(bytes{info.BasicPerfInfo.CommitLimit * info.BasicInfo.PageSize}));
        log << std::format(L"  PeakCommitment: {0} ({1})\n", locale_formatting::to_wstring(info.BasicPerfInfo.PeakCommitment), to_wstring(bytes{info.BasicPerfInfo.PeakCommitment * info.BasicInfo.PageSize}));
    }

    {
        log << L" PerfInfo:\n";
        // ReSharper disable StringLiteralTypo
        {
            using namespace time_units;
            log << std::format(L"  IdleProcessTime: {0} ({1})\n", to_timespan_wstring(filetime_nanoseconds{info.PerfInfo.IdleProcessTime}), locale_formatting::to_wstring(info.PerfInfo.IdleProcessTime));
        }

        {
            using namespace size_units::base_16;
            log << std::format(L"  IoReadTransferCount: {}\n", locale_formatting::to_wstring(info.PerfInfo.IoReadTransferCount));
            log << std::format(L"  IoWriteTransferCount: {}\n", locale_formatting::to_wstring(info.PerfInfo.IoWriteTransferCount));
            log << std::format(L"  IoOtherTransferCount: {}\n", locale_formatting::to_wstring(info.PerfInfo.IoOtherTransferCount));
            log << std::format(L"  IoReadOperationCount: {}\n", locale_formatting::to_wstring(info.PerfInfo.IoReadOperationCount));
            log << std::format(L"  IoWriteOperationCount: {}\n", locale_formatting::to_wstring(info.PerfInfo.IoWriteOperationCount));
            log << std::format(L"  IoOtherOperationCount: {}\n", locale_formatting::to_wstring(info.PerfInfo.IoOtherOperationCount));
            log << std::format(L"  AvailablePages: {0} ({1})\n", locale_formatting::to_wstring(info.PerfInfo.AvailablePages), to_wstring(bytes{info.PerfInfo.AvailablePages * info.BasicInfo.PageSize}));
            log << std::format(L"  CommittedPages: {0} ({1})\n", locale_formatting::to_wstring(info.PerfInfo.CommittedPages), to_wstring(bytes{info.PerfInfo.CommittedPages * info.BasicInfo.PageSize}));
            log << std::format(L"  CommitLimit: {0} ({1})\n", locale_formatting::to_wstring(info.PerfInfo.CommitLimit), to_wstring(bytes{info.PerfInfo.CommitLimit * info.BasicInfo.PageSize}));
            log << std::format(L"  PeakCommitment: {0} ({1})\n", locale_formatting::to_wstring(info.PerfInfo.PeakCommitment), to_wstring(bytes{info.PerfInfo.PeakCommitment * info.BasicInfo.PageSize}));
            log << std::format(L"  PageFaultCount: {}\n", locale_formatting::to_wstring(info.PerfInfo.PageFaultCount));
            log << std::format(L"  CopyOnWriteCount: {}\n", locale_formatting::to_wstring(info.PerfInfo.CopyOnWriteCount));
            log << std::format(L"  TransitionCount: {}\n", locale_formatting::to_wstring(info.PerfInfo.TransitionCount));
            log << std::format(L"  CacheTransitionCount: {}\n", locale_formatting::to_wstring(info.PerfInfo.CacheTransitionCount));
            log << std::format(L"  DemandZeroCount: {}\n", locale_formatting::to_wstring(info.PerfInfo.DemandZeroCount));
            log << std::format(L"  PageReadCount: {}\n", locale_formatting::to_wstring(info.PerfInfo.PageReadCount));
            log << std::format(L"  PageReadIoCount: {}\n", locale_formatting::to_wstring(info.PerfInfo.PageReadIoCount));
            log << std::format(L"  CacheReadCount: {}\n", locale_formatting::to_wstring(info.PerfInfo.CacheReadCount));
            log << std::format(L"  CacheIoCount: {}\n", locale_formatting::to_wstring(info.PerfInfo.CacheIoCount));
            log << std::format(L"  DirtyPagesWriteCount: {}\n", locale_formatting::to_wstring(info.PerfInfo.DirtyPagesWriteCount));
            log << std::format(L"  DirtyWriteIoCount: {}\n", locale_formatting::to_wstring(info.PerfInfo.DirtyWriteIoCount));
            log << std::format(L"  MappedPagesWriteCount: {}\n", locale_formatting::to_wstring(info.PerfInfo.MappedPagesWriteCount));
            log << std::format(L"  MappedWriteIoCount: {}\n", locale_formatting::to_wstring(info.PerfInfo.MappedWriteIoCount));
            log << std::format(L"  PagedPoolPages: {0} ({1})\n", locale_formatting::to_wstring(info.PerfInfo.PagedPoolPages), to_wstring(bytes{info.PerfInfo.PagedPoolPages * info.BasicInfo.PageSize}));
            log << std::format(L"  NonPagedPoolPages: {0} ({1})\n", locale_formatting::to_wstring(info.PerfInfo.NonPagedPoolPages), to_wstring(bytes{info.PerfInfo.NonPagedPoolPages * info.BasicInfo.PageSize}));
            log << std::format(L"  PagedPoolAllocs: {}\n", locale_formatting::to_wstring(info.PerfInfo.PagedPoolAllocs));
            log << std::format(L"  PagedPoolFrees: {}\n", locale_formatting::to_wstring(info.PerfInfo.PagedPoolFrees));
            log << std::format(L"  NonPagedPoolFrees: {}\n", locale_formatting::to_wstring(info.PerfInfo.NonPagedPoolFrees));
            log << std::format(L"  FreeSystemPtes: {0} ({1})\n", locale_formatting::to_wstring(info.PerfInfo.FreeSystemPtes), to_wstring(bytes{info.PerfInfo.FreeSystemPtes * info.BasicInfo.PageSize}));
            log << std::format(L"  ResidentSystemCodePage: {0} ({1})\n", locale_formatting::to_wstring(info.PerfInfo.ResidentSystemCodePage), to_wstring(bytes{info.PerfInfo.ResidentSystemCodePage * info.BasicInfo.PageSize}));
            log << std::format(L"  TotalSystemDriverPages: {0} ({1})\n", locale_formatting::to_wstring(info.PerfInfo.TotalSystemDriverPages), to_wstring(bytes{info.PerfInfo.TotalSystemDriverPages * info.BasicInfo.PageSize}));
            log << std::format(L"  TotalSystemCodePages: {0} ({1})\n", locale_formatting::to_wstring(info.PerfInfo.TotalSystemCodePages), to_wstring(bytes{info.PerfInfo.TotalSystemCodePages * info.BasicInfo.PageSize}));
            log << std::format(L"  PagedPoolLookasideHits: {}\n", locale_formatting::to_wstring(info.PerfInfo.PagedPoolLookasideHits));
            log << std::format(L"  AvailablePagedPoolPages: {0} ({1})\n", locale_formatting::to_wstring(info.PerfInfo.AvailablePagedPoolPages), to_wstring(bytes{info.PerfInfo.AvailablePagedPoolPages * info.BasicInfo.PageSize}));
            log << std::format(L"  ResidentSystemCachePage: {0} ({1})\n", locale_formatting::to_wstring(info.PerfInfo.ResidentSystemCachePage), to_wstring(bytes{info.PerfInfo.ResidentSystemCachePage * info.BasicInfo.PageSize}));
            log << std::format(L"  ResidentPagedPoolPage: {0} ({1})\n", locale_formatting::to_wstring(info.PerfInfo.ResidentPagedPoolPage), to_wstring(bytes{info.PerfInfo.ResidentPagedPoolPage * info.BasicInfo.PageSize}));
            log << std::format(L"  ResidentSystemDriverPage: {0} ({1})\n", locale_formatting::to_wstring(info.PerfInfo.ResidentSystemDriverPage), to_wstring(bytes{info.PerfInfo.ResidentSystemDriverPage * info.BasicInfo.PageSize}));
            log << std::format(L"  CcFastReadWait: {}\n", locale_formatting::to_wstring(info.PerfInfo.CcFastReadWait));
            log << std::format(L"  CcFastReadResourceMiss: {}\n", locale_formatting::to_wstring(info.PerfInfo.CcFastReadResourceMiss));
            log << std::format(L"  CcFastReadNotPossible: {}\n", locale_formatting::to_wstring(info.PerfInfo.CcFastReadNotPossible));
            log << std::format(L"  CcFastMdlReadNoWait: {}\n", locale_formatting::to_wstring(info.PerfInfo.CcFastMdlReadNoWait));
            log << std::format(L"  CcFastMdlReadWait: {}\n", locale_formatting::to_wstring(info.PerfInfo.CcFastMdlReadWait));
            log << std::format(L"  CcFastMdlReadResourceMiss: {}\n", locale_formatting::to_wstring(info.PerfInfo.CcFastMdlReadResourceMiss));
            log << std::format(L"  CcFastMdlReadNotPossible: {}\n", locale_formatting::to_wstring(info.PerfInfo.CcFastMdlReadNotPossible));
            log << std::format(L"  CcMapDataWait: {}\n", locale_formatting::to_wstring(info.PerfInfo.CcMapDataWait));
            log << std::format(L"  CcMapDataNoWaitMiss: {}\n", locale_formatting::to_wstring(info.PerfInfo.CcMapDataNoWaitMiss));
            log << std::format(L"  CcMapDataWaitMiss: {}\n", locale_formatting::to_wstring(info.PerfInfo.CcMapDataWaitMiss));
            log << std::format(L"  CcPinMappedDataCount: {}\n", locale_formatting::to_wstring(info.PerfInfo.CcPinMappedDataCount));
            log << std::format(L"  CcPinReadNoWait: {}\n", locale_formatting::to_wstring(info.PerfInfo.CcPinReadNoWait));
            log << std::format(L"  CcPinReadWait: {}\n", locale_formatting::to_wstring(info.PerfInfo.CcPinReadWait));
            log << std::format(L"  CcPinReadNoWaitMiss: {}\n", locale_formatting::to_wstring(info.PerfInfo.CcPinReadNoWaitMiss));
            log << std::format(L"  CcPinReadWaitMiss: {}\n", locale_formatting::to_wstring(info.PerfInfo.CcPinReadWaitMiss));
            log << std::format(L"  CcCopyReadNoWait: {}\n", locale_formatting::to_wstring(info.PerfInfo.CcCopyReadNoWait));
            log << std::format(L"  CcCopyReadWait: {}\n", locale_formatting::to_wstring(info.PerfInfo.CcCopyReadWait));
            log << std::format(L"  CcCopyReadNoWaitMiss: {}\n", locale_formatting::to_wstring(info.PerfInfo.CcCopyReadNoWaitMiss));
            log << std::format(L"  CcCopyReadWaitMiss: {}\n", locale_formatting::to_wstring(info.PerfInfo.CcCopyReadWaitMiss));
            log << std::format(L"  CcMdlReadNoWait: {}\n", locale_formatting::to_wstring(info.PerfInfo.CcMdlReadNoWait));
            log << std::format(L"  CcMdlReadWait: {}\n", locale_formatting::to_wstring(info.PerfInfo.CcMdlReadWait));
            log << std::format(L"  CcMdlReadNoWaitMiss: {}\n", locale_formatting::to_wstring(info.PerfInfo.CcMdlReadNoWaitMiss));
            log << std::format(L"  CcMdlReadWaitMiss: {}\n", locale_formatting::to_wstring(info.PerfInfo.CcMdlReadWaitMiss));
            log << std::format(L"  CcReadAheadIos: {}\n", locale_formatting::to_wstring(info.PerfInfo.CcReadAheadIos));
            log << std::format(L"  CcLazyWriteIos: {}\n", locale_formatting::to_wstring(info.PerfInfo.CcLazyWriteIos));
            log << std::format(L"  CcLazyWritePages: {0} ({1})\n", locale_formatting::to_wstring(info.PerfInfo.CcLazyWritePages), to_wstring(bytes{info.PerfInfo.CcLazyWritePages * info.BasicInfo.PageSize}));
            log << std::format(L"  CcDataFlushes: {}\n", locale_formatting::to_wstring(info.PerfInfo.CcDataFlushes));
            log << std::format(L"  CcDataPages: {0} ({1})\n", locale_formatting::to_wstring(info.PerfInfo.CcDataPages), to_wstring(bytes{info.PerfInfo.CcDataPages * info.BasicInfo.PageSize}));
            log << std::format(L"  ContextSwitches: {}\n", locale_formatting::to_wstring(info.PerfInfo.ContextSwitches));
            log << std::format(L"  FirstLevelTbFills: {}\n", locale_formatting::to_wstring(info.PerfInfo.FirstLevelTbFills));
            log << std::format(L"  SystemCalls: {}\n", locale_formatting::to_wstring(info.PerfInfo.SystemCalls));

            if ((info.Flags & MINIDUMP_SYSMEMINFO1_PERF_CCTOTALDIRTYPAGES_CCDIRTYPAGETHRESHOLD) == MINIDUMP_SYSMEMINFO1_PERF_CCTOTALDIRTYPAGES_CCDIRTYPAGETHRESHOLD)
            {
                log << std::format(L"  CcTotalDirtyPages: {0} ({1})\n", locale_formatting::to_wstring(info.PerfInfo.CcTotalDirtyPages), to_wstring(bytes{info.PerfInfo.CcTotalDirtyPages * info.BasicInfo.PageSize}));
                log << std::format(L"  CcDirtyPageThreshold: {}\n", locale_formatting::to_wstring(info.PerfInfo.CcDirtyPageThreshold));
            }

            if ((info.Flags & MINIDUMP_SYSMEMINFO1_PERF_RESIDENTAVAILABLEPAGES_SHAREDCOMMITPAGES) == MINIDUMP_SYSMEMINFO1_PERF_RESIDENTAVAILABLEPAGES_SHAREDCOMMITPAGES)
            {
                log << std::format(L"  ResidentAvailablePages: {0} ({1})\n", locale_formatting::to_wstring(info.PerfInfo.ResidentAvailablePages), to_wstring(bytes{info.PerfInfo.ResidentAvailablePages * info.BasicInfo.PageSize}));
                log << std::format(L"  SharedCommittedPages: {0} ({1})\n", locale_formatting::to_wstring(info.PerfInfo.SharedCommittedPages), to_wstring(bytes{info.PerfInfo.SharedCommittedPages * info.BasicInfo.PageSize}));
            }
        }
        // ReSharper restore StringLiteralTypo
    }
}
