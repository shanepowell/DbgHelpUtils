#include "dump_mini_dump_heap.h"

#include "dump_file_options.h"
#include "DbgHelpUtils/cache_manager.h"
#include "DbgHelpUtils/crt_entry.h"
#include "DbgHelpUtils/crt_heap.h"
#include "DbgHelpUtils/dph_entry.h"
#include "DbgHelpUtils/dph_heap.h"
#include "DbgHelpUtils/heap_entry.h"
#include "DbgHelpUtils/heap_lfh_affinity_slot.h"
#include "DbgHelpUtils/heap_lfh_bucket.h"
#include "DbgHelpUtils/heap_lfh_context.h"
#include "DbgHelpUtils/heap_lfh_entry.h"
#include "DbgHelpUtils/heap_lfh_subsegment.h"
#include "DbgHelpUtils/heap_page_segment.h"
#include "DbgHelpUtils/heap_segment.h"
#include "DbgHelpUtils/heap_segment_context.h"
#include "DbgHelpUtils/heap_subsegment.h"
#include "DbgHelpUtils/heap_ucr_descriptor.h"
#include "DbgHelpUtils/heap_virtual_block.h"
#include "DbgHelpUtils/heap_vs_context.h"
#include "DbgHelpUtils/heap_vs_entry.h"
#include "DbgHelpUtils/heap_vs_subsegment.h"
#include "DbgHelpUtils/hex_dump.h"
#include "DbgHelpUtils/large_alloc_entry.h"
#include "DbgHelpUtils/lfh_heap.h"
#include "DbgHelpUtils/lfh_segment.h"
#include "DbgHelpUtils/locale_number_formatting.h"
#include "DbgHelpUtils/memory_range.h"
#include "DbgHelpUtils/mini_dump_memory_walker.h"
#include "DbgHelpUtils/nt_heap.h"
#include "DbgHelpUtils/page_range_descriptor.h"
#include "DbgHelpUtils/page_range_flags_utils.h"
#include "DbgHelpUtils/process_heaps.h"
#include "DbgHelpUtils/process_heaps_statistics.h"
#include "DbgHelpUtils/process_heaps_statistic_bucket_view.h"
#include "DbgHelpUtils/process_heaps_statistic_view.h"
#include "DbgHelpUtils/process_heap_entry.h"
#include "DbgHelpUtils/segment_heap.h"
#include "DbgHelpUtils/size_units.h"
#include "DbgHelpUtils/stream_hex_dump.h"
#include "DbgHelpUtils/stream_stack_dump.h"

// ReSharper disable CppClangTidyBugproneBranchClone

using namespace std;
using namespace dlg_help_utils;

constexpr auto units_max_width = 16;

namespace
{
    struct LfhSegmentData
    {
        LfhSegmentData(size_t const segment_index, heap::lfh_segment segment)
            : segment_index{ segment_index }
            , segment{std::move(segment)}
        {
        }

        size_t segment_index;
        heap::lfh_segment segment;
    };

    struct LfhSubsegmentData
    {
        LfhSubsegmentData(std::shared_ptr<LfhSegmentData> segment, heap::heap_subsegment const& subsegment)
            : segment{ std::move(segment) }
            , subsegment{ subsegment }
        {
        }

        std::shared_ptr<LfhSegmentData> segment;
        heap::heap_subsegment subsegment;
        bool printed_subsegment{ false };
    };


    void print_nt_heap_line(std::wostream& log, std::wstring_view const& process_heap_marker, heap::nt_heap const& nt_heap)
    {
        using namespace size_units::base_16;
        log << std::format(L" NT Heap      {1} {2:<{0}} {3:<{0}} {4:<{0}} {5:<{0}} {6:<{0}} {7:<8} {8:<4}{9}{10}\n"
            , units_max_width
            , stream_hex_dump::to_hex_full(nt_heap.flags())
            , to_wstring(nt_heap.reserved())
            , to_wstring(nt_heap.committed())
            , to_wstring(nt_heap.uncommitted())
            , to_wstring(nt_heap.virtual_blocks())
            , to_wstring(nt_heap.total_free_size())
            , locale_formatting::to_wstring(nt_heap.total_segments())
            , locale_formatting::to_wstring(nt_heap.total_ucrs())
            , (nt_heap.is_low_fragment_heap_enabled() ? L" (LFH)"sv : L""sv)
            , (nt_heap.debug_page_heap().has_value() ? L" (DPH)"sv : L""sv)
            , process_heap_marker
        );
    }

    void print_nt_heap_segment_line(std::wostream& log, std::streamsize const hex_length, size_t const segment_index, heap::heap_segment const& segment, std::wstring const& indent_str)
    {
        auto const first_entry = segment.first_entry();
        auto const last_entry = segment.last_entry();
        using namespace size_units::base_16;
        auto const total_size_bytes = bytes{ last_entry - first_entry };
        log << std::format(L"{0}Segment {1} {2}-{3} {4}\n", indent_str, locale_formatting::to_wstring(segment_index), stream_hex_dump::to_hex(first_entry, hex_length), stream_hex_dump::to_hex(last_entry, hex_length), to_wstring(total_size_bytes));
    }

    void print_nt_heap_segments_list(std::wostream& log, std::streamsize const hex_length, heap::nt_heap const& nt_heap, size_t const indent)
    {
        std::wstring const indent_str(indent, L' ');
        for (size_t segment_index = 0; auto const& segment : nt_heap.segments())
        {
            print_nt_heap_segment_line(log, hex_length, segment_index, segment, indent_str);
            ++segment_index;
        }
    }

    void print_lfh_heap_segment_line(std::wostream& log, std::streamsize const hex_length, size_t const segment_index, heap::lfh_segment const& segment, std::wstring const& indent_str)
    {
        using namespace size_units::base_16;
        bytes segment_total{ 0 };
        for (auto const& subsegment : segment.subsegments())
        {
            if (subsegment.address() != 0)
            {
                segment_total += bytes{ subsegment.block_count() * subsegment.block_stride() };
            }
        }

        log << std::format(L"{0}LFH Segment {1} {2} - subsegments {3} - total {4}\n", indent_str, locale_formatting::to_wstring(segment_index), stream_hex_dump::to_hex(segment.address(), hex_length), locale_formatting::to_wstring(segment.subsegments_count()), to_wstring(segment_total));
    }

    void print_nt_heap_lfh_segments_list(std::wostream& log, std::streamsize const hex_length, heap::nt_heap const& nt_heap, size_t const indent)
    {
        if (auto const lfh_heap = nt_heap.lfh_heap(); lfh_heap.has_value())
        {
            std::wstring const indent_str(indent, L' ');
            for (size_t segment_index = 0; auto const& segment : lfh_heap.value().lfh_segments())
            {
                print_lfh_heap_segment_line(log, hex_length, segment_index, segment, indent_str);
                ++segment_index;
            }
        }
    }

    void print_nt_heap(std::wostream& log, std::streamsize const hex_length, std::wstring_view const& process_heap_marker, heap::nt_heap const& nt_heap, size_t const indent)
    {
        std::wstring const indent_str(indent, L' ');
        log << std::format(L"{0}NT Heap: {1} {2}\n", indent_str, stream_hex_dump::to_hex(nt_heap.nt_heap_address(), hex_length), process_heap_marker);
        log << std::format(L"{0}  Flags: {1}\n", indent_str, stream_hex_dump::to_hex_full(nt_heap.flags()));
        using namespace size_units::base_16;
        log << std::format(L"{0}  Total Reserved: {1} ({2})\n", indent_str, to_wstring(nt_heap.reserved()), stream_hex_dump::to_hex(nt_heap.reserved()));
        log << std::format(L"{0}  Total Committed: {1} ({2})\n", indent_str, to_wstring(nt_heap.committed()), stream_hex_dump::to_hex(nt_heap.committed()));
        log << std::format(L"{0}  Total Uncommitted: {1} ({2})\n", indent_str, to_wstring(nt_heap.uncommitted()), stream_hex_dump::to_hex(nt_heap.uncommitted()));
        log << std::format(L"{0}  Total Virtual Blocks: {1} ({2})\n", indent_str, to_wstring(nt_heap.virtual_blocks()), stream_hex_dump::to_hex(nt_heap.virtual_blocks()));
        log << std::format(L"{0}  Total Free Size: {1} ({2})\n", indent_str, to_wstring(nt_heap.total_free_size()), stream_hex_dump::to_hex(nt_heap.total_free_size()));
        log << std::format(L"{0}  Total Segments: {1}\n", indent_str, locale_formatting::to_wstring(nt_heap.total_segments()));
        log << std::format(L"{0}  Total Uncommitted Ranges: {1}\n", indent_str, locale_formatting::to_wstring(nt_heap.total_ucrs()));
        log << std::format(L"{0}  Granularity: {1}\n", indent_str, to_wstring(bytes{ nt_heap.granularity() }));
        log << std::format(L"{0}  Is Heap Entry Encoded: {1}\n", indent_str, nt_heap.is_encoded());
        log << std::format(L"{0}  Allocator Back Trace Index : {1}\n", indent_str, locale_formatting::to_wstring(nt_heap.allocator_back_trace_index()));
        log << std::format(L"{0}  Front End Heap Type : {1}{2}\n", indent_str, locale_formatting::to_wstring(nt_heap.front_end_heap_type()), nt_heap.is_low_fragment_heap_enabled() ? L" (LFH)"sv : L""sv);
        if (nt_heap.is_low_fragment_heap_enabled())
        {
            if (auto const lfh_heap = nt_heap.lfh_heap(); lfh_heap.has_value())
            {
                if (auto const lfh_key = lfh_heap.value().lfh_key(); lfh_key.has_value())
                {
                    log << std::format(L"{0}  LFH Key : {1}\n", indent_str,  stream_hex_dump::to_hex(lfh_key.value()));
                }
            }
        }
        log << L'\n';
    }

    void print_entry_flags_break_down(std::wostream& log, heap::heap_entry const& entry)
    {
        auto const busy = (entry.flags() & heap::heap_entry::FlagBusy) == heap::heap_entry::FlagBusy;
        if (busy)
        {
            log << L", Busy";
        }
        else
        {
            log << L", Free";
        }
        if ((entry.flags() & heap::heap_entry::FlagVirtualAlloc) == heap::heap_entry::FlagVirtualAlloc)
        {
            log << (busy ? L", Internal" : L", Decommitted");
        }
        if ((entry.flags() & heap::heap_entry::FlagExtraPresent) == heap::heap_entry::FlagExtraPresent)
        {
            log << L", Tail fill";
        }
        if ((entry.flags() & heap::heap_entry::FlagFillPattern) == heap::heap_entry::FlagFillPattern)
        {
            log << L", Fill";
        }
        if ((entry.flags() & heap::heap_entry::FlagLastEntry) == heap::heap_entry::FlagLastEntry)
        {
            log << L", Last";
        }
        if ((entry.flags() & heap::heap_entry::FlagUserMask) != 0)
        {
            log << std::format(L", User Flag {0}", static_cast<uint16_t>(entry.flags() >> heap::heap_entry::FlagUserBitOffset));
        }

        if (entry.is_front_padded())
        {
            log << L", Front Padded";
        }

        if (!entry.is_valid())
        {
            log << L", Invalid Checksum";
        }
    }

    void print_dump_entry_single_line(std::wostream& log, std::streamsize const hex_length, heap::heap_entry const& entry, size_t const indent)
    {
        using namespace size_units::base_16;
        log << std::format(L"{0:{1}}{2} Size({3})", L' ', indent, stream_hex_dump::to_hex(entry.address(), hex_length), to_wstring(entry.size()));
        if (entry.is_uncommitted())
        {
            log << L" - Uncommitted\n";
        }
        else if (entry.is_unknown())
        {
            log << L" - Unknown Range\n";
        }
        else
        {
            if (entry.is_busy())
            {
                log << std::format(L" ReqSize({0}) UnusedBytes({1})", to_wstring(entry.user_requested_size()), to_wstring(entry.unused_bytes()));
                if (entry.end_unused_bytes() > bytes{ 0 })
                {
                    log << std::format(L" EndUnusedBytes({})", to_wstring(entry.end_unused_bytes()));
                }
                if (entry.user_address() != 0)
                {
                    log << std::format(L" UsrPtr({})", stream_hex_dump::to_hex(entry.user_address(), hex_length));
                }
            }

            if (entry.is_lfh_entry())
            {
                log << (entry.is_lfh_busy() ? L" Busy" : L" Free");
            }
            else
            {
                log << std::format(L" Flags({})", stream_hex_dump::to_hex(entry.flags()));
                print_entry_flags_break_down(log, entry);
            }

            log << L'\n';
        }
    }

    void print_dump_entry_debug(std::wostream& log, std::streamsize const hex_length, heap::heap_entry const& entry, size_t const indent)
    {
        using namespace size_units::base_16;
        std::wstring const indent_str(indent, L' ');
        if (entry.is_uncommitted())
        {
            log << std::format(L"{0}Uncommitted Entry: {1}\n", indent_str, stream_hex_dump::to_hex(entry.address(), hex_length));
            log << std::format(L"{0}  Size: {1} ({2})\n", indent_str, to_wstring(entry.size()), stream_hex_dump::to_hex(entry.size()));
        }
        else if (entry.is_unknown())
        {
            log << std::format(L"{0}Unknown Range Entry: {1}\n", indent_str, stream_hex_dump::to_hex(entry.address(), hex_length));
            log << std::format(L"{0}  Size: {1} ({2})\n", indent_str, to_wstring(entry.size()), stream_hex_dump::to_hex(entry.size()));
        }
        else
        {
            log << std::format(L"{0}{1}{2} Entry: {3}\n", indent_str, entry.is_lfh_entry() ? L"LFH "sv : L""sv, entry.is_busy() ? L"Busy"sv : L"Free"sv, stream_hex_dump::to_hex(entry.address(), hex_length));
            log << std::format(L"{0}  Size: {1} ({2})\n", indent_str, to_wstring(entry.size()), stream_hex_dump::to_hex(entry.size()));
            if (entry.is_busy())
            {
                log << std::format(L"{0}  User Requested Size: {1} ({2})\n", indent_str, to_wstring(entry.user_requested_size()), stream_hex_dump::to_hex(entry.user_requested_size()));
                log << std::format(L"{0}  Unused Bytes: {1} ({2})\n", indent_str, to_wstring(entry.unused_bytes()), stream_hex_dump::to_hex(entry.unused_bytes()));
                log << std::format(L"{0}  Segment Offset: {1} ({2})\n", indent_str, locale_formatting::to_wstring(entry.segment_offset()), stream_hex_dump::to_hex(entry.segment_offset()));
                log << std::format(L"{0}  Small Tag Index: {1} ({2})\n", indent_str, locale_formatting::to_wstring(entry.small_tag_index()), stream_hex_dump::to_hex(entry.small_tag_index()));
                if (entry.end_unused_bytes() > bytes{ 0 })
                {
                    log << std::format(L"{0}  End Unused Bytes: {1} ({2})\n", indent_str, to_wstring(entry.end_unused_bytes()), stream_hex_dump::to_hex(entry.end_unused_bytes()));
                }
                if (entry.user_address() != 0)
                {
                    log << std::format(L"{0}  User Address: {1}\n", indent_str, stream_hex_dump::to_hex(entry.user_address(), hex_length));
                }
                if (entry.ust_address() != 0)
                {
                    log << std::format(L"{0}  UST Address: {1}\n", indent_str, stream_hex_dump::to_hex(entry.ust_address(), hex_length));
                }
            }

            if (!entry.is_lfh_entry())
            {
                log << std::format(L"{0}  Flags: {1}", indent_str, stream_hex_dump::to_hex(entry.flags()));
                print_entry_flags_break_down(log, entry);
                log << L'\n';
            }
        }
    }

    void print_dump_entry(std::wostream& log, std::streamsize const hex_length, heap::heap_entry const& entry, bool const entry_contains_lfh_subsegments, dump_file_options const& options, size_t const indent)
    {
        if(options.debug_heap_data())
        {
            print_dump_entry_debug(log, hex_length, entry, indent);
        }
        else
        {
            print_dump_entry_single_line(log, hex_length, entry, indent);
        }

        if (entry.is_unknown())
        {
            if(options.hex_dump_memory_data())
            {
                const uint64_t size = entry.size().count();
                if(auto stream = entry.walker().get_process_memory_stream(entry.address(), size); !stream.eof())
                {
                    hex_dump::hex_dump(log, stream, options.hex_dump_memory_size(size), indent + 2);
                    log << L'\n';
                }
            }
        }
        else if(!entry.is_uncommitted())
        {
            if(options.display_symbols() && entry.is_busy() && !entry.allocation_stack_trace().empty())
            {
                log << std::format(L"{0:{1}}Allocation Stack Trace:\n", L' ', indent + 2);
                hex_dump_stack(log, entry.walker(), entry.allocation_stack_trace(), entry.peb().is_x86_target(), indent + 2);
                log << L'\n';
            }

            if(options.hex_dump_memory_data() && entry.is_busy() && entry.user_address() != 0 && ((entry.flags() & heap::heap_entry::FlagVirtualAlloc) != heap::heap_entry::FlagVirtualAlloc || !entry_contains_lfh_subsegments))
            {
                uint64_t const size = entry.user_requested_size().count();
                if(auto stream = entry.walker().get_process_memory_stream(entry.user_address(), size); !stream.eof())
                {
                    hex_dump::hex_dump(log, stream, options.hex_dump_memory_size(size), indent + 2);
                    log << L'\n';
                }
            }
        }
    }

    void print_lfh_heap_segment_single_line(std::wostream& log, std::streamsize const hex_length, LfhSubsegmentData& data, size_t const indent)
    {
        using namespace size_units::base_16;

        data.printed_subsegment = true;
        bytes const total{ data.subsegment.address() ? data.subsegment.block_count() * data.subsegment.block_stride() : 0 };
        log << std::format(L"{0:{1}}LFH Segment {2} Subsegment {3}, block count {4}, block size {5}, entry data {6}, total {7}\n"
            , L' ', indent
            , locale_formatting::to_wstring(data.segment->segment_index)
            , stream_hex_dump::to_hex(data.subsegment.address(), hex_length)
            , locale_formatting::to_wstring(data.subsegment.block_count())
            , to_wstring(data.subsegment.block_size())
            , stream_hex_dump::to_hex(data.subsegment.entry_start_address(), hex_length)
            , to_wstring(total));
    }

    void print_lfh_heap_segment_debug(std::wostream& log, std::streamsize const hex_length, LfhSubsegmentData& data, size_t const indent)
    {
        using namespace size_units::base_16;
        std::wstring const indent_str(indent, L' ');

        data.printed_subsegment = true;
        bytes const total{ data.subsegment.address() ? data.subsegment.block_count() * data.subsegment.block_stride() : 0 };
        log << std::format(L"{0}LFH Segment: {1} @ {2}\n", indent_str, locale_formatting::to_wstring(data.segment->segment_index), stream_hex_dump::to_hex(data.segment->segment.address(), hex_length));
        log << std::format(L"{0}  Subsegment: {1}\n", indent_str, stream_hex_dump::to_hex(data.subsegment.address(), hex_length));
        log << std::format(L"{0}  Block Count: {1}\n", indent_str, locale_formatting::to_wstring(data.subsegment.block_count()));
        log << std::format(L"{0}  Block Size: {1} ({2})\n", indent_str, to_wstring(data.subsegment.block_size()), stream_hex_dump::to_hex(data.subsegment.block_size()));
        log << std::format(L"{0}  Entry Data: {1}\n", indent_str, stream_hex_dump::to_hex(data.subsegment.entry_start_address(), hex_length));
        log << std::format(L"{0}  Total: {1} ({2})\n", indent_str, to_wstring(total), stream_hex_dump::to_hex(total));
    }

    void print_lfh_heap_segment(std::wostream& log, std::streamsize const hex_length, LfhSubsegmentData& data, dump_file_options const& options, size_t const indent)
    {
        if(options.debug_heap_data())
        {
            print_lfh_heap_segment_debug(log, hex_length, data, indent);
        }
        else
        {
            print_lfh_heap_segment_single_line(log, hex_length, data, indent);
        }

        for (auto const& entry : data.subsegment.entries())
        {
            print_dump_entry(log, hex_length, entry, false, options, indent + 2);
        }
    }

    bool is_lfh_subsegment_in_entry(heap::heap_entry const& entry, LfhSubsegmentData const& data, bool const match_any_entry = false)
    {
        auto const is_internal_entry = match_any_entry || (entry.flags() & (heap::heap_entry::FlagBusy | heap::heap_entry::FlagVirtualAlloc)) == (heap::heap_entry::FlagBusy | heap::heap_entry::FlagVirtualAlloc);
        return !data.printed_subsegment && is_internal_entry && data.subsegment.entry_start_address() > entry.address() && data.subsegment.entry_start_address() < entry.address() + entry.size().count();
    }

    void print_heap_segment(std::wostream& log, std::streamsize const hex_length, size_t const segment_index, heap::heap_segment const& segment, vector<LfhSubsegmentData>& lfh_data, dump_file_options const& options, size_t const indent)
    {
        using namespace size_units::base_16;
        std::wstring const indent_str(indent, L' ');
        print_nt_heap_segment_line(log, hex_length, segment_index, segment, indent_str);
        log << std::format(L"{0}   Flags: {1}\n", indent_str, stream_hex_dump::to_hex(segment.segment_flags()));
        log << std::format(L"{0}   Number of Pages: {1}\n", indent_str, stream_hex_dump::to_hex(segment.number_of_pages()));
        log << std::format(L"{0}   Number of Uncommitted Pages: {1}\n", indent_str, stream_hex_dump::to_hex(segment.number_of_uncommitted_pages()));
        log << std::format(L"{0}   Number of Uncommitted Ranges: {1}\n", indent_str, stream_hex_dump::to_hex(segment.number_of_uncommitted_ranges()));
        log << L'\n';

        log << std::format(L"{0}Uncommitted Ranges\n", indent_str);
        for (auto const& uncommitted_range : segment.uncommitted_ranges())
        {
            log << std::format(L"{0}    Range {1} for {2}\n", indent_str, stream_hex_dump::to_hex(uncommitted_range.address()), to_wstring(uncommitted_range.size()));
        }

        log << std::format(L"\n{}Heap Entries\n", indent_str);
        for (auto const& entry : segment.entries())
        {
            auto const entry_contains_lfh_subsegments = ranges::any_of(lfh_data, [&entry](LfhSubsegmentData const& data) { return is_lfh_subsegment_in_entry(entry, data); });
            print_dump_entry(log, hex_length, entry, entry_contains_lfh_subsegments, options, indent + 4);

            for (auto& data : lfh_data)
            {
                if (is_lfh_subsegment_in_entry(entry, data))
                {
                    print_lfh_heap_segment(log, hex_length, data, options, indent + 6);
                }
            }
        }
        log << L'\n';
    }

    std::wstring_view get_process_marker(bool const is_process_heap)
    {
        return is_process_heap ? L" (process heap)"sv : L""sv;
    }

    void print_nt_heap_uncommitted_ranges(std::wostream& log, heap::nt_heap const& nt_heap, size_t const indent)
    {
        using namespace size_units::base_16;
        std::wstring const indent_str(indent, L' ');
        log << std::format(L"{}Heap Uncommitted ranges\n", indent_str);
        for (auto const& uncommitted_range : nt_heap.uncommitted_ranges())
        {
            log << std::format(L"{0}  Range {1} for {2}\n", indent_str, stream_hex_dump::to_hex(uncommitted_range.address()), to_wstring(uncommitted_range.size()));
        }
        log << L'\n';
    }

    void print_nt_heap_virtual_allocated_blocks(std::wostream& log, std::streamsize const hex_length, heap::nt_heap const& nt_heap, dump_file_options const& options, size_t const indent)
    {
        using namespace size_units::base_16;
        std::wstring const indent_str(indent, L' ');
        log << std::format(L"{0}Virtual Allocated Blocks\n", indent_str);
        for (auto const& virtual_block : nt_heap.heap_virtual_blocks())
        {
            log << std::format(L"{0}  Entry {1} data {2} reversed {3} committed {4}\n"
                , indent_str
                , stream_hex_dump::to_hex(virtual_block.descriptor_address())
                , stream_hex_dump::to_hex(virtual_block.address())
                , to_wstring(virtual_block.reserved())
                , to_wstring(virtual_block.committed()));
            for (auto const& entry : virtual_block.entries())
            {
                print_dump_entry(log, hex_length, entry, false, options, indent + 4);
            }
        }
        log << L'\n';
    }

    void print_nt_heap_free_list(std::wostream& log, std::streamsize const hex_length, heap::nt_heap const& nt_heap, dump_file_options const& options, size_t const indent)
    {
        log << std::format(L"{0:{1}}Free List Entries\n", ' ', indent);
        for (auto const& free_entry : nt_heap.free_entries())
        {
            print_dump_entry(log, hex_length, free_entry, false, options, indent + 4);
        }
        log << L'\n';
    }

    void print_debug_page_heap_entry_single_line(std::wostream& log, streamsize const hex_length, size_t const index, heap::dph_entry const& entry, size_t const indent)
    {
        using namespace size_units::base_16;
        log << std::format(L"{0:{1}}{2} @ {3} {4} BlockPtr({5}) BlockSize({6}) UserPtr({7}) ReqSize({8})\n", ' ', indent
            , locale_formatting::to_wstring(index)
            , stream_hex_dump::to_hex(entry.entry_address(), hex_length)
            , entry.is_allocated() ? L"Busy"sv : L"Free"sv
            , stream_hex_dump::to_hex(entry.virtual_block_address(), hex_length)
            , to_wstring(entry.virtual_block_size())
            , stream_hex_dump::to_hex(entry.user_address(), hex_length)
            , to_wstring(entry.user_requested_size()));
    }

    void print_debug_page_heap_entry_debug(std::wostream& log, streamsize const hex_length, size_t const index, heap::dph_entry const& entry, size_t const indent)
    {
        using namespace size_units::base_16;
        std::wstring const indent_str(indent, L' ');
        log << std::format(L"{0}{1} @ {2}\n", indent_str, locale_formatting::to_wstring(index), stream_hex_dump::to_hex(entry.entry_address(), hex_length));
        log << std::format(L"{0}  Is Allocated: {1}\n", indent_str, entry.is_allocated());
        log << std::format(L"{0}  Virtual Block: {1}\n", indent_str, stream_hex_dump::to_hex(entry.virtual_block_address(), hex_length));
        log << std::format(L"{0}  Virtual Block Size: {1} ({2})\n", indent_str, to_wstring(entry.virtual_block_size()), stream_hex_dump::to_hex(entry.virtual_block_size().count()));
        log << std::format(L"{0}  User Allocation: {1}\n", indent_str, stream_hex_dump::to_hex(entry.user_address(), hex_length));
        log << std::format(L"{0}  User Requested Size: {1} ({2})\n", indent_str, to_wstring(entry.user_requested_size()), stream_hex_dump::to_hex(entry.user_requested_size().count()));
        log << std::format(L"{0}  UST Address: {1}\n", indent_str, stream_hex_dump::to_hex(entry.ust_address(), hex_length));
    }

    void print_debug_page_heap_entry(std::wostream& log, streamsize const hex_length, size_t const index, heap::dph_entry const& entry, dump_file_options const& options, size_t const indent)
    {
        if(options.debug_heap_data())
        {
            print_debug_page_heap_entry_debug(log, hex_length, index, entry, indent);
        }
        else
        {
            print_debug_page_heap_entry_single_line(log, hex_length, index, entry, indent);
        }

        if(options.display_symbols() && entry.is_allocated() && !entry.allocation_stack_trace().empty())
        {
            std::wstring const indent_str(indent, L' ');
            log << std::format(L"{0:{1}}Allocation Stack Trace:\n", L' ', indent + 2);
            hex_dump_stack(log, entry.heap().walker(), entry.allocation_stack_trace(), entry.heap().peb().is_x86_target(), indent + 4);
            log << L'\n';
        }

        if(options.hex_dump_memory_data() && entry.is_allocated() && entry.user_address() != 0)
        {
            uint64_t const size = entry.user_requested_size().count();
            if(auto stream = entry.walker().get_process_memory_stream(entry.user_address(), size); !stream.eof())
            {
                hex_dump::hex_dump(log, stream, options.hex_dump_memory_size(size), indent + 4);
                log << L'\n';
            }
        }
    }

    void print_debug_page_heap(std::wostream& log, streamsize const hex_length, heap::dph_heap const& heap, dump_file_options const& options, size_t const indent)
    {
        std::wstring const indent_str(indent, L' ');
        using namespace size_units::base_16;
        log << std::format(L"{0}Debug Page Heap: {1}\n", indent_str, stream_hex_dump::to_hex(heap.address(), hex_length));
        log << std::format(L"{0}  Flags: {1}\n", indent_str, stream_hex_dump::to_hex(heap.flags()));
        log << std::format(L"{0}  Extra Flags: {1}\n", indent_str, stream_hex_dump::to_hex(heap.extra_flags()));
        log << std::format(L"{0}  Seed: {1}\n", indent_str, stream_hex_dump::to_hex(heap.seed()));
        log << std::format(L"{0}  Normal Heap: {1}\n", indent_str, stream_hex_dump::to_hex(heap.normal_heap(), hex_length));
        log << std::format(L"{0}  Busy Allocations: {1}\n", indent_str, locale_formatting::to_wstring(heap.busy_allocations()));
        log << std::format(L"{0}  Busy Allocations Committed Total: {1} ({2})\n", indent_str, to_wstring(heap.busy_allocations_committed()), stream_hex_dump::to_hex(heap.busy_allocations_committed()));
        log << std::format(L"{0}  Virtual Ranges: {1}\n", indent_str, locale_formatting::to_wstring(heap.virtual_storage_ranges()));
        log << std::format(L"{0}  Virtual Ranges Total: {1} ({2})\n", indent_str, to_wstring(heap.virtual_storage_total()), stream_hex_dump::to_hex(heap.virtual_storage_total()));
        log << std::format(L"{0}  Free Allocations: {1}\n", indent_str, locale_formatting::to_wstring(heap.free_allocations()));
        log << std::format(L"{0}  Free Allocations Committed Total: {1} ({2})\n", indent_str, to_wstring(heap.free_allocations_committed()), stream_hex_dump::to_hex(heap.free_allocations_committed()));
        log << std::format(L"{0}  Busy Entries: ({1})\n", indent_str, locale_formatting::to_wstring(heap.busy_allocations()));

        size_t index = 1;
        for(auto const& entry : heap.busy_entries())
        {
            print_debug_page_heap_entry(log, hex_length, index, entry, options, indent + 4);
            ++index;
        }

        log << std::format(L"{0}  Virtual Ranges: ({1})\n", indent_str, locale_formatting::to_wstring(heap.virtual_storage_ranges()));

        index = 1;
        for(auto const& entry : heap.virtual_ranges())
        {
            print_debug_page_heap_entry(log, hex_length, index, entry, options, indent + 4);
            ++index;
        }

        log << std::format(L"{0}  Free Allocations: ({1})\n", indent_str, locale_formatting::to_wstring(heap.free_allocations()));

        index = 1;
        for(auto const& entry : heap.free_entries())
        {
            print_debug_page_heap_entry(log, hex_length, index, entry, options, indent + 4);
            ++index;
        }
        log << L'\n';
    }

    void print_segment_heap_line(std::wostream& log, std::wstring_view const& process_heap_marker, heap::segment_heap const& segment_heap)
    {
        using namespace size_units::base_16;
        log << std::format(L" Segment Heap {0} {2:<{1}} {3:<{1}} {4:<{1}} {5:<{1}} {6}\n"
            , stream_hex_dump::to_hex_full(segment_heap.global_flags())
            , units_max_width
            , to_wstring(segment_heap.reserved())
            , to_wstring(segment_heap.committed())
            , to_wstring(segment_heap.uncommitted())
            , to_wstring(segment_heap.large_reserved())
            , process_heap_marker);
    }

    void print_segment_heap_page_entry_single_line(std::wostream& log, streamsize const hex_length, heap::page_range_descriptor const& entry, size_t const indent)
    {
        using namespace size_units::base_16;
        auto const range_flags = entry.range_flags();
        log << std::format(L"{0:{1}}Page Entry: {2} : {3} Extra({4}) BlockPtr({5}) BlockSize({6}) UserPtr({7}) ReqSize({8}) Flags({9}) {10}\n", ' ', indent
            , locale_formatting::to_wstring(entry.index())
            , stream_hex_dump::to_hex(entry.page_range_descriptor_address(), hex_length)
            , to_wstring(entry.extra_bytes())
            , stream_hex_dump::to_hex(entry.block_address(), hex_length)
            , to_wstring(entry.block_size())
            , stream_hex_dump::to_hex(entry.user_address(), hex_length)
            , to_wstring(entry.user_requested_size())
            , stream_hex_dump::to_hex(static_cast<uint32_t>(range_flags))
            , dump_page_range_to_string(range_flags));
    }

    void print_segment_heap_page_entry_debug(std::wostream& log, streamsize const hex_length, heap::page_range_descriptor const& entry, size_t const indent)
    {
        std::wstring const indent_str(indent, L' ');
        using namespace size_units::base_16;
        auto const range_flags = entry.range_flags();
        log << std::format(L"{0}Page Entry: {1} : {2}\n", indent_str, locale_formatting::to_wstring(entry.index()), stream_hex_dump::to_hex(entry.page_range_descriptor_address(), hex_length));
        log << std::format(L"{0}  Extra Present: {1}\n", indent_str, entry.extra_present());
        log << std::format(L"{0}  Range Flags: {1} ({2}) - {3}\n", indent_str, stream_hex_dump::to_hex(static_cast<uint32_t>(range_flags)), dump_page_range_flags_to_string(range_flags), dump_page_range_to_string(range_flags));
        log << std::format(L"{0}  Extra Bytes: {1} ({2})\n", indent_str, to_wstring(entry.extra_bytes()), stream_hex_dump::to_hex(entry.extra_bytes()));
        log << std::format(L"{0}  Committed Page Count: {1}\n", indent_str, locale_formatting::to_wstring(entry.committed_page_count()));
        log << std::format(L"{0}  Unit Offset: {1}\n", indent_str, locale_formatting::to_wstring(entry.unit_offset()));
        log << std::format(L"{0}  Unit Size: {1} ({2})\n", indent_str, to_wstring(entry.unit_size()), stream_hex_dump::to_hex(entry.unit_size()));
        log << std::format(L"{0}  Block Address: {1}\n", indent_str, stream_hex_dump::to_hex(entry.block_address(), hex_length));
        log << std::format(L"{0}  Block Size: {1} ({2})\n", indent_str, to_wstring(entry.block_size()), stream_hex_dump::to_hex(entry.block_size()));
        log << std::format(L"{0}  User Address: {1}\n", indent_str, stream_hex_dump::to_hex(entry.user_address(), hex_length));
        log << std::format(L"{0}  Requested User Size: {1} ({2})\n", indent_str, to_wstring(entry.user_requested_size()), stream_hex_dump::to_hex(entry.user_requested_size()));
        log << std::format(L"{0}  UST Address: {1}\n", indent_str, stream_hex_dump::to_hex(entry.ust_address(), hex_length));
    }

    void print_segment_heap_page_entry(std::wostream& log, streamsize const hex_length, heap::page_range_descriptor const& entry, dump_file_options const& options, size_t const indent)
    {
        if(options.debug_heap_data())
        {
            print_segment_heap_page_entry_debug(log, hex_length, entry, indent);
        }
        else
        {
            print_segment_heap_page_entry_single_line(log, hex_length, entry, indent);
        }

        if(options.display_symbols() && !entry.allocation_stack_trace().empty())
        {
            log << std::format(L"{0:{1}}Allocation Stack Trace:\n", L' ', indent + 2);
            hex_dump_stack(log, entry.heap().walker(), entry.allocation_stack_trace(), entry.heap().peb().is_x86_target(), indent + 4);
            log << L'\n';
        }

        if(options.hex_dump_memory_data() && entry.range_flags() == page_range_flags_utils::page_range_flags::PAGE_RANGE_BACKEND_SUBSEGMENT && entry.user_address() != 0)
        {
            uint64_t const size = entry.user_requested_size().count();
            if(auto stream = entry.walker().get_process_memory_stream(entry.user_address(), size); !stream.eof())
            {
                hex_dump::hex_dump(log, stream, options.hex_dump_memory_size(size), indent + 4);
                log << L'\n';
            }
        }
    }

    void print_segment_heap_page(std::wostream& log, streamsize const hex_length, size_t const page_index, heap::heap_page_segment const& page, dump_file_options const& options, size_t const indent)
    {
        std::wstring const indent_str(indent, L' ');
        using namespace size_units::base_16;
        log << std::format(L"{0}Segment Page: {1} : {2}\n" , indent_str, locale_formatting::to_wstring(page_index), stream_hex_dump::to_hex(page.heap_page_segment_address(), hex_length));
        log << std::format(L"{0}  Signature: {1}{2}\n", indent_str, stream_hex_dump::to_hex(page.signature(), hex_length), page.is_signature_valid() ? L" (valid)"sv : L""sv);

        if(options.debug_heap_data())
        {
            for(auto const& entry : page.all_entries())
            {
                print_segment_heap_page_entry(log, hex_length, entry, options, indent + 2);
            }
        }
        else
        {
            for(auto const& entry : page.entries())
            {
                print_segment_heap_page_entry(log, hex_length, entry, options, indent + 2);
            }
        }
    }

    void print_segment_context_heap(std::wostream& log, streamsize const hex_length, size_t const segment_context_index, heap::heap_segment_context const& segment_context, dump_file_options const& options, size_t const indent)
    {
        std::wstring const indent_str(indent, L' ');
        using namespace size_units::base_16;
        log << std::format(L"{0}Segment Context: {1} : {2}\n", indent_str, locale_formatting::to_wstring(segment_context_index), stream_hex_dump::to_hex(segment_context.heap_segment_context_address(), hex_length));
        log << std::format(L"{0}  Segment Mask: {1}\n", indent_str, stream_hex_dump::to_hex(segment_context.segment_mask(), hex_length));
        log << std::format(L"{0}  Max Allocation Size: {1} ({2})\n", indent_str, to_wstring(segment_context.max_allocation_size()), stream_hex_dump::to_hex(segment_context.max_allocation_size()));
        log << std::format(L"{0}  Unit Shift: {1}\n", indent_str, stream_hex_dump::to_hex(segment_context.unit_shift()));
        log << std::format(L"{0}  Pages Per Unit Shift: {1}\n", indent_str, locale_formatting::to_wstring(segment_context.pages_per_unit_shift()));
        log << std::format(L"{0}  Segment Count: {1}\n", indent_str, locale_formatting::to_wstring(segment_context.segment_count()));

        size_t index{0};
        for(auto const& page : segment_context.pages())
        {
            print_segment_heap_page(log, hex_length, index, page, options, indent + 2);
            ++index;
        }

        auto first = true;
        for(auto const& entry : segment_context.free_page_ranges())
        {
            if(first)
            {
                log << std::format(L"{0}  Free Page Ranges:\n", indent_str);
                first = false;
            }
            print_segment_heap_page_entry(log, hex_length, entry, options, indent + 4);
        }
    }

    std::wstring_view get_vs_entry_title(heap::heap_vs_entry const& entry)
    {
        return (entry.uncommitted_range() ? L"Uncommitted"sv : (entry.allocated() ? L"Busy"sv : L"Free"sv));
    }

    void print_vs_entry_single_line(std::wostream& log, streamsize const hex_length, heap::heap_vs_entry const& entry, size_t const indent)
    {
        using namespace size_units::base_16;
        log << std::format(L"{0:{1}}VS {2} Entry: {3} size({4} - {5})", L' ', indent
            , get_vs_entry_title(entry)
            , stream_hex_dump::to_hex(entry.heap_vs_entry_address(), hex_length)
            , to_wstring(entry.size())
            , stream_hex_dump::to_hex(entry.size()));

        if(!entry.uncommitted_range())
        {
            log << std::format(L" PSize({})" , to_wstring(entry.previous_size()));
            if(entry.has_unused_bytes())
            {
                log << std::format(L" UnusedBytes({})", to_wstring(entry.unused_bytes()));
            }
            log << std::format(L" UserPtr({0}) ReqSize({1})"
                , stream_hex_dump::to_hex(entry.user_address(), hex_length)
                , to_wstring(entry.user_requested_size()));
            if(!entry.is_valid())
            {
                log << L" - invalid";
            }
        }

        log << L'\n';
    }

    void print_vs_entry_debug(std::wostream& log, streamsize const hex_length, heap::heap_vs_entry const& entry, size_t const indent)
    {
        std::wstring const indent_str(indent, L' ');
        using namespace size_units::base_16;
        log << std::format(L"{0}VS {1} Entry: {2}\n", indent_str
            , get_vs_entry_title(entry)
            , stream_hex_dump::to_hex(entry.heap_vs_entry_address(), hex_length));
        log << std::format(L"{0}  Size: {1} ({2})\n", indent_str, to_wstring(entry.size()), stream_hex_dump::to_hex(entry.size()));

        if(!entry.uncommitted_range())
        {
            log << std::format(L"{0}  Previous Size: {1} ({2})\n", indent_str, to_wstring(entry.previous_size()), stream_hex_dump::to_hex(entry.previous_size()));
            log << std::format(L"{0}  Is Valid: {1}\n", indent_str, entry.is_valid());
            log << std::format(L"{0}  Allocated: {1}\n", indent_str, entry.allocated());
            log << std::format(L"{0}  Memory Cost: {1}\n", indent_str, locale_formatting::to_wstring(entry.memory_cost()));
            log << std::format(L"{0}  Segment Page Offset: {1}\n", indent_str, locale_formatting::to_wstring(entry.segment_page_offset()));
            log << std::format(L"{0}  Has Unused Bytes: {1}\n", indent_str, entry.has_unused_bytes());
            if(entry.has_unused_bytes())
            {
                log << std::format(L"{0}  Unused Bytes: {1}\n", indent_str, to_wstring(entry.unused_bytes()));
            }
            log << std::format(L"{0}  Skip During Walk: {1}\n", indent_str, entry.skip_during_walk());
            log << std::format(L"{0}  Spare: {1}\n", indent_str, stream_hex_dump::to_hex(entry.spare()));
            log << std::format(L"{0}  Allocated Chunk Bits: {1}\n", indent_str, stream_hex_dump::to_hex(entry.allocated_chunk_bits()));
            log << std::format(L"{0}  User Address: {1}\n", indent_str, stream_hex_dump::to_hex(entry.user_address(), hex_length));
            log << std::format(L"{0}  Requested User Size: {1} ({2})\n", indent_str, to_wstring(entry.user_requested_size()), stream_hex_dump::to_hex(entry.user_requested_size()));
            log << std::format(L"{0}  UST Address: {1}\n", indent_str, stream_hex_dump::to_hex(entry.ust_address(), hex_length));
        }
    }

    void print_vs_entry(std::wostream& log, streamsize const hex_length, heap::heap_vs_entry const& entry, dump_file_options const& options, size_t const indent)
    {
        if(options.debug_heap_data())
        {
            print_vs_entry_debug(log, hex_length, entry, indent);
        }
        else
        {
            print_vs_entry_single_line(log, hex_length, entry, indent);
        }

        if(!entry.uncommitted_range())
        {
            if(options.display_symbols() && entry.allocated() && !entry.allocation_stack_trace().empty())
            {
            log << std::format(L"{0:{1}}Allocation Stack Trace:\n", L' ', indent + 2);
                hex_dump_stack(log, entry.heap().walker(), entry.allocation_stack_trace(), entry.heap().peb().is_x86_target(), indent + 4);
                log << L'\n';
            }

            if(options.hex_dump_memory_data() && entry.allocated())
            {
                uint64_t const size = entry.user_requested_size().count();
                if(auto stream = entry.walker().get_process_memory_stream(entry.user_address(), size); !stream.eof())
                {
                    hex_dump::hex_dump(log, stream, options.hex_dump_memory_size(size), indent + 2);
                    log << L'\n';
                }
            }
        }
    }

    void print_vs_subsegment(std::wostream& log, streamsize const hex_length, heap::heap_vs_subsegment const& subsegment, dump_file_options const& options, size_t const indent)
    {
        std::wstring const indent_str(indent, L' ');
        using namespace size_units::base_16;
        log << std::format(L"{0}VS Subsegment: {1}\n", indent_str, stream_hex_dump::to_hex(subsegment.heap_vs_subsegment_address(), hex_length));
        log << std::format(L"{0}  Size: {1} ({2})\n", indent_str, to_wstring(subsegment.size()), stream_hex_dump::to_hex(subsegment.size()));
        log << std::format(L"{0}  Signature: {1}{2}\n", indent_str, stream_hex_dump::to_hex(subsegment.signature()), subsegment.is_signature_valid() ? L" (valid)"sv : L" (invalid)"sv);
        log << std::format(L"{0}  Full Commit: {1}\n", indent_str, subsegment.full_commit());

        for(auto const& entry : subsegment.entries())
        {
            print_vs_entry(log, hex_length, entry, options, indent + 2);
        }
    }

    void print_vs_context_heap(std::wostream& log, streamsize const hex_length, heap::heap_vs_context const& vs_context, dump_file_options const& options, size_t const indent)
    {
        std::wstring const indent_str(indent, L' ');
        log << std::format(L"{0}VS Context: {1}\n", indent_str, stream_hex_dump::to_hex(vs_context.heap_vs_context_address(), hex_length));
        log << std::format(L"{0}  Total Committed Units: {1}\n", indent_str, vs_context.total_committed_units());
        log << std::format(L"{0}  Free Committed Units: {1}\n", indent_str, vs_context.free_committed_units());

        for(auto const& subsegment : vs_context.subsegments())
        {
            print_vs_subsegment(log, hex_length, subsegment, options, indent + 2);
        }

        log << std::format(L"{0}  Free Entries:\n", indent_str);
        for(auto const& entry : vs_context.free_entries())
        {
            print_vs_entry(log, hex_length, entry, options, indent + 4);
        }
    }

    void print_heap_large_entry_single_line(std::wostream& log, streamsize const hex_length, heap::large_alloc_entry const& entry, size_t const indent)
    {
        using namespace size_units::base_16;
        log << std::format(L"{0:{1}}Large Entry: {2} Size({3}) VirtualPtr({4}) UnusedBytes({5}) UserPtr({6}) ReqSize({7})\n", L' ', indent
            , stream_hex_dump::to_hex(entry.large_alloc_entry_address(), hex_length)
            , to_wstring(entry.size())
            , stream_hex_dump::to_hex(entry.virtual_address(), hex_length)
            , to_wstring(entry.unused_bytes())
            , stream_hex_dump::to_hex(entry.user_address(), hex_length)
            , to_wstring(entry.user_requested_size()));
    }

    void print_heap_large_entry_debug(std::wostream& log, streamsize const hex_length, heap::large_alloc_entry const& entry, size_t const indent)
    {
        std::wstring const indent_str(indent, L' ');
        using namespace size_units::base_16;
        log << std::format(L"{0}Large Entry: {1}\n", indent_str, stream_hex_dump::to_hex(entry.large_alloc_entry_address(), hex_length));
        log << std::format(L"{0}  Size: {1} ({2})\n", indent_str, to_wstring(entry.size()), stream_hex_dump::to_hex(entry.size()));
        log << std::format(L"{0}  Virtual Address: {1}\n", indent_str, stream_hex_dump::to_hex(entry.virtual_address(), hex_length));
        log << std::format(L"{0}  Unused Bytes: {1} ({2})\n", indent_str, to_wstring(entry.unused_bytes()), stream_hex_dump::to_hex(entry.unused_bytes()));
        log << std::format(L"{0}  Spare: {1}\n", indent_str, stream_hex_dump::to_hex(entry.spare()));
        log << std::format(L"{0}  Extra Present: {1}\n", indent_str, entry.extra_present());
        log << std::format(L"{0}  Guard Page Count: {1}\n", indent_str, locale_formatting::to_wstring(entry.guard_page_count()));
        log << std::format(L"{0}  Guard Page Alignment: {1}\n", indent_str, locale_formatting::to_wstring(entry.guard_page_alignment()));
        log << std::format(L"{0}  Allocated Pages: {1}\n", indent_str, locale_formatting::to_wstring(entry.allocated_pages()));
        log << std::format(L"{0}  User Address: {1}\n", indent_str, stream_hex_dump::to_hex(entry.user_address(), hex_length));
        log << std::format(L"{0}  Requested User Size: {1} ({2})\n", indent_str, to_wstring(entry.user_requested_size()), stream_hex_dump::to_hex(entry.user_requested_size()));
        log << std::format(L"{0}  UST Address: {1}\n", indent_str, stream_hex_dump::to_hex(entry.ust_address(), hex_length));
    }

    void print_heap_large_entry(std::wostream& log, streamsize const hex_length, heap::large_alloc_entry const& entry, dump_file_options const& options, size_t const indent)
    {
        if(options.debug_heap_data())
        {
            print_heap_large_entry_debug(log, hex_length, entry, indent);
        }
        else
        {
            print_heap_large_entry_single_line(log, hex_length, entry, indent);
        }

        if(options.display_symbols() && !entry.allocation_stack_trace().empty())
        {
            log << std::format(L"{0:{1}}Allocation Stack Trace:\n", L' ', indent + 2);
            hex_dump_stack(log, entry.heap().walker(), entry.allocation_stack_trace(), entry.heap().peb().is_x86_target(), indent + 4);
            log << L'\n';
        }

        if(options.hex_dump_memory_data())
        {
            uint64_t const size = entry.user_requested_size().count();
            if(auto stream = entry.walker().get_process_memory_stream(entry.user_address(), size); !stream.eof())
            {
                hex_dump::hex_dump(log, stream, options.hex_dump_memory_size(size), indent + 2);
                log << L'\n';
            }
        }
    }

    void print_lfh_entry_single_line(std::wostream& log, streamsize const hex_length, heap::heap_lfh_entry const& entry, size_t const indent)
    {
        using namespace size_units::base_16;
        log << std::format(L"{0:{1}}{2} LFH Entry: {3} BlockSize({4}){5} UserPtr({6}) ReqSize({7})\n", L' ', indent
            , entry.allocated() ? L"Busy"sv : L"Free"sv
            , stream_hex_dump::to_hex(entry.heap_lfh_entry_address(), hex_length)
            , to_wstring(entry.block_size())
            , entry.has_unused_bytes() ? std::format(L" UnusedBytes({})", to_wstring(entry.unused_bytes())) : L""s
            , stream_hex_dump::to_hex(entry.user_address(), hex_length)
            , to_wstring(entry.user_requested_size()));
    }

    void print_lfh_entry_debug(std::wostream& log, streamsize const hex_length, heap::heap_lfh_entry const& entry, size_t const indent)
    {
        std::wstring const indent_str(indent, L' ');
        using namespace size_units::base_16;
        log << std::format(L"{0}LFH Entry: {1}\n", indent_str, stream_hex_dump::to_hex(entry.heap_lfh_entry_address(), hex_length));
        log << std::format(L"{0}  Block Size: {1} ({2})\n", indent_str, to_wstring(entry.block_size()), stream_hex_dump::to_hex(entry.block_size()));
        log << std::format(L"{0}  Allocated: {1}\n", indent_str, entry.allocated());
        log << std::format(L"{0}  Has Unused Bytes: {1}\n", indent_str, entry.has_unused_bytes());
        if(entry.has_unused_bytes())
        {
            log << std::format(L"{0}  Unused Bytes: {1} ({2})\n", indent_str, to_wstring(entry.unused_bytes()), stream_hex_dump::to_hex(entry.unused_bytes()));
        }
        log << std::format(L"{0}  User Address: {1}\n", indent_str, stream_hex_dump::to_hex(entry.user_address(), hex_length));
        log << std::format(L"{0}  Requested User Size: {1} ({2})\n", indent_str, to_wstring(entry.user_requested_size()), stream_hex_dump::to_hex(entry.user_requested_size()));
        log << std::format(L"{0}  UST Address: {1}\n", indent_str, stream_hex_dump::to_hex(entry.ust_address(), hex_length));
    }

    void print_lfh_entry(std::wostream& log, streamsize const hex_length, heap::heap_lfh_entry const& entry, dump_file_options const& options, size_t const indent)
    {
        if(options.debug_heap_data())
        {
            print_lfh_entry_debug(log, hex_length, entry, indent);
        }
        else
        {
            print_lfh_entry_single_line(log, hex_length, entry, indent);
        }

        if(options.display_symbols() && entry.allocated() && !entry.allocation_stack_trace().empty())
        {
            log << std::format(L"{0:{1}}Allocation Stack Trace:\n", L' ', indent + 2);
            hex_dump_stack(log, entry.heap().walker(), entry.allocation_stack_trace(), entry.heap().peb().is_x86_target(), indent + 4);
            log << L'\n';
        }

        if(options.hex_dump_memory_data() && entry.allocated() && entry.user_address() != 0)
        {
            uint64_t const size = entry.user_requested_size().count();
            if(auto stream = entry.walker().get_process_memory_stream(entry.user_address(), size); !stream.eof())
            {
                hex_dump::hex_dump(log, stream, options.hex_dump_memory_size(size), indent + 2);
                log << L'\n';
            }
        }
    }

    void print_lfh_subsegment(std::wostream& log, streamsize const hex_length, heap::heap_lfh_subsegment const& subsegment, dump_file_options const& options, size_t const indent)
    {
        std::wstring const indent_str(indent, L' ');
        using namespace size_units::base_16;
        log << std::format(L"{0}LFH Subsegment: {1}\n", indent_str, stream_hex_dump::to_hex(subsegment.heap_lfh_subsegment_address(), hex_length));
        log << std::format(L"{0}  Free Count: {1}\n", indent_str, locale_formatting::to_wstring(subsegment.free_count()));
        log << std::format(L"{0}  Block Count: {1}\n", indent_str, locale_formatting::to_wstring(subsegment.block_count()));
        log << std::format(L"{0}  Block Size: {1} ({2})\n", indent_str, to_wstring(subsegment.block_size()), stream_hex_dump::to_hex(subsegment.block_size()));
        auto const location{subsegment.location()};
        log << std::format(L"{0}  Location: {1} ({2})\n", indent_str, locale_formatting::to_wstring(static_cast<uint16_t>(location)), dump_page_range_flags_to_string(location));
        if(options.debug_heap_data())
        {
            log << std::format(L"{0}  Witheld Block Count: {1}\n", indent_str, locale_formatting::to_wstring(subsegment.witheld_block_count()));
            log << std::format(L"{0}  Commit Unit Count: {1}\n", indent_str, locale_formatting::to_wstring(subsegment.commit_unit_count()));
            log << std::format(L"{0}  Commit Unit Shift: {1}\n", indent_str, locale_formatting::to_wstring(subsegment.commit_unit_shift()));
        }

        for(auto const& entry : subsegment.entries())
        {
            print_lfh_entry(log, hex_length, entry, options, indent + 2);
        }
    }

    void print_lfh_affinity_slot(std::wostream& log, streamsize const hex_length, heap::heap_lfh_affinity_slot const& affinity_slot, dump_file_options const& options, size_t const indent)
    {
        std::wstring const indent_str(indent, L' ');
        using namespace size_units::base_16;
        log << std::format(L"{0}LFH Affinity Slot: {1}\n", indent_str, stream_hex_dump::to_hex(affinity_slot.heap_lfh_affinity_slot_address(), hex_length));
        log << std::format(L"{0}  Slot Index: {1}\n", indent_str, locale_formatting::to_wstring(affinity_slot.slot_index()));
        log << std::format(L"{0}  Bucket Index: {1}\n", indent_str, locale_formatting::to_wstring(affinity_slot.bucket_index()));
        log << std::format(L"{0}  Available Subsegment Count: {1}\n", indent_str, locale_formatting::to_wstring(affinity_slot.available_subsegment_count()));

        for(auto const& subsegment : affinity_slot.subsegments())
        {
            print_lfh_subsegment(log, hex_length, subsegment, options, indent + 2);
        }
    }

    void print_lfh_bucket_single_line(std::wostream& log, streamsize const hex_length, heap::heap_lfh_bucket const& lfh_bucket, size_t const indent)
    {
        using namespace size_units::base_16;
        log << std::format(L"{0:{1}}LFH Bucket: {2} @ {3} {4} Granularity({5}) MaxAllocationSize({6})", L' ', indent
            , stream_hex_dump::to_hex(lfh_bucket.heap_lfh_bucket_address(), hex_length)
            , locale_formatting::to_wstring(lfh_bucket.bucket_index())
            , lfh_bucket.is_enabled() ? L"Enabled"sv : L"Disabled"sv
            , to_wstring(lfh_bucket.bucket_granularity())
            , to_wstring(lfh_bucket.max_allocation_size()));

        log << (lfh_bucket.is_enabled() ? std::format(L" TotalBlockCount({0}) TotalSubsegmentCount({1}) Shift({2})"
                                                      , locale_formatting::to_wstring(lfh_bucket.total_block_count())
                                                      , locale_formatting::to_wstring(lfh_bucket.total_subsegment_count())
                                                      , locale_formatting::to_wstring(lfh_bucket.shift()))
            : std::format(L" UsageCount({})", locale_formatting::to_wstring(lfh_bucket.usage_count())));
        log << L'\n';
    }

    void print_lfh_bucket_debug(std::wostream& log, streamsize const hex_length, heap::heap_lfh_bucket const& lfh_bucket, size_t const indent)
    {
        std::wstring const indent_str(indent, L' ');
        using namespace size_units::base_16;
        log << std::format(L"{0}LFH Bucket: {1}\n", indent_str, stream_hex_dump::to_hex(lfh_bucket.heap_lfh_bucket_address(), hex_length));
        log << std::format(L"{0}  Bucket Index: {1}\n", indent_str, locale_formatting::to_wstring(lfh_bucket.bucket_index()));
        log << std::format(L"{0}  Bucket Enabled: {1}\n", indent_str, lfh_bucket.is_enabled());
        log << std::format(L"{0}  Granularity: {1} ({2})\n", indent_str, to_wstring(lfh_bucket.bucket_granularity()), stream_hex_dump::to_hex(lfh_bucket.bucket_granularity()));
        log << std::format(L"{0}  Max Allocation Size: {1} ({2})\n", indent_str, to_wstring(lfh_bucket.max_allocation_size()), stream_hex_dump::to_hex(lfh_bucket.max_allocation_size()));

        if(lfh_bucket.is_enabled())
        {
            log << std::format(L"{0}  Slot Count: {1}\n", indent_str, lfh_bucket.slot_count());
            log << std::format(L"{0}  Total Block Count: {1}\n", indent_str, locale_formatting::to_wstring(lfh_bucket.total_block_count()));
            log << std::format(L"{0}  Total Subsegment Count: {1}\n", indent_str, locale_formatting::to_wstring(lfh_bucket.total_subsegment_count()));
            log << std::format(L"{0}  Shift: {1}\n", indent_str, locale_formatting::to_wstring(lfh_bucket.shift()));
            log << std::format(L"{0}  Reciprocal Block Size: {1} ({2})\n", indent_str, to_wstring(lfh_bucket.reciprocal_block_size()), stream_hex_dump::to_hex(lfh_bucket.reciprocal_block_size()));
            log << std::format(L"{}  Processor Affinity Mapping:\n", indent_str);
            auto const processor_affinity_mapping = lfh_bucket.processor_affinity_mapping();
            for(size_t index = 0; index < processor_affinity_mapping.size(); ++index)
            {
                log << std::format(L"{0}    {{ CPU:{1} -> Slot:{2} }}\n", indent_str, locale_formatting::to_wstring(index), locale_formatting::to_wstring(processor_affinity_mapping[index]));
            }
        }
        else
        {
            log << std::format(L"{0}  Usage Count: {1}\n", indent_str, locale_formatting::to_wstring(lfh_bucket.usage_count()));
        }
    }

    void print_lfh_bucket(std::wostream& log, streamsize const hex_length, heap::heap_lfh_bucket const& lfh_bucket, dump_file_options const& options, size_t const indent)
    {
        if(options.debug_heap_data())
        {
            print_lfh_bucket_debug(log, hex_length, lfh_bucket, indent);
        }
        else
        {
            print_lfh_bucket_single_line(log, hex_length, lfh_bucket, indent);
        }

        if(lfh_bucket.is_enabled())
        {
            for(auto const& affinity_slot : lfh_bucket.affinity_slots())
            {
                print_lfh_affinity_slot(log, hex_length, affinity_slot, options, indent + 2);
            }
        }
    }

    void print_lfh_context_heap(std::wostream& log, streamsize const hex_length, const heap::heap_lfh_context& lfh_context, dump_file_options const& options, size_t const indent)
    {
        std::wstring const indent_str(indent, L' ');
        using namespace size_units::base_16;
        log << std::format(L"{0}LFH Context: {1}\n", indent_str, stream_hex_dump::to_hex(lfh_context.heap_lfh_context_address(), hex_length));
        log << std::format(L"{0}  Max Affinity: {1}\n", indent_str, locale_formatting::to_wstring(lfh_context.max_affinity()));
        log << std::format(L"{0}  Max Block Size: {1} ({2})\n", indent_str, to_wstring(lfh_context.max_block_size()), stream_hex_dump::to_hex(lfh_context.max_block_size()));
        log << std::format(L"{0}  With Old Page Crossing Blocks: {1}\n", indent_str, lfh_context.with_old_page_crossing_blocks());
        log << std::format(L"{0}  Disable Randomization: {1}\n", indent_str, lfh_context.disable_randomization());

        for(auto const& bucket : lfh_context.active_buckets())
        {
            print_lfh_bucket(log, hex_length, bucket, options, indent + 2);
        }
    }

    void print_segment_heap(std::wostream& log, std::streamsize const hex_length, std::wstring_view const& process_heap_marker, heap::segment_heap const& segment_heap, dump_file_options const& options, size_t const indent)
    {
        std::wstring const indent_str(indent, L' ');
        using namespace size_units::base_16;
        log << std::format(L"{0}Segment Heap: {1} {2}\n", indent_str, stream_hex_dump::to_hex(segment_heap.segment_heap_address(), hex_length), process_heap_marker);
        log << std::format(L"{0}  Global Flags: {1}\n", indent_str, stream_hex_dump::to_hex_full(segment_heap.global_flags()));
        log << std::format(L"{0}  Total Reserved: {1} ({2})\n", indent_str, to_wstring(segment_heap.reserved()), stream_hex_dump::to_hex(segment_heap.reserved()));
        log << std::format(L"{0}  Total Committed: {1} ({2})\n", indent_str, to_wstring(segment_heap.committed()), stream_hex_dump::to_hex(segment_heap.committed()));
        log << std::format(L"{0}  Total Uncommitted: {1} ({2})\n", indent_str, to_wstring(segment_heap.uncommitted()), stream_hex_dump::to_hex(segment_heap.uncommitted()));
        log << std::format(L"{0}  Large Reserved: {1} ({2})\n", indent_str, to_wstring(segment_heap.large_reserved()), stream_hex_dump::to_hex(segment_heap.large_reserved()));
        log << std::format(L"{0}  Large Committed: {1} ({2})\n", indent_str, to_wstring(segment_heap.large_committed()), stream_hex_dump::to_hex(segment_heap.large_committed()));
        log << std::format(L"{0}  Large Uncommitted: {1} ({2})\n", indent_str, to_wstring(segment_heap.large_uncommitted()), stream_hex_dump::to_hex(segment_heap.large_uncommitted()));
        log << std::format(L"{0}  Total Reserved Pages: {1}\n", indent_str, locale_formatting::to_wstring(segment_heap.total_reserved_pages()));
        log << std::format(L"{0}  Total Committed Pages: {1}\n", indent_str, locale_formatting::to_wstring(segment_heap.total_committed_pages()));
        log << std::format(L"{0}  Free Committed Pages: {1}\n", indent_str, locale_formatting::to_wstring(segment_heap.free_committed_pages()));
        log << std::format(L"{0}  LFH Free Committed Pages: {1}\n", indent_str, locale_formatting::to_wstring(segment_heap.lfh_free_committed_pages()));
        log << std::format(L"{0}  Large Reserved Pages: {1}\n", indent_str, locale_formatting::to_wstring(segment_heap.large_reserved_pages()));
        log << std::format(L"{0}  Large Committed Pages: {1}\n", indent_str, locale_formatting::to_wstring(segment_heap.large_committed_pages()));
        log << std::format(L"{0}  Heap Key: {1}\n", indent_str, stream_hex_dump::to_hex(segment_heap.heap_key(), hex_length));
        log << L'\n';

        size_t index{0};
        for(auto const& segment_context : segment_heap.segment_contexts())
        {
            print_segment_context_heap(log, hex_length, index, segment_context, options, indent + 2);
            ++index;
        }

        print_vs_context_heap(log, hex_length, segment_heap.vs_context(), options, indent + 2);
        print_lfh_context_heap(log, hex_length, segment_heap.lfh_context(), options, indent + 2);

        for(auto const& entry : segment_heap.large_entries())
        {
            print_heap_large_entry(log, hex_length, entry, options, indent + 2);
        }
    }

    std::unique_ptr<heap::process_heaps> setup_base_diff_dump_heaps(std::unique_ptr<mini_dump> const& base_diff_dump, cache_manager& base_cache, heap::process_heaps& heaps, dbg_help::symbol_engine& symbol_engine, dump_file_options const& options)
    {
        if(!base_diff_dump)
        {
            return {};
        }

        auto base_diff_heaps = std::make_unique<heap::process_heaps>(*base_diff_dump, base_cache, symbol_engine, options.system_module_list(), options.statistic_view_options());
        heaps.set_base_diff_filter(*base_diff_heaps);
        return base_diff_heaps;
    }

    struct diff_crt_data
    {
        diff_crt_data(mini_dump const& mini_dump, dbg_help::symbol_engine& symbol_engine)
        : peb{mini_dump, cache, symbol_engine}
        , crtheap{cache, peb}
        {
        }

        cache_manager cache;
        process::process_environment_block const peb;
        heap::crt_heap crtheap;
    };

    std::unique_ptr<diff_crt_data> setup_base_diff_dump_crtheap(std::unique_ptr<mini_dump> const& base_diff_dump, heap::crt_heap& crtheap, dbg_help::symbol_engine& symbol_engine)
    {
        if(!base_diff_dump)
        {
            return {};
        }

        auto base_diff_crtheap = std::make_unique<diff_crt_data>(*base_diff_dump, symbol_engine);
        crtheap.set_base_diff_filter(base_diff_crtheap->crtheap);
        return base_diff_crtheap;
    }
}

void dump_mini_dump_heap(std::wostream& log, mini_dump const& mini_dump, cache_manager& cache, dump_file_options const& options, dbg_help::symbol_engine& symbol_engine)
{
    process::process_environment_block const peb{mini_dump, cache, symbol_engine};
    auto const hex_length = peb.machine_hex_printable_length();

    log << L"Heaps:\n";
    log << std::format(L"{1:<{0}} NT/Segment   Flags      {3:<{2}} {4:<{2}} {5:<{2}} {6:<{2}} {7:<{2}} {8:<8} {9:<4}\n"
        , hex_length + 3, L"Address"sv
        , units_max_width
        , L"Reserved"sv
        , L"Committed"sv
        , L"Uncommitted"sv
        , L"Virtual"sv
        , L"Free"sv
        , L"Segments"sv
        , L"UCRs"sv);
    for(uint32_t heap_index = 0; heap_index < peb.number_of_heaps(); ++heap_index)
    {
        auto const heap_address = peb.heap_address(heap_index);
        log << ' ' << stream_hex_dump::to_hex(heap_address, hex_length);

        if(auto const segment_signature = peb.segment_signature(heap_index); segment_signature == heap::SegmentSignatureNtHeap)
        {
            if(auto const nt_heap = peb.nt_heap(heap_index); nt_heap.has_value())
            {
                print_nt_heap_line(log, get_process_marker(nt_heap.value().is_process_heap(peb.process_heap())), nt_heap.value());
                print_nt_heap_segments_list(log, hex_length, nt_heap.value(), 2);
                print_nt_heap_lfh_segments_list(log, hex_length, nt_heap.value(), 2);
            }
        }
        else if(segment_signature == heap::SegmentSignatureSegmentHeap)
        {
            if(auto const segment_heap = peb.segment_heap(heap_index); segment_heap.has_value())
            {
                print_segment_heap_line(log, get_process_marker(peb.process_heap() == heap_address), segment_heap.value());
            }
        }
        else
        {
            log << std::format(L" Unknown Heap Type ({0})\n", stream_hex_dump::to_hex(segment_signature));
        }
    }

    log << L'\n';

    for(uint32_t heap_index = 0; heap_index < peb.number_of_heaps(); ++heap_index)
    {
        if(auto const nt_heap = peb.nt_heap(heap_index); nt_heap.has_value())
        {
            print_nt_heap(log, hex_length, get_process_marker(nt_heap.value().is_process_heap(peb.process_heap())), nt_heap.value(), 0);

            vector<LfhSubsegmentData> lfh_data;
            if(auto const lfh_heap = nt_heap.value().lfh_heap(); lfh_heap.has_value())
            {
                for(size_t segment_index = 0; auto const& segment : lfh_heap.value().lfh_segments())
                {
                    auto segment_data = std::make_shared<LfhSegmentData>(segment_index, segment);

                    for(auto const& subsegment : segment.subsegments())
                    {
                        lfh_data.emplace_back(segment_data, subsegment);
                    }
                    ++segment_index;
                }
            }

            for(size_t segment_index = 0; auto const& segment : nt_heap.value().segments())
            {
                print_heap_segment(log, hex_length, segment_index, segment, lfh_data, options, 4);
                ++segment_index;
            }

            auto printed_lfh_output{false};
            for(auto& data : lfh_data)
            {
                if(!data.printed_subsegment)
                {
                    if(!printed_lfh_output)
                    {
                        log << "WARNING: LFH segment(s) not found in any heap allocations!\n";
                    }

                    for(bool found = false; auto const& segment : nt_heap.value().segments())
                    {
                        for(auto const& entry : segment.entries())
                        {
                            if(is_lfh_subsegment_in_entry(entry, data, true))
                            {
                                found = true;
                                log << std::format(L"ERROR: LFH Segment found in heap invalid entry: {0}\n", stream_hex_dump::to_hex(entry.address(), hex_length));
                                break;
                            }
                        }

                        if(found)
                        {
                            break;
                        }
                    }

                    print_lfh_heap_segment(log, hex_length, data, options, 4);
                    printed_lfh_output = true;
                }
            }

            if(printed_lfh_output)
            {
                log << L'\n';
            }

            print_nt_heap_uncommitted_ranges(log, nt_heap.value(), 2);

            print_nt_heap_virtual_allocated_blocks(log, hex_length, nt_heap.value(), options, 2);

            print_nt_heap_free_list(log, hex_length, nt_heap.value(), options, 2);
        }
        else if(auto const segment_heap = peb.segment_heap(heap_index); segment_heap.has_value())
        {
            print_segment_heap(log, hex_length, get_process_marker(peb.process_heap() == segment_heap.value().segment_heap_address()), segment_heap.value(), options, 0);
        }
    }

    for(auto const& heap : heap::dph_heap::dph_heaps(cache, peb))
    {
        print_debug_page_heap(log, hex_length, heap, options, 2);
    }

    log << L"\nVirtual Allocation Memory Ranges:\n";
    log << std::format(L" {1:<{0}} {2:<{0}} {3:<{0}}\n", hex_length + 2, L"Start"sv, L"End"sv, L"Size"sv);
    using namespace size_units::base_16;
    for(auto const& range : peb.walker().memory_ranges())
    {
        log << std::format(L" {0} {1} {2} ({3})\n", stream_hex_dump::to_hex(range.start_range, hex_length), stream_hex_dump::to_hex(range.end_range, hex_length), stream_hex_dump::to_hex(range.end_range - range.start_range, hex_length), to_wstring(bytes{range.end_range - range.start_range}));
    }

    log << L'\n';
}

void print_process_entry(std::wostream& log, heap::process_heap_entry const& entry, process::process_environment_block const& peb, std::streamsize const hex_length, dump_file_options const& options, size_t const indent)
{
    using namespace size_units::base_16;
    log << std::format(L"{0:{1}}{2} size({3}) overhead({4})", L' ', indent, stream_hex_dump::to_hex(entry.user_address(), hex_length), to_wstring(entry.user_requested_size()), to_wstring(entry.overhead_size()));
    if(!entry.filename().empty())
    {
        log << std::format(L" {0}:{1}", entry.filename(), locale_formatting::to_wstring(entry.line_number()));
    }
    if(entry.has_request_number())
    {
        log << std::format(L" #{}", locale_formatting::to_wstring(entry.request_number()));
    }
    if(!entry.allocation_stack_trace().empty())
    {
        log << L" (has stack)";
    }
    log << L'\n';
\
    if(options.display_symbols() && !entry.allocation_stack_trace().empty())
    {
        log << std::format(L"{0:{1}}Allocation Stack Trace:\n", L' ', indent + 2);
        hex_dump_stack(log, peb.walker(), entry.allocation_stack_trace(), peb.is_x86_target(), indent + 4);
        log << L'\n';
    }

    if(options.hex_dump_memory_data())
    {
        if(auto stream = entry.all_user_data(); !stream.eof())
        {
            hex_dump::hex_dump(log, stream, options.hex_dump_memory_size(entry.user_requested_size().count()), indent + 4);
            log << L'\n';
        }
    }
}

void dump_mini_dump_heap_entries(std::wostream& log, mini_dump const& mini_dump, cache_manager& cache, std::unique_ptr<dlg_help_utils::mini_dump> const& base_diff_dump, dump_file_options const& options, dbg_help::symbol_engine& symbol_engine)
{
    heap::process_heaps heaps{mini_dump, cache, symbol_engine, options.system_module_list(), options.statistic_view_options()};
    cache_manager base_cache;
    [[maybe_unused]] auto const base_diff_heaps = setup_base_diff_dump_heaps(base_diff_dump, base_cache, heaps, symbol_engine, options);
    auto const hex_length = heaps.peb().machine_hex_printable_length();

    log << L"Heap Allocated Entries:\n";
    for(auto const& entry : heaps.entries())
    {
        print_process_entry(log, entry, heaps.peb(), hex_length, options, 2);
    }

    log << L"Heap Free Entries:\n";
    for(auto const& entry : heaps.free_entries())
    {
        print_process_entry(log, entry, heaps.peb(), hex_length, options, 2);
    }

    log << L'\n';
}

void dump_mini_dump_crtheap(std::wostream& log, mini_dump const& mini_dump, cache_manager& cache, std::unique_ptr<dlg_help_utils::mini_dump> const& base_diff_dump, dump_file_options const& options, dbg_help::symbol_engine& symbol_engine)
{
    process::process_environment_block const peb{mini_dump, cache, symbol_engine};
    auto const hex_length = peb.machine_hex_printable_length();

    heap::crt_heap heap{cache, peb};

    if(!heap.is_using_crt_heap())
    {
        log << L"Not using CRT Heap.\n";
        return;
    }

    [[maybe_unused]] auto const base_diff_crtheap = setup_base_diff_dump_crtheap(base_diff_dump, heap, symbol_engine);

    log << L"CRT Heap Entries:\n";
    std::wstring const indent_str(2, L' ');
    for (auto const& entry : heap.entries())
    {
        using namespace size_units::base_16;
        if(options.debug_heap_data())
        {
            log << std::format(L"{0}CRT Entry: {1}\n", indent_str, stream_hex_dump::to_hex(entry.entry_address(), hex_length));
            log << std::format(L"{0}  Block In Use: {1}\n", indent_str, entry.block_use());
            log << std::format(L"{0}  Filename: {1}\n", indent_str, entry.filename());
            log << std::format(L"{0}  Line Number: {1}\n", indent_str, locale_formatting::to_wstring(entry.line_number()));
            log << std::format(L"{0}  Request Number: {1}\n", indent_str, locale_formatting::to_wstring(entry.request_number()));
            log << std::format(L"{0}  Data Size: {1} ({2})\n", indent_str, to_wstring(entry.data_size()), stream_hex_dump::to_hex(entry.data_size()));
            log << std::format(L"{0}  User Address: {1}\n", indent_str, stream_hex_dump::to_hex(entry.user_address(), hex_length));
        }
        else
        {
            log << std::format(L"{0}CRT Entry: {1} {2} {3} UserPtr({4})", indent_str
                , stream_hex_dump::to_hex(entry.entry_address(), hex_length)
                , entry.block_use() ? L"in use"sv : L"free"sv
                , to_wstring(entry.data_size())
                , stream_hex_dump::to_hex(entry.user_address(), hex_length));
            if(!entry.filename().empty())
            {
                log << std::format(L" : {0}:{1}", entry.filename(), locale_formatting::to_wstring(entry.line_number()));
            }
            log << L'\n';
        }

        if(entry.block_use() && options.hex_dump_memory_data())
        {
            uint64_t const size = entry.data_size().count();
            if(auto stream = entry.walker().get_process_memory_stream(entry.user_address(), size); !stream.eof())
            {
                hex_dump::hex_dump(log, stream, options.hex_dump_memory_size(size), 4);
                log << L'\n';
            }
        }
    }
}

void dump_mini_dump_heap_statistics_view(std::wostream& log, process::process_environment_block const& peb, heap::process_heaps_statistic_view const& view_by_size_frequency, dump_file_options const& options, bool const is_x86_target, streamsize const hex_length)
{
    using namespace size_units::base_16;
    log << std::format(L"  {}:\n", heap::process_heaps_statistic_view::to_wstring(view_by_size_frequency.view()));
    auto const single_line_range_title_length = 2 + hex_length;

    if(view_by_size_frequency.is_range_single_value())
    {
        log << std::format(L" {0:<12} ", L"");
        log << std::format(L"{0:<{1}} ", L"", single_line_range_title_length);
    }
    else
    {
        log << std::format(L" {0:<12} {1:<12} ", L"range", L"range");
    }

    log << std::format(L"{:<12} ", L"allocated");
    log << std::format(L"{:<12} ", L"allocated");
    log << std::format(L"{:<12} ", L"allocated");
    log << std::format(L"{:<12} ", L"overhead");
    log << std::format(L"{:<12} ", L"free");
    log << std::format(L"{:<12} ", L"free");
    log << std::format(L"{:<7} ", L"count");
    log << std::format(L"{:<7} ", L"size");
    log << L"\n";

    if(view_by_size_frequency.is_range_single_value())
    {
        log << std::format(L" {0:<12} ", L"size");
        log << std::format(L"{0:<{1}} ", L"size hex", single_line_range_title_length);
    }
    else
    {
        log << std::format(L" {0:<12} {1:<12} ", L"start", L"size");
    }

    log << std::format(L"{:<12} ", L"count");
    log << std::format(L"{:<12} ", L"total");
    log << std::format(L"{:<12} ", L"average");
    log << std::format(L"{:<12} ", L"total");
    log << std::format(L"{:<12} ", L"count");
    log << std::format(L"{:<12} ", L"total");
    log << std::format(L"{:<7} ", L"percent");
    log << std::format(L"{:<7} ", L"percent");
    log << L" application call site\n";

    if(view_by_size_frequency.is_range_single_value())
    {
        log << std::format(L"={0:=<12}=", L"");
        log << std::format(L"{0:=<{1}}=", L"", single_line_range_title_length);
    }
    else
    {
        log << std::format(L" {0:=<12}={1:=<12}=", L"", L"");
    }

    log << std::format(L"{:=<12}=", L"");
    log << std::format(L"{:=<12}=", L"");
    log << std::format(L"{:=<12}=", L"");
    log << std::format(L"{:=<12}=", L"");
    log << std::format(L"{:=<12}=", L"");
    log << std::format(L"{:=<12}=", L"");
    log << std::format(L"{:=<7}=", L"");
    log << std::format(L"{:=<7}=", L"");
    log << L"======================\n";

    for(auto const& bucket : view_by_size_frequency.buckets())
    {
        if(view_by_size_frequency.is_range_single_value())
        {
            log << std::format(L" {:12} ", to_wstring(bucket.start_range()));
            log << std::format(L"{0:{1}} ", stream_hex_dump::to_hex(bucket.start_range()), single_line_range_title_length);
        }
        else
        {
            log << std::format(L" {0:12} {1:12} ", to_wstring(bucket.start_range()), to_wstring(bucket.end_range() - bucket.start_range()));
        }
        log << std::format(L"{:12} ", locale_formatting::to_wstring(bucket.allocated_count()));
        log << std::format(L"{:12} ", to_wstring(bucket.allocated_total()));
        log << std::format(L"{:12} ", to_wstring(bucket.allocated_average()));
        log << std::format(L"{:12} ", to_wstring(bucket.overhead_total()));
        log << std::format(L"{:12} ", locale_formatting::to_wstring(bucket.free_count()));
        log << std::format(L"{:12} ", to_wstring(bucket.free_total()));
        log << std::format(L"{:<7.2f} ", bucket.range_count_percent());
        log << std::format(L"{:<7.2f} ", bucket.range_size_percent());
        if(bucket.common_allocation_callsite().has_value())
        {
            log << std::format(L" {0}", stream_stack_dump::hex_dump_stack_frame(bucket.common_allocation_callsite().value(), is_x86_target));
        }
        log << L'\n';

        if(options.display_symbols() && !bucket.allocation_stack_trace().empty())
        {
            log << L"  Allocation Stack Trace:\n";
            hex_dump_stack(log, peb.walker(), bucket.allocation_stack_trace(), is_x86_target, 2);
            log << L'\n';
        }

        if(options.debug_heap_data())
        {
            log << L"  Entries:\n";
            for(auto const& entry : bucket.entries())
            {
                print_process_entry(log, entry, peb, hex_length, options, 4);
            }

            log << L"  Free Entries:\n";
            for(auto const& entry : bucket.free_entries())
            {
                print_process_entry(log, entry, peb, hex_length, options, 4);
            }
        }
    }

    if(view_by_size_frequency.is_range_single_value())
    {
        log << std::format(L"={0:=<12}=", L"");
        log << std::format(L"{0:=<{1}}=", L"", single_line_range_title_length);
    }
    else
    {
        log << std::format(L" {0:=<12}={1:=<12}=", L"", L"");
    }

    log << std::format(L"{:=<12}=", L"");
    log << std::format(L"{:=<12}=", L"");
    log << std::format(L"{:=<12}=", L"");
    log << std::format(L"{:=<12}=", L"");
    log << std::format(L"{:=<8}=", L"");
    log << std::format(L"{:=<12}=", L"");
    log << std::format(L"{:=<7}=", L"");
    log << std::format(L"{:=<7}=", L"");
    log << L"======================\n";

    if(view_by_size_frequency.is_range_single_value())
    {
        log << std::format(L" {:12} ", L"");
        log << std::format(L"{0:>{1}} ", L"Total:", single_line_range_title_length);
    }
    else
    {
        log << std::format(L" {0:12} {1:>12} ", L"", L"Total:");
    }
    log << std::format(L"{:12} ", locale_formatting::to_wstring(view_by_size_frequency.allocated_count()));
    log << std::format(L"{:12} ", to_wstring(view_by_size_frequency.allocated_total()));
    log << std::format(L"{:12} ", to_wstring(view_by_size_frequency.allocated_average()));
    log << std::format(L"{:12} ", to_wstring(view_by_size_frequency.overhead_total()));
    log << std::format(L"{:12} ", locale_formatting::to_wstring(view_by_size_frequency.free_count()));
    log << std::format(L"{:12} ", to_wstring(view_by_size_frequency.free_total()));
    log << L'\n';
}

void dump_mini_dump_heap_statistics(std::wostream& log, mini_dump const& mini_dump, cache_manager& cache, std::unique_ptr<dlg_help_utils::mini_dump> const& base_diff_dump, dump_file_options const& options, dbg_help::symbol_engine& symbol_engine)
{
    heap::process_heaps heaps{mini_dump, cache, symbol_engine, options.system_module_list(), options.statistic_view_options()};
    cache_manager base_cache;
    [[maybe_unused]] auto const base_diff_heaps = setup_base_diff_dump_heaps(base_diff_dump, base_cache, heaps, symbol_engine, options);

    auto const loading_heap_statistics = L"Loading heap statistics..."s;
    std::wstring const move_back(loading_heap_statistics.size(), L'\b');
    std::wstring const clear(loading_heap_statistics.size(), L' ');
    std::wcerr << loading_heap_statistics;

    auto const hex_length = heaps.peb().machine_hex_printable_length();
    auto const is_x86_target = heaps.peb().is_x86_target();
    auto const statistics = heaps.statistics();

    std::wcerr << move_back;
    std::wcerr << clear;
    std::wcerr << move_back;

    log << L"Heap Statistics:\n";
    if(options.display_heap_statistic_view(heap_statistics_view::by_size_frequency_view))
    {
        dump_mini_dump_heap_statistics_view(log, heaps.peb(), statistics.view_by_size_frequency(), options, is_x86_target, hex_length);
        log << L'\n';
    }
    if(options.display_heap_statistic_view(heap_statistics_view::by_size_ranges_frequency_view))
    {
        dump_mini_dump_heap_statistics_view(log, heaps.peb(), statistics.view_by_size_ranges_frequency(), options, is_x86_target, hex_length);
        log << L'\n';
    }
    if(options.display_heap_statistic_view(heap_statistics_view::by_stacktrace_frequency_view))
    {
        dump_mini_dump_heap_statistics_view(log, heaps.peb(), statistics.view_by_stacktrace_frequency(), options, is_x86_target, hex_length);
        log << L'\n';
    }
    if(options.display_heap_statistic_view(heap_statistics_view::by_application_callsite_frequency_view))
    {
        dump_mini_dump_heap_statistics_view(log, heaps.peb(), statistics.view_by_application_callsite_frequency(), options, is_x86_target, hex_length);
        log << L'\n';
    }
}

