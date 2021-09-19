#include "dump_mini_dump_heap.h"

#include <iostream>
#include <format>

#include "dump_file_options.h"
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
#include "DbgHelpUtils/mini_dump_stack_walk.h"
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

using namespace std;
using namespace dlg_help_utils;

constexpr auto units_max_width = 16;

namespace
{
    struct LfhSegmentData
    {
        LfhSegmentData(size_t const segment_index, heap::lfh_segment segment)
            : segment_index{ segment_index }
            , segment{ std::move(segment) }
        {
        }

        size_t segment_index;
        heap::lfh_segment segment;
    };

    struct LfhSubsegmentData
    {
        LfhSubsegmentData(std::shared_ptr<LfhSegmentData> segment, heap::heap_subsegment subsegment)
            : segment{ std::move(segment) }
            , subsegment{ std::move(subsegment) }
        {
        }

        std::shared_ptr<LfhSegmentData> segment;
        heap::heap_subsegment subsegment;
        bool printed_subsegment{ false };
    };


    void print_nt_heap_line(char const* process_heap_marker, heap::nt_heap const& nt_heap)
    {
        wcout << " NT Heap     ";
        wcout << ' ' << stream_hex_dump::to_hex_full(nt_heap.flags());
        wcout << ' ';
        using namespace size_units::base_16;
        wcout << std::left << std::setw(units_max_width) << nt_heap.reserved();
        wcout << ' ';
        wcout << std::left << std::setw(units_max_width) << nt_heap.committed();
        wcout << ' ';
        wcout << std::left << std::setw(units_max_width) << nt_heap.uncommitted();
        wcout << ' ';
        wcout << std::left << std::setw(units_max_width) << nt_heap.virtual_blocks();
        wcout << ' ';
        wcout << std::left << std::setw(units_max_width) << nt_heap.total_free_size();
        wcout << ' ';
        wcout << std::left << std::setw(8) << nt_heap.total_segments();
        wcout << ' ';
        wcout << std::left << std::setw(4) << nt_heap.total_ucrs();
        if (nt_heap.is_low_fragment_heap_enabled())
        {
            wcout << " (LFH)";
        }
        if(nt_heap.debug_page_heap().has_value())
        {
            wcout << " (DPH)";
        }
        wcout << process_heap_marker << '\n';
    }

    void print_nt_heap_segment_line(std::streamsize const hex_length, size_t const segment_index, heap::heap_segment const& segment, std::wstring const& indent_str)
    {
        auto const first_entry = segment.first_entry();
        auto const last_entry = segment.last_entry();
        using namespace size_units::base_16;
        auto const total_size_bytes = bytes{ last_entry - first_entry };
        wcout << indent_str << "Segment " << segment_index << ' ' << stream_hex_dump::to_hex(first_entry, hex_length) << '-' << stream_hex_dump::to_hex(last_entry, hex_length) << ' ' << total_size_bytes << '\n';
    }

    void print_nt_heap_segments_list(std::streamsize const hex_length, heap::nt_heap const& nt_heap, size_t const indent)
    {
        std::wstring const indent_str(indent, L' ');
        for (size_t segment_index = 0; auto const& segment : nt_heap.segments())
        {
            print_nt_heap_segment_line(hex_length, segment_index, segment, indent_str);
            ++segment_index;
        }
    }

    void print_lfh_heap_segment_line(std::streamsize const hex_length, size_t const segment_index, heap::lfh_segment const& segment, std::wstring const& indent_str)
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

        wcout << indent_str << "LFH Segment " << segment_index << ' ' << stream_hex_dump::to_hex(segment.address(), hex_length) << " - subsegments " << segment.subsegments_count() << " - total " << segment_total << '\n';
    }

    void print_nt_heap_lfh_segments_list(std::streamsize const hex_length, heap::nt_heap const& nt_heap, size_t const indent)
    {
        if (auto const lfh_heap = nt_heap.lfh_heap(); lfh_heap.has_value())
        {
            std::wstring const indent_str(indent, L' ');
            for (size_t segment_index = 0; auto const& segment : lfh_heap.value().lfh_segments())
            {
                print_lfh_heap_segment_line(hex_length, segment_index, segment, indent_str);
                ++segment_index;
            }
        }
    }

    void print_nt_heap(std::streamsize const hex_length, char const* process_heap_marker, heap::nt_heap const& nt_heap, size_t const indent)
    {
        std::wstring const indent_str(indent, L' ');
        wcout << indent_str << "NT Heap: " << stream_hex_dump::to_hex(nt_heap.nt_heap_address(), hex_length) << ' ' << process_heap_marker << '\n';
        wcout << indent_str << "  Flags: " << stream_hex_dump::to_hex_full(nt_heap.flags()) << '\n';
        using namespace size_units::base_16;
        wcout << indent_str << "  Total Reserved: " << nt_heap.reserved() << " (" << stream_hex_dump::to_hex(nt_heap.reserved().count()) << ")" << '\n';
        wcout << indent_str << "  Total Committed: " << nt_heap.committed() << " (" << stream_hex_dump::to_hex(nt_heap.committed().count()) << ")" << '\n';
        wcout << indent_str << "  Total Uncommitted: " << nt_heap.uncommitted() << " (" << stream_hex_dump::to_hex(nt_heap.uncommitted().count()) << ")" << '\n';
        wcout << indent_str << "  Total Virtual Blocks: " << nt_heap.virtual_blocks() << " (" << stream_hex_dump::to_hex(nt_heap.virtual_blocks().count()) << ")" << '\n';
        wcout << indent_str << "  Total Free Size: " << nt_heap.total_free_size() << " (" << stream_hex_dump::to_hex(nt_heap.total_free_size().count()) << ")" << '\n';
        wcout << indent_str << "  Total Segments: " << nt_heap.total_segments() << '\n';
        wcout << indent_str << "  Total Uncommitted Ranges: " << nt_heap.total_ucrs() << '\n';
        wcout << indent_str << "  Granularity: " << bytes{ nt_heap.granularity() } << '\n';
        wcout << indent_str << "  Is Heap Entry Encoded: " << std::boolalpha << nt_heap.is_encoded() << '\n';
        wcout << indent_str << "  Allocator Back Trace Index : " << nt_heap.allocator_back_trace_index() << '\n';
        wcout << indent_str << "  Front End Heap Type : " << static_cast<uint16_t>(nt_heap.front_end_heap_type()) << (nt_heap.is_low_fragment_heap_enabled() ? " (LFH)" : "") << '\n';
        if (nt_heap.is_low_fragment_heap_enabled())
        {
            if (auto const lfh_heap = nt_heap.lfh_heap(); lfh_heap.has_value())
            {
                if (auto const lfh_key = lfh_heap.value().lfh_key(); lfh_key.has_value())
                {
                    wcout << indent_str << "  LFH Key : " << stream_hex_dump::to_hex(lfh_key.value()) << '\n';
                }
            }
        }
        wcout << '\n';
    }

    void print_entry_flags_break_down(heap::heap_entry const& entry)
    {
        auto const busy = (entry.flags() & heap::heap_entry::FlagBusy) == heap::heap_entry::FlagBusy;
        if (busy)
        {
            wcout << ", Busy";
        }
        else
        {
            wcout << ", Free";
        }
        if ((entry.flags() & heap::heap_entry::FlagVirtualAlloc) == heap::heap_entry::FlagVirtualAlloc)
        {
            wcout << (busy ? ", Internal" : ", Decommitted");
        }
        if ((entry.flags() & heap::heap_entry::FlagExtraPresent) == heap::heap_entry::FlagExtraPresent)
        {
            wcout << ", Tail ill";
        }
        if ((entry.flags() & heap::heap_entry::FlagFillPattern) == heap::heap_entry::FlagFillPattern)
        {
            wcout << ", Fill";
        }
        if ((entry.flags() & heap::heap_entry::FlagLastEntry) == heap::heap_entry::FlagLastEntry)
        {
            wcout << ", Last";
        }
        if ((entry.flags() & heap::heap_entry::FlagUserMask) != 0)
        {
            wcout << ", User Flag " << static_cast<uint16_t>(entry.flags() >> heap::heap_entry::FlagUserBitOffset);
        }

        if (entry.is_front_padded())
        {
            wcout << ", Front Padded";
        }

        if (!entry.is_valid())
        {
            wcout << ", Invalid Checksum";
        }
    }

    void print_dump_entry_single_line(std::streamsize const hex_length, heap::heap_entry const& entry, size_t const indent)
    {
        using namespace size_units::base_16;
        wcout << std::wstring(indent, L' ') << stream_hex_dump::to_hex(entry.address(), hex_length);
        wcout << " Size(" << entry.size() << ")";
        if (entry.is_uncommitted())
        {
            wcout << " - Uncommitted\n";
        }
        else if (entry.is_unknown())
        {
            wcout << " - Unknown Range\n";
        }
        else
        {
            if (entry.is_busy())
            {
                wcout << " ReqSize(" << entry.user_requested_size() << ")";
                wcout << " UnusedBytes(" << entry.unused_bytes() << ")";
                if (entry.end_unused_bytes() > bytes{ 0 })
                {
                    wcout << " EndUnusedBytes(" << entry.end_unused_bytes() << ")";
                }
                if (entry.user_address() != 0)
                {
                    wcout << " UsrPtr(" << stream_hex_dump::to_hex(entry.user_address(), hex_length) << ")";
                }
            }

            if (entry.is_lfh_entry())
            {
                wcout << (entry.is_lfh_busy() ? " Busy" : " Free");
            }
            else
            {
                wcout << " Flags(" << stream_hex_dump::to_hex(entry.flags()) << ")";
                print_entry_flags_break_down(entry);
            }

            wcout << '\n';
        }
    }

    void print_dump_entry_debug(std::streamsize const hex_length, heap::heap_entry const& entry, size_t const indent)
    {
        using namespace size_units::base_16;
        std::wstring const indent_str(indent, L' ');
        if (entry.is_uncommitted())
        {
            wcout << indent_str << "Uncommitted Entry: " << stream_hex_dump::to_hex(entry.address(), hex_length) << '\n';
            wcout << indent_str << "  Size: " << entry.size() <<  " (" << stream_hex_dump::to_hex(entry.size().count()) << ")" << '\n';
        }
        else if (entry.is_unknown())
        {
            wcout << indent_str << "Unknown Range Entry: " << stream_hex_dump::to_hex(entry.address(), hex_length) << '\n';
            wcout << indent_str << "  Size: " << entry.size() <<  " (" << stream_hex_dump::to_hex(entry.size().count()) << ")" << '\n';
        }
        else
        {
            wcout << indent_str << (entry.is_lfh_entry() ? "LFH " : "") << (entry.is_busy() ? "Busy" : "Free") << " Entry: " << stream_hex_dump::to_hex(entry.address(), hex_length) << '\n';
            wcout << indent_str << "  Size: " << entry.size() <<  " (" << stream_hex_dump::to_hex(entry.size().count()) << ")" << '\n';
            if (entry.is_busy())
            {
                wcout << indent_str << "  User Requested Size: " << entry.user_requested_size() << " (" << stream_hex_dump::to_hex(entry.user_requested_size().count()) << ")" << '\n';
                wcout << indent_str << "  Unused Bytes: " << entry.unused_bytes() << " (" << stream_hex_dump::to_hex(entry.unused_bytes().count()) << ")" << '\n';
                if (entry.end_unused_bytes() > bytes{ 0 })
                {
                    wcout << indent_str << "  End Unused Bytes: " << entry.end_unused_bytes() << " (" << stream_hex_dump::to_hex(entry.end_unused_bytes().count()) << ")" << '\n';
                }
                if (entry.user_address() != 0)
                {
                    wcout << indent_str << "  User Address: " << stream_hex_dump::to_hex(entry.user_address(), hex_length) << '\n';
                }
                if (entry.ust_address() != 0)
                {
                    wcout << indent_str << "  UST Address: " << stream_hex_dump::to_hex(entry.ust_address(), hex_length) << '\n';
                }
            }

            if (!entry.is_lfh_entry())
            {
                wcout << indent_str << "  Flags: " << stream_hex_dump::to_hex(entry.flags());
                print_entry_flags_break_down(entry);
                wcout << '\n';
            }
        }
    }

    void print_dump_entry(std::streamsize const hex_length, heap::heap_entry const& entry, bool const entry_contains_lfh_subsegments, dump_file_options const& options, size_t const indent)
    {
        if(options.debug_heap_data())
        {
            print_dump_entry_debug(hex_length, entry, indent);
        }
        else
        {
            print_dump_entry_single_line(hex_length, entry, indent);
        }

        if (entry.is_unknown())
        {
            if(options.hex_dump_memory_data())
            {
                const uint64_t size = entry.size().count();
                if(auto stream = entry.walker().get_process_memory_stream(entry.address(), size); !stream.eof())
                {
                    hex_dump::hex_dump(wcout, stream, options.hex_dump_memory_size(size), indent + 2);
                    wcout << L'\n';
                }
            }
        }
        else if(!entry.is_uncommitted())
        {
            if(options.display_symbols() && entry.is_busy() && !entry.allocation_stack_trace().empty())
            {
                wcout << std::wstring(indent + 2, L' ') << "Allocation Stack Trace:\n";
                hex_dump_stack(wcout, entry.walker(), entry.allocation_stack_trace(), entry.peb().is_x86_target(), indent + 2);
                wcout << '\n';
            }

            if(options.hex_dump_memory_data() && entry.is_busy() && entry.user_address() != 0 && ((entry.flags() & heap::heap_entry::FlagVirtualAlloc) != heap::heap_entry::FlagVirtualAlloc || !entry_contains_lfh_subsegments))
            {
                uint64_t const size = entry.user_requested_size().count();
                if(auto stream = entry.walker().get_process_memory_stream(entry.user_address(), size); !stream.eof())
                {
                    hex_dump::hex_dump(wcout, stream, options.hex_dump_memory_size(size), indent + 2);
                    wcout << L'\n';
                }
            }
        }
    }

    void print_lfh_heap_segment_single_line(std::streamsize const hex_length, LfhSubsegmentData& data, size_t const indent)
    {
        using namespace size_units::base_16;
        std::wstring const indent_str(indent, L' ');

        data.printed_subsegment = true;
        bytes const total{ data.subsegment.address() ? data.subsegment.block_count() * data.subsegment.block_stride() : 0 };
        wcout << indent_str << "LFH Segment " << data.segment->segment_index << " Subsegment " << stream_hex_dump::to_hex(data.subsegment.address(), hex_length) << ", block count " << data.subsegment.block_count() << ", block size " << data.subsegment.block_size() << ", entry data " << stream_hex_dump::to_hex(data.subsegment.entry_start_address(), hex_length) << ", total " << total << '\n';
    }

    void print_lfh_heap_segment_debug(std::streamsize const hex_length, LfhSubsegmentData& data, size_t const indent)
    {
        using namespace size_units::base_16;
        std::wstring const indent_str(indent, L' ');

        data.printed_subsegment = true;
        bytes const total{ data.subsegment.address() ? data.subsegment.block_count() * data.subsegment.block_stride() : 0 };
        wcout << indent_str << "LFH Segment: " << data.segment->segment_index << '\n';
        wcout << indent_str << "  Subsegment: " << stream_hex_dump::to_hex(data.subsegment.address(), hex_length) << '\n';
        wcout << indent_str << "  Block Count: " << data.subsegment.block_count() << '\n';
        wcout << indent_str << "  Block Size: " << data.subsegment.block_size() << " (" << stream_hex_dump::to_hex(data.subsegment.block_size().count()) << ")" << '\n';
        wcout << indent_str << "  Entry Data: " << stream_hex_dump::to_hex(data.subsegment.entry_start_address(), hex_length) << '\n';
        wcout << indent_str << "  Total: " << total << " (" << stream_hex_dump::to_hex(total.count()) << ")" << '\n';
    }

    void print_lfh_heap_segment(std::streamsize const hex_length, LfhSubsegmentData& data, dump_file_options const& options, size_t const indent)
    {
        if(options.debug_heap_data())
        {
            print_lfh_heap_segment_debug(hex_length, data, indent);
        }
        else
        {
            print_lfh_heap_segment_single_line(hex_length, data, indent);
        }

        for (auto const& entry : data.subsegment.entries())
        {
            print_dump_entry(hex_length, entry, false, options, indent + 2);
        }
    }

    bool is_lfh_subsegment_in_entry(heap::heap_entry const& entry, LfhSubsegmentData const& data)
    {
        return !data.printed_subsegment && data.subsegment.entry_start_address() > entry.address() && data.subsegment.entry_start_address() < entry.address() + entry.size().count();
    }

    void print_heap_segment(std::streamsize const hex_length, size_t const segment_index, heap::heap_segment const& segment, vector<LfhSubsegmentData>& lfh_data, dump_file_options const& options, size_t const indent)
    {
        using namespace size_units::base_16;
        std::wstring const indent_str(indent, L' ');
        print_nt_heap_segment_line(hex_length, segment_index, segment, indent_str);
        wcout << indent_str << "   Flags: " << stream_hex_dump::to_hex(segment.segment_flags()) << '\n';
        wcout << indent_str << "   Number of Pages: " << stream_hex_dump::to_hex(segment.number_of_pages()) << '\n';
        wcout << indent_str << "   Number of Uncommitted Pages: " << stream_hex_dump::to_hex(segment.number_of_uncommitted_pages()) << '\n';
        wcout << indent_str << "   Number of Uncommitted Ranges: " << stream_hex_dump::to_hex(segment.number_of_uncommitted_ranges()) << '\n';
        wcout << '\n';

        wcout << indent_str << "Uncommitted Ranges\n";
        for (auto const& uncommitted_range : segment.uncommitted_ranges())
        {
            wcout << indent_str << "    Range " << stream_hex_dump::to_hex(uncommitted_range.address()) << " for " << uncommitted_range.size() << '\n';
        }

        wcout << '\n' << indent_str << "Heap Entries\n";
        for (auto const& entry : segment.entries())
        {
            auto const entry_contains_lfh_subsegments = ranges::any_of(lfh_data, [&entry](LfhSubsegmentData const& data) { return is_lfh_subsegment_in_entry(entry, data); });
            print_dump_entry(hex_length, entry, entry_contains_lfh_subsegments, options, indent + 4);

            for (auto& data : lfh_data)
            {
                if (is_lfh_subsegment_in_entry(entry, data))
                {
                    print_lfh_heap_segment(hex_length, data, options, indent + 6);
                }
            }
        }
        wcout << '\n';
    }

    char const* get_process_marker(bool const is_process_heap)
    {
        return is_process_heap ? " (process heap)" : "";
    }

    void print_nt_heap_uncommitted_ranges(heap::nt_heap const& nt_heap, size_t const indent)
    {
        using namespace size_units::base_16;
        std::wstring const indent_str(indent, L' ');
        wcout << indent_str << "Heap Uncommitted ranges\n";
        for (auto const& uncommitted_range : nt_heap.uncommitted_ranges())
        {
            wcout << indent_str << "  Range " << stream_hex_dump::to_hex(uncommitted_range.address()) << " for " << uncommitted_range.size() << '\n';
        }
        wcout << '\n';
    }

    void print_nt_heap_virtual_allocated_blocks(std::streamsize const hex_length, heap::nt_heap const& nt_heap, dump_file_options const& options, size_t const indent)
    {
        using namespace size_units::base_16;
        std::wstring const indent_str(indent, L' ');
        wcout << indent_str << "Virtual Allocated Blocks\n";
        for (auto const& virtual_block : nt_heap.heap_virtual_blocks())
        {
            wcout << indent_str << "  Entry " << stream_hex_dump::to_hex(virtual_block.descriptor_address()) << " data " << stream_hex_dump::to_hex(virtual_block.address()) << " reversed " << virtual_block.reserved() << " committed " << virtual_block.committed() << '\n';
            for (auto const& entry : virtual_block.entries())
            {
                print_dump_entry(hex_length, entry, false, options, indent + 4);
            }
        }
        wcout << '\n';
    }

    void print_nt_heap_free_list(std::streamsize const hex_length, heap::nt_heap const& nt_heap, dump_file_options const& options, size_t const indent)
    {
        std::wstring const indent_str(indent, L' ');
        wcout << indent_str << "Free List Entries\n";
        for (auto const& free_entry : nt_heap.free_entries())
        {
            print_dump_entry(hex_length, free_entry, false, options, indent + 4);
        }
        wcout << '\n';
    }

    void print_debug_page_heap_entry_single_line(streamsize const hex_length, size_t const index, heap::dph_entry const& entry, size_t const indent)
    {
        using namespace size_units::base_16;
        std::wstring const indent_str(indent, L' ');
        wcout << indent_str << index << " @ " << stream_hex_dump::to_hex(entry.entry_address(), hex_length);
        wcout << (entry.is_allocated() ? " Busy" : " Free");
        wcout << " BlockPtr(" << stream_hex_dump::to_hex(entry.virtual_block_address(), hex_length) << ')';
        wcout << " BlockSize(" << entry.virtual_block_size() << ')';
        wcout << " UserPtr(" << stream_hex_dump::to_hex(entry.user_address(), hex_length) << ')';
        wcout << " ReqSize(" << entry.user_requested_size() << ')' << '\n';
    }

    void print_debug_page_heap_entry_debug(streamsize const hex_length, size_t const index, heap::dph_entry const& entry, size_t const indent)
    {
        using namespace size_units::base_16;
        std::wstring const indent_str(indent, L' ');
        wcout << indent_str << index << " @ " << stream_hex_dump::to_hex(entry.entry_address(), hex_length) << '\n';
        wcout << indent_str << "  Is Allocated: " << std::boolalpha << entry.is_allocated() << '\n';
        wcout << indent_str << "  Virtual Block: " << stream_hex_dump::to_hex(entry.virtual_block_address(), hex_length) << '\n';
        wcout << indent_str << "  Virtual Block Size: " << entry.virtual_block_size() << " (" << entry.virtual_block_size().count() << ")" << '\n';
        wcout << indent_str << "  User Allocation: " << stream_hex_dump::to_hex(entry.user_address(), hex_length) << '\n';
        wcout << indent_str << "  User Requested Size: " << entry.user_requested_size() << " (" << entry.user_requested_size().count() << ")" << '\n';
        wcout << indent_str << "  UST Address: " << stream_hex_dump::to_hex(entry.ust_address(), hex_length) << '\n';
    }

    void print_debug_page_heap_entry(streamsize const hex_length, size_t const index, heap::dph_entry const& entry, dump_file_options const& options, size_t const indent)
    {
        if(options.debug_heap_data())
        {
            print_debug_page_heap_entry_debug(hex_length, index, entry, indent);
        }
        else
        {
            print_debug_page_heap_entry_single_line(hex_length, index, entry, indent);
        }

        if(options.display_symbols() && entry.is_allocated() && !entry.allocation_stack_trace().empty())
        {
            std::wstring const indent_str(indent, L' ');
            wcout << indent_str << "  Allocation Stack Trace:\n";
            hex_dump_stack(wcout, entry.heap().walker(), entry.allocation_stack_trace(), entry.heap().peb().is_x86_target(), indent + 4);
            wcout << '\n';
        }

        if(options.hex_dump_memory_data() && entry.is_allocated() && entry.user_address() != 0)
        {
            uint64_t const size = entry.user_requested_size().count();
            if(auto stream = entry.walker().get_process_memory_stream(entry.user_address(), size); !stream.eof())
            {
                hex_dump::hex_dump(wcout, stream, options.hex_dump_memory_size(size), indent + 4);
                wcout << L'\n';
            }
        }
    }

    void print_debug_page_heap(streamsize const hex_length, heap::dph_heap const& heap, dump_file_options const& options, size_t const indent)
    {
        std::wstring const indent_str(indent, L' ');
        using namespace size_units::base_16;
        wcout << indent_str << "Debug Page Heap: " << stream_hex_dump::to_hex(heap.address(), hex_length) << '\n';
        wcout << indent_str << "  Flags: " << stream_hex_dump::to_hex(heap.flags()) << '\n';
        wcout << indent_str << "  Extra Flags: " << stream_hex_dump::to_hex(heap.extra_flags()) << '\n';
        wcout << indent_str << "  Seed: " << stream_hex_dump::to_hex(heap.seed()) << '\n';
        wcout << indent_str << "  Normal Heap: " << stream_hex_dump::to_hex(heap.normal_heap(), hex_length) << '\n';
        wcout << indent_str << "  Busy Allocations: " << heap.busy_allocations() << '\n';
        wcout << indent_str << "  Busy Allocations Committed Total: " << heap.busy_allocations_committed() << " (" << stream_hex_dump::to_hex(heap.busy_allocations_committed().count()) << ")" << '\n';
        wcout << indent_str << "  Virtual Ranges: " << heap.virtual_storage_ranges() << '\n';
        wcout << indent_str << "  Virtual Ranges Total: " << heap.virtual_storage_total() << " (" << stream_hex_dump::to_hex(heap.virtual_storage_total().count()) << ")" << '\n';
        wcout << indent_str << "  Free Allocations: " << heap.free_allocations() << '\n';
        wcout << indent_str << "  Free Allocations Committed Total: " << heap.free_allocations_committed() << " (" << stream_hex_dump::to_hex(heap.free_allocations_committed().count()) << ")" << '\n';
        wcout << indent_str << "  Busy Entries: (" << heap.busy_allocations() << ")\n";

        size_t index = 1;
        for(auto const& entry : heap.busy_entries())
        {
            print_debug_page_heap_entry(hex_length, index, entry, options, indent + 4);
            ++index;
        }

        wcout << indent_str << "  Virtual Ranges: (" << heap.virtual_storage_ranges() << ")\n";

        index = 1;
        for(auto const& entry : heap.virtual_ranges())
        {
            print_debug_page_heap_entry(hex_length, index, entry, options, indent + 4);
            ++index;
        }

        wcout << indent_str << "  Free Allocations: (" << heap.free_allocations() << ")\n";

        index = 1;
        for(auto const& entry : heap.free_entries())
        {
            print_debug_page_heap_entry(hex_length, index, entry, options, indent + 4);
            ++index;
        }
        wcout << '\n';
    }

    void print_segment_heap_line(char const* process_heap_marker, heap::segment_heap const& segment_heap)
    {
        wcout << " Segment Heap";
        wcout << ' ' << stream_hex_dump::to_hex_full(segment_heap.global_flags());
        wcout << ' ';
        using namespace size_units::base_16;
        wcout << std::left << std::setw(units_max_width) << segment_heap.reserved();
        wcout << ' ';
        wcout << std::left << std::setw(units_max_width) << segment_heap.committed();
        wcout << ' ';
        wcout << std::left << std::setw(units_max_width) << segment_heap.uncommitted();
        wcout << ' ';
        wcout << std::left << std::setw(units_max_width) << segment_heap.large_reserved();
        wcout << process_heap_marker << '\n';
    }

    void print_segment_heap_page_entry_single_line(streamsize const hex_length, heap::page_range_descriptor const& entry, size_t const indent)
    {
        using namespace size_units::base_16;
        std::wstring const indent_str(indent, L' ');
        wcout << indent_str << "Page Entry: " << entry.index() << " : " << stream_hex_dump::to_hex(entry.page_range_descriptor_address(), hex_length);
        wcout << " Extra(" << entry.extra_bytes() << ")";
        wcout << " BlockPtr(" << stream_hex_dump::to_hex(entry.block_address(), hex_length) << ')';
        wcout << " BlockSize(" << entry.block_size() << ')';
        wcout << " UserPtr(" << stream_hex_dump::to_hex(entry.user_address(), hex_length) << ')';
        wcout << " ReqSize(" << entry.user_requested_size() << ')';
        auto const range_flags = entry.range_flags();
        wcout << " Flags(" << stream_hex_dump::to_hex(static_cast<uint32_t>(range_flags)) << ") " << dump_page_range_to_string(range_flags);
        wcout << '\n';
    }

    void print_segment_heap_page_entry_debug(streamsize const hex_length, heap::page_range_descriptor const& entry, size_t const indent)
    {
        std::wstring const indent_str(indent, L' ');
        wcout << indent_str << "Page Entry: " << entry.index() << " : " << stream_hex_dump::to_hex(entry.page_range_descriptor_address(), hex_length) << '\n';
        using namespace size_units::base_16;
        wcout << indent_str << "  Extra Present: " << std::boolalpha << entry.extra_present() << '\n';
        auto const range_flags = entry.range_flags();
        wcout << indent_str << "  Range Flags: " << stream_hex_dump::to_hex(static_cast<uint32_t>(range_flags)) << " (" << dump_page_range_flags_to_string(range_flags) << ") - " << dump_page_range_to_string(range_flags) << '\n';
        wcout << indent_str << "  Extra Bytes: " << entry.extra_bytes() << " (" << stream_hex_dump::to_hex(entry.extra_bytes().count()) << ")" << '\n';
        wcout << indent_str << "  Committed Page Count: " << static_cast<uint16_t>(entry.committed_page_count()) << '\n';
        wcout << indent_str << "  Unit Offset: " << static_cast<uint16_t>(entry.unit_offset()) << '\n';
        wcout << indent_str << "  Unit Size: " << entry.unit_size() << " (" << stream_hex_dump::to_hex(entry.unit_size().count()) << ")" << '\n';
        wcout << indent_str << "  Block Address: " << stream_hex_dump::to_hex(entry.block_address(), hex_length) << '\n';
        wcout << indent_str << "  Block Size: " << entry.block_size() << " (" << stream_hex_dump::to_hex(entry.block_size().count()) << ")" << '\n';
        wcout << indent_str << "  User Address: " << stream_hex_dump::to_hex(entry.user_address(), hex_length) << '\n';
        wcout << indent_str << "  Requested User Size: " << entry.user_requested_size() << " (" << stream_hex_dump::to_hex(entry.user_requested_size().count()) << ")" << '\n';
        wcout << indent_str << "  UST Address: " << stream_hex_dump::to_hex(entry.ust_address(), hex_length) << '\n';
    }

    void print_segment_heap_page_entry(streamsize const hex_length, heap::page_range_descriptor const& entry, dump_file_options const& options, size_t const indent)
    {
        if(options.debug_heap_data())
        {
            print_segment_heap_page_entry_debug(hex_length, entry, indent);
        }
        else
        {
            print_segment_heap_page_entry_single_line(hex_length, entry, indent);
        }

        if(options.display_symbols() && !entry.allocation_stack_trace().empty())
        {
            std::wstring const indent_str(indent, L' ');
            wcout << indent_str << "  Allocation Stack Trace:\n";
            hex_dump_stack(wcout, entry.heap().walker(), entry.allocation_stack_trace(), entry.heap().peb().is_x86_target(), indent + 4);
            wcout << '\n';
        }

        if(options.hex_dump_memory_data() && entry.range_flags() == page_range_flags_utils::page_range_flags::PAGE_RANGE_BACKEND_SUBSEGMENT && entry.user_address() != 0)
        {
            uint64_t const size = entry.user_requested_size().count();
            if(auto stream = entry.walker().get_process_memory_stream(entry.user_address(), size); !stream.eof())
            {
                hex_dump::hex_dump(wcout, stream, options.hex_dump_memory_size(size), indent + 4);
                wcout << L'\n';
            }
        }
    }

    void print_segment_heap_page(streamsize const hex_length, size_t const page_index, heap::heap_page_segment const& page, dump_file_options const& options, size_t const indent)
    {
        std::wstring const indent_str(indent, L' ');
        wcout << indent_str << "Segment Page: " << page_index << " : " << stream_hex_dump::to_hex(page.heap_page_segment_address(), hex_length) << '\n';
        using namespace size_units::base_16;
        wcout << indent_str << "  Signature: " << stream_hex_dump::to_hex(page.signature(), hex_length) << (page.is_signature_valid() ? " (valid)" : "") << '\n';

        if(options.debug_heap_data())
        {
            for(auto const& entry : page.all_entries())
            {
                print_segment_heap_page_entry(hex_length, entry, options, indent + 2);
            }
        }
        else
        {
            for(auto const& entry : page.entries())
            {
                print_segment_heap_page_entry(hex_length, entry, options, indent + 2);
            }
        }
    }

    void print_segment_context_heap(streamsize const hex_length, size_t const segment_context_index, heap::heap_segment_context const& segment_context, dump_file_options const& options, size_t const indent)
    {
        std::wstring const indent_str(indent, L' ');
        wcout << indent_str << "Segment Context: " << segment_context_index << " : " << stream_hex_dump::to_hex(segment_context.heap_segment_context_address(), hex_length) << '\n';
        wcout << indent_str << "  Segment Mask: " << stream_hex_dump::to_hex(segment_context.segment_mask(), hex_length) << '\n';
        using namespace size_units::base_16;
        wcout << indent_str << "  Max Allocation Size: " << segment_context.max_allocation_size() << " (" << stream_hex_dump::to_hex(segment_context.max_allocation_size().count()) << ")" << '\n';
        wcout << indent_str << "  Unit Shift: " << stream_hex_dump::to_hex(segment_context.unit_shift()) << '\n';
        wcout << indent_str << "  Pages Per Unit Shift: " << static_cast<uint16_t>(segment_context.pages_per_unit_shift()) << '\n';
        wcout << indent_str << "  Segment Count: " << segment_context.segment_count() << '\n';

        size_t index{0};
        for(auto const& page : segment_context.pages())
        {
            print_segment_heap_page(hex_length, index, page, options, indent + 2);
            ++index;
        }

        auto first = true;
        for(auto const& entry : segment_context.free_page_ranges())
        {
            if(first)
            {
                wcout << indent_str << "  Free Page Ranges:\n";
                first = false;
            }
            print_segment_heap_page_entry(hex_length, entry, options, indent + 4);
        }
    }

    void print_vs_entry_single_line(streamsize const hex_length, heap::heap_vs_entry const& entry, size_t const indent)
    {
        std::wstring const indent_str(indent, L' ');
        using namespace size_units::base_16;
        wcout << indent_str << (entry.uncommitted_range() ? "VS Uncommitted Entry: " : (entry.allocated() ? "VS Busy Entry: " : "VS Free Entry: ")) << stream_hex_dump::to_hex(entry.heap_vs_entry_address(), hex_length);
        wcout << " size(" << entry.size() << " (" << stream_hex_dump::to_hex(entry.size().count()) << ")";

        if(!entry.uncommitted_range())
        {
            wcout << " PSize(" << entry.previous_size() << ")";
            if(entry.has_unused_bytes())
            {
                wcout << " UnusedBytes(" << entry.unused_bytes() << ')';
            }
            wcout << " UserPtr(" << stream_hex_dump::to_hex(entry.user_address(), hex_length) << ')';
            wcout << " ReqSize(" << entry.user_requested_size() << ")";
            if(!entry.is_valid())
            {
                wcout << " - invalid";
            }
        }

        wcout << '\n';
    }

    void print_vs_entry_debug(streamsize const hex_length, heap::heap_vs_entry const& entry, size_t const indent)
    {
        std::wstring const indent_str(indent, L' ');
        using namespace size_units::base_16;
        wcout << indent_str << (entry.uncommitted_range() ? "VS Uncommitted Entry: " : "VS Entry: ") << stream_hex_dump::to_hex(entry.heap_vs_entry_address(), hex_length) << '\n';
        wcout << indent_str << "  Size: " << entry.size() << " (" << stream_hex_dump::to_hex(entry.size().count()) << ")" << '\n';

        if(!entry.uncommitted_range())
        {
            wcout << indent_str << "  Previous Size: " << entry.previous_size() << " (" << stream_hex_dump::to_hex(entry.previous_size().count()) << ")" << '\n';
            wcout << indent_str << "  Is Valid: " << std::boolalpha << entry.is_valid() << '\n';
            wcout << indent_str << "  Allocated: " << std::boolalpha << entry.allocated() << '\n';
            wcout << indent_str << "  Memory Cost: " << entry.memory_cost() << '\n';
            wcout << indent_str << "  Segment Page Offset: " << static_cast<uint16_t>(entry.segment_page_offset()) << '\n';
            wcout << indent_str << "  Has Unused Bytes: " << std::boolalpha << entry.has_unused_bytes() << '\n';
            if(entry.has_unused_bytes())
            {
                wcout << indent_str << "  Unused Bytes: " << entry.unused_bytes() << '\n';
            }
            wcout << indent_str << "  Skip During Walk: " << std::boolalpha << entry.skip_during_walk() << '\n';
            wcout << indent_str << "  Spare: " << stream_hex_dump::to_hex(entry.spare()) << '\n';
            wcout << indent_str << "  Allocated Chunk Bits: " << stream_hex_dump::to_hex(entry.allocated_chunk_bits()) << '\n';
            wcout << indent_str << "  User Address: " << stream_hex_dump::to_hex(entry.user_address(), hex_length) << '\n';
            wcout << indent_str << "  Requested User Size: " << entry.user_requested_size() << " (" << stream_hex_dump::to_hex(entry.user_requested_size().count()) << ")" << '\n';
            wcout << indent_str << "  UST Address: " << stream_hex_dump::to_hex(entry.ust_address(), hex_length) << '\n';
        }
    }

    void print_vs_entry(streamsize const hex_length, heap::heap_vs_entry const& entry, dump_file_options const& options, size_t const indent)
    {
        if(options.debug_heap_data())
        {
            print_vs_entry_debug(hex_length, entry, indent);
        }
        else
        {
            print_vs_entry_single_line(hex_length, entry, indent);
        }

        if(!entry.uncommitted_range())
        {
            if(options.display_symbols() && entry.allocated() && !entry.allocation_stack_trace().empty())
            {
                std::wstring const indent_str(indent, L' ');
                wcout << indent_str << "  Allocation Stack Trace:\n";
                hex_dump_stack(wcout, entry.heap().walker(), entry.allocation_stack_trace(), entry.heap().peb().is_x86_target(), indent + 4);
                wcout << '\n';
            }

            if(options.hex_dump_memory_data() && entry.allocated())
            {
                uint64_t const size = entry.user_requested_size().count();
                if(auto stream = entry.walker().get_process_memory_stream(entry.user_address(), size); !stream.eof())
                {
                    hex_dump::hex_dump(wcout, stream, options.hex_dump_memory_size(size), indent + 2);
                    wcout << L'\n';
                }
            }
        }
    }

    void print_vs_subsegment(streamsize const hex_length, heap::heap_vs_subsegment const& subsegment, dump_file_options const& options, size_t const indent)
    {
        std::wstring const indent_str(indent, L' ');
        using namespace size_units::base_16;
        wcout << indent_str << "VS Subsegment: " << stream_hex_dump::to_hex(subsegment.heap_vs_subsegment_address(), hex_length) << '\n';
        wcout << indent_str << "  Size: " << subsegment.size() << " (" << stream_hex_dump::to_hex(subsegment.size().count()) << ")" << '\n';
        wcout << indent_str << "  Signature: " << stream_hex_dump::to_hex(subsegment.signature()) << (subsegment.is_signature_valid() ? " (valid)" : " (invalid)") << '\n';
        wcout << indent_str << "  Full Commit: " << std::boolalpha << subsegment.full_commit() << '\n';

        for(auto const& entry : subsegment.entries())
        {
            print_vs_entry(hex_length, entry, options, indent + 2);
        }
    }

    void print_vs_context_heap(streamsize const hex_length, heap::heap_vs_context const& vs_context, dump_file_options const& options, size_t const indent)
    {
        std::wstring const indent_str(indent, L' ');
        wcout << indent_str << "VS Context: " << stream_hex_dump::to_hex(vs_context.heap_vs_context_address(), hex_length) << '\n';
        wcout << indent_str << "  Total Committed Units: " << vs_context.total_committed_units() << '\n';
        wcout << indent_str << "  Free Committed Units: " << vs_context.free_committed_units() << '\n';

        for(auto const& subsegment : vs_context.subsegments())
        {
            print_vs_subsegment(hex_length, subsegment, options, indent + 2);
        }

        wcout << indent_str << "  Free Entries:\n";
        for(auto const& entry : vs_context.free_entries())
        {
            print_vs_entry(hex_length, entry, options, indent + 4);
        }
    }

    void print_heap_large_entry_single_line(streamsize const hex_length, heap::large_alloc_entry const& entry, size_t const indent)
    {
        std::wstring const indent_str(indent, L' ');
        using namespace size_units::base_16;
        wcout << indent_str << "Large Entry: " << stream_hex_dump::to_hex(entry.large_alloc_entry_address(), hex_length);
        wcout << " Size(" << entry.size() << ')';
        wcout << " VirtualPtr(" << stream_hex_dump::to_hex(entry.virtual_address(), hex_length) << ')';
        wcout << " UnusedBytes(" << entry.unused_bytes() << ')';
        wcout << " UserPtr(" << stream_hex_dump::to_hex(entry.user_address(), hex_length) << ')';
        wcout << " ReqSize(" << entry.user_requested_size() << ')';
        wcout << '\n';
    }

    void print_heap_large_entry_debug(streamsize const hex_length, heap::large_alloc_entry const& entry, size_t const indent)
    {
        std::wstring const indent_str(indent, L' ');
        using namespace size_units::base_16;
        wcout << indent_str << "Large Entry: " << stream_hex_dump::to_hex(entry.large_alloc_entry_address(), hex_length) << '\n';
        wcout << indent_str << "  Size: " << entry.size() << " (" << stream_hex_dump::to_hex(entry.size().count()) << ")" << '\n';
        wcout << indent_str << "  Virtual Address: " << stream_hex_dump::to_hex(entry.virtual_address(), hex_length) << '\n';
        wcout << indent_str << "  Unused Bytes: " << entry.unused_bytes() << " (" << stream_hex_dump::to_hex(entry.unused_bytes().count()) << ")" << '\n';
        wcout << indent_str << "  Spare: " << stream_hex_dump::to_hex(entry.spare()) << '\n';
        wcout << indent_str << "  Extra Present: " << std::boolalpha << entry.extra_present() << '\n';
        wcout << indent_str << "  Guard Page Count: " << entry.guard_page_count() << '\n';
        wcout << indent_str << "  Guard Page Alignment: " << entry.guard_page_alignment() << '\n';
        wcout << indent_str << "  Allocated Pages: " << entry.allocated_pages() << '\n';
        wcout << indent_str << "  User Address: " << stream_hex_dump::to_hex(entry.user_address(), hex_length) << '\n';
        wcout << indent_str << "  Requested User Size: " << entry.user_requested_size() << " (" << stream_hex_dump::to_hex(entry.user_requested_size().count()) << ")" << '\n';
        wcout << indent_str << "  UST Address: " << stream_hex_dump::to_hex(entry.ust_address(), hex_length) << '\n';
    }

    void print_heap_large_entry(streamsize const hex_length, heap::large_alloc_entry const& entry, dump_file_options const& options, size_t const indent)
    {
        if(options.debug_heap_data())
        {
            print_heap_large_entry_debug(hex_length, entry, indent);
        }
        else
        {
            print_heap_large_entry_single_line(hex_length, entry, indent);
        }

        if(options.display_symbols() && !entry.allocation_stack_trace().empty())
        {
            std::wstring const indent_str(indent, L' ');
            wcout << indent_str << "  Allocation Stack Trace:\n";
            hex_dump_stack(wcout, entry.heap().walker(), entry.allocation_stack_trace(), entry.heap().peb().is_x86_target(), indent + 4);
            wcout << '\n';
        }

        if(options.hex_dump_memory_data())
        {
            uint64_t const size = entry.user_requested_size().count();
            if(auto stream = entry.walker().get_process_memory_stream(entry.user_address(), size); !stream.eof())
            {
                hex_dump::hex_dump(wcout, stream, options.hex_dump_memory_size(size), indent + 2);
                wcout << L'\n';
            }
        }
    }

    void print_lfh_entry_single_line(streamsize const hex_length, heap::heap_lfh_entry const& entry, size_t const indent)
    {
        std::wstring const indent_str(indent, L' ');
        using namespace size_units::base_16;
        wcout << indent_str << (entry.allocated() ? "Busy LFH Entry: " : "Free LFH Entry: ") << stream_hex_dump::to_hex(entry.heap_lfh_entry_address(), hex_length);
        wcout << " BlockSize(" << entry.block_size() << ')';
        if(entry.has_unused_bytes())
        {
            wcout << " UnusedBytes(" << entry.unused_bytes() << ')';
        }
        wcout << " UserPtr(" << stream_hex_dump::to_hex(entry.user_address(), hex_length) << ')';
        wcout << " ReqSize(" << entry.user_requested_size() << ')';
        wcout << '\n';
    }

    void print_lfh_entry_debug(streamsize const hex_length, heap::heap_lfh_entry const& entry, size_t const indent)
    {
        std::wstring const indent_str(indent, L' ');
        using namespace size_units::base_16;
        wcout << indent_str << "LFH Entry: " << stream_hex_dump::to_hex(entry.heap_lfh_entry_address(), hex_length) << '\n';
        wcout << indent_str << "  Block Size: " << entry.block_size() << " (" << stream_hex_dump::to_hex(entry.block_size().count()) << ")" << '\n';
        wcout << indent_str << "  Allocated: " << std::boolalpha << entry.allocated() << '\n';
        wcout << indent_str << "  Has Unused Bytes: " << std::boolalpha << entry.has_unused_bytes() << '\n';
        if(entry.has_unused_bytes())
        {
            wcout << indent_str << "  Unused Bytes: " << entry.unused_bytes() << " (" << stream_hex_dump::to_hex(entry.unused_bytes().count()) << ")"  << '\n';
        }
        wcout << indent_str << "  User Address: " << stream_hex_dump::to_hex(entry.user_address(), hex_length) << '\n';
        wcout << indent_str << "  Requested User Size: " << entry.user_requested_size() << " (" << stream_hex_dump::to_hex(entry.user_requested_size().count()) << ")" << '\n';
        wcout << indent_str << "  UST Address: " << stream_hex_dump::to_hex(entry.ust_address(), hex_length) << '\n';
    }

    void print_lfh_entry(streamsize const hex_length, heap::heap_lfh_entry const& entry, dump_file_options const& options, size_t const indent)
    {
        if(options.debug_heap_data())
        {
            print_lfh_entry_debug(hex_length, entry, indent);
        }
        else
        {
            print_lfh_entry_single_line(hex_length, entry, indent);
        }

        if(options.display_symbols() && entry.allocated() && !entry.allocation_stack_trace().empty())
        {
            std::wstring const indent_str(indent, L' ');
            wcout << indent_str << "  Allocation Stack Trace:\n";
            hex_dump_stack(wcout, entry.heap().walker(), entry.allocation_stack_trace(), entry.heap().peb().is_x86_target(), indent + 4);
            wcout << '\n';
        }

        if(options.hex_dump_memory_data() && entry.allocated() && entry.user_address() != 0)
        {
            uint64_t const size = entry.user_requested_size().count();
            if(auto stream = entry.walker().get_process_memory_stream(entry.user_address(), size); !stream.eof())
            {
                hex_dump::hex_dump(wcout, stream, options.hex_dump_memory_size(size), indent + 2);
                wcout << L'\n';
            }
        }
    }

    void print_lfh_subsegment(streamsize const hex_length, heap::heap_lfh_subsegment const& subsegment, dump_file_options const& options, size_t const indent)
    {
        std::wstring const indent_str(indent, L' ');
        using namespace size_units::base_16;
        wcout << indent_str << "LFH Subsegment: " << stream_hex_dump::to_hex(subsegment.heap_lfh_subsegment_address(), hex_length) << '\n';
        wcout << indent_str << "  Free Count: " << subsegment.free_count() << '\n';
        wcout << indent_str << "  Block Count: " << subsegment.block_count() << '\n';
        wcout << indent_str << "  Block Size: " << subsegment.block_size() << " (" << stream_hex_dump::to_hex(subsegment.block_size().count()) << ")" << '\n';
        auto const location{subsegment.location()};
        wcout << indent_str << "  Location: " << static_cast<uint16_t>(location) << " (" << dump_page_range_flags_to_string(location) << ")" << '\n';
        if(options.debug_heap_data())
        {
            wcout << indent_str << "  Witheld Block Count: " << static_cast<uint16_t>(subsegment.witheld_block_count()) << '\n';
            wcout << indent_str << "  Commit Unit Count: " << static_cast<uint16_t>(subsegment.commit_unit_count()) << '\n';
            wcout << indent_str << "  Commit Unit Shift: " << static_cast<uint16_t>(subsegment.commit_unit_shift()) << '\n';
        }

        for(auto const& entry : subsegment.entries())
        {
            print_lfh_entry(hex_length, entry, options, indent + 2);
        }
    }

    void print_lfh_affinity_slot(streamsize const hex_length, heap::heap_lfh_affinity_slot const& affinity_slot, dump_file_options const& options, size_t const indent)
    {
        std::wstring const indent_str(indent, L' ');
        using namespace size_units::base_16;
        wcout << indent_str << "LFH Affinity Slot: " << stream_hex_dump::to_hex(affinity_slot.heap_lfh_affinity_slot_address(), hex_length) << '\n';
        wcout << indent_str << "  Slot Index: " << affinity_slot.slot_index() << '\n';
        wcout << indent_str << "  Bucket Index: " << affinity_slot.bucket_index() << '\n';
        wcout << indent_str << "  Available Subsegment Count: " << affinity_slot.available_subsegment_count() << '\n';

        for(auto const& subsegment : affinity_slot.subsegments())
        {
            print_lfh_subsegment(hex_length, subsegment, options, indent + 2);
        }
    }

    void print_lfh_bucket_single_line(streamsize const hex_length, heap::heap_lfh_bucket const& lfh_bucket, size_t const indent)
    {
        std::wstring const indent_str(indent, L' ');
        using namespace size_units::base_16;
        wcout << indent_str << "LFH Bucket: " << stream_hex_dump::to_hex(lfh_bucket.heap_lfh_bucket_address(), hex_length);
        wcout << " @ " << lfh_bucket.bucket_index();
        wcout << (lfh_bucket.is_enabled() ? " Enabled" : " Disabled");
        wcout << " Granularity(" << lfh_bucket.bucket_granularity() << ')';
        wcout << " MaxAllocationSize(" << lfh_bucket.max_allocation_size() << ')';

        if(lfh_bucket.is_enabled())
        {
            wcout << " TotalBlockCount(" << lfh_bucket.total_block_count() << ')';
            wcout << " TotalSubsegmentCount(" << lfh_bucket.total_subsegment_count() << ')';
            wcout << " Shift(" << static_cast<uint16_t>(lfh_bucket.shift()) << ')';
        }
        else
        {
            wcout << " UsageCount(" << lfh_bucket.usage_count() << ')';
        }
        wcout << '\n';
    }

    void print_lfh_bucket_debug(streamsize const hex_length, heap::heap_lfh_bucket const& lfh_bucket, size_t const indent)
    {
        std::wstring const indent_str(indent, L' ');
        using namespace size_units::base_16;
        wcout << indent_str << "LFH Bucket: " << stream_hex_dump::to_hex(lfh_bucket.heap_lfh_bucket_address(), hex_length) << '\n';
        wcout << indent_str << "  Bucket Index: " << lfh_bucket.bucket_index() << '\n';
        wcout << indent_str << "  Bucket Enabled: " << std::boolalpha << lfh_bucket.is_enabled() << '\n';
        wcout << indent_str << "  Granularity: " << lfh_bucket.bucket_granularity() << " (" << stream_hex_dump::to_hex(lfh_bucket.bucket_granularity().count()) << ")" << '\n';
        wcout << indent_str << "  Max Allocation Size: " << lfh_bucket.max_allocation_size() << " (" << stream_hex_dump::to_hex(lfh_bucket.max_allocation_size().count()) << ")" << '\n';

        if(lfh_bucket.is_enabled())
        {
            wcout << indent_str << "  Slot Count: " << lfh_bucket.slot_count() << '\n';
            wcout << indent_str << "  Total Block Count: " << lfh_bucket.total_block_count() << '\n';
            wcout << indent_str << "  Total Subsegment Count: " << lfh_bucket.total_subsegment_count() << '\n';
            wcout << indent_str << "  Shift: " << static_cast<uint16_t>(lfh_bucket.shift()) << '\n';
            wcout << indent_str << "  Reciprocal Block Size: " << lfh_bucket.reciprocal_block_size() << " (" << stream_hex_dump::to_hex(lfh_bucket.reciprocal_block_size()) << ")"  << '\n';
            auto const processor_affinity_mapping = lfh_bucket.processor_affinity_mapping();
            wcout << indent_str << "  Processor Affinity Mapping:\n";
            for(size_t index = 0; index < processor_affinity_mapping.size(); ++index)
            {
                wcout << indent_str << "    { CPU:" << index << " -> Slot:" << processor_affinity_mapping[index] << " }\n";
            }
        }
        else
        {
            wcout << indent_str << "  Usage Count: " << lfh_bucket.usage_count() << '\n';
        }
    }

    void print_lfh_bucket(streamsize const hex_length, heap::heap_lfh_bucket const& lfh_bucket, dump_file_options const& options, size_t const indent)
    {
        if(options.debug_heap_data())
        {
            print_lfh_bucket_debug(hex_length, lfh_bucket, indent);
        }
        else
        {
            print_lfh_bucket_single_line(hex_length, lfh_bucket, indent);
        }

        if(lfh_bucket.is_enabled())
        {
            for(auto const& affinity_slot : lfh_bucket.affinity_slots())
            {
                print_lfh_affinity_slot(hex_length, affinity_slot, options, indent + 2);
            }
        }
    }

    void print_lfh_context_heap(streamsize const hex_length, const heap::heap_lfh_context& lfh_context, dump_file_options const& options, size_t const indent)
    {
        std::wstring const indent_str(indent, L' ');
        using namespace size_units::base_16;
        wcout << indent_str << "LFH Context: " << stream_hex_dump::to_hex(lfh_context.heap_lfh_context_address(), hex_length) << '\n';
        wcout << indent_str << "  Max Affinity: " << static_cast<uint16_t>(lfh_context.max_affinity()) << '\n';
        wcout << indent_str << "  Max Block Size: " << lfh_context.max_block_size() << " (" << stream_hex_dump::to_hex(lfh_context.max_block_size().count()) << ")" << '\n';
        wcout << indent_str << "  With Old Page Crossing Blocks: " << std::boolalpha << lfh_context.with_old_page_crossing_blocks() << '\n';
        wcout << indent_str << "  Disable Randomization: " << std::boolalpha << lfh_context.disable_randomization() << '\n';

        for(auto const& bucket : lfh_context.active_buckets())
        {
            print_lfh_bucket(hex_length, bucket, options, indent + 2);
        }
    }

    void print_segment_heap(std::streamsize const hex_length, char const* process_heap_marker, heap::segment_heap const& segment_heap, dump_file_options const& options, size_t const indent)
    {
        std::wstring const indent_str(indent, L' ');
        wcout << indent_str << "Segment Heap: " << stream_hex_dump::to_hex(segment_heap.segment_heap_address(), hex_length) << ' ' << process_heap_marker << '\n';
        wcout << indent_str << "  Global Flags: " << stream_hex_dump::to_hex_full(segment_heap.global_flags()) << '\n';
        using namespace size_units::base_16;
        wcout << indent_str << "  Total Reserved: " << segment_heap.reserved() << " (" << stream_hex_dump::to_hex(segment_heap.reserved().count()) << ")" << '\n';
        wcout << indent_str << "  Total Committed: " << segment_heap.committed() << " (" << stream_hex_dump::to_hex(segment_heap.committed().count()) << ")" << '\n';
        wcout << indent_str << "  Total Uncommitted: " << segment_heap.uncommitted() << " (" << stream_hex_dump::to_hex(segment_heap.uncommitted().count()) << ")" << '\n';
        wcout << indent_str << "  Large Reserved: " << segment_heap.large_reserved() << " (" << stream_hex_dump::to_hex(segment_heap.large_reserved().count()) << ")" << '\n';
        wcout << indent_str << "  Large Committed: " << segment_heap.large_committed() << " (" << stream_hex_dump::to_hex(segment_heap.large_committed().count()) << ")" << '\n';
        wcout << indent_str << "  Large Uncommitted: " << segment_heap.large_uncommitted() << " (" << stream_hex_dump::to_hex(segment_heap.large_uncommitted().count()) << ")" << '\n';
        wcout << indent_str << "  Total Reserved Pages: " << segment_heap.total_reserved_pages() << '\n';
        wcout << indent_str << "  Total Committed Pages: " << segment_heap.total_committed_pages() << '\n';
        wcout << indent_str << "  Free Committed Pages: " << segment_heap.free_committed_pages() << '\n';
        wcout << indent_str << "  LFH Free Committed Pages: " << segment_heap.lfh_free_committed_pages() << '\n';
        wcout << indent_str << "  Large Reserved Pages: " << segment_heap.large_reserved_pages() << '\n';
        wcout << indent_str << "  Large Committed Pages: " << segment_heap.large_committed_pages() << '\n';
        wcout << indent_str << "  Heap Key: " << stream_hex_dump::to_hex(segment_heap.heap_key(), hex_length) << '\n';
        wcout << '\n';

        size_t index{0};
        for(auto const& segment_context : segment_heap.segment_contexts())
        {
            print_segment_context_heap(hex_length, index, segment_context, options, indent + 2);
            ++index;
        }

        print_vs_context_heap(hex_length, segment_heap.vs_context(), options, indent + 2);
        print_lfh_context_heap(hex_length, segment_heap.lfh_context(), options, indent + 2);

        for(auto const& entry : segment_heap.large_entries())
        {
            print_heap_large_entry(hex_length, entry, options, indent + 2);
        }
    }
}

void dump_mini_dump_heap(mini_dump const& mini_dump, dump_file_options const& options, dbg_help::symbol_engine& symbol_engine)
{
    process::process_environment_block const peb{mini_dump, symbol_engine};
    auto const hex_length = peb.machine_hex_printable_length();

    wcout << "Heaps:\n";
    wcout << ' ' << std::left << std::setw(hex_length + 2) << "Address" << " NT/Segment   Flags      "
        << std::left << std::setw(units_max_width) << "Reserved" << ' '
        << std::left << std::setw(units_max_width) << "Committed" << ' '
        << std::left << std::setw(units_max_width) << "Uncommitted" << ' '
        << std::left << std::setw(units_max_width) << "Virtual" << ' '
        << std::left << std::setw(units_max_width) << "Free" << ' '
        << std::left << std::setw(8) << "Segments" << ' '
        << std::left << std::setw(4) << "UCRs" << ' '
        << '\n';
    for(uint32_t heap_index = 0; heap_index < peb.number_of_heaps(); ++heap_index)
    {
        auto const heap_address = peb.heap_address(heap_index);
        wcout << ' ' << stream_hex_dump::to_hex(heap_address, hex_length);

        if(auto const segment_signature = peb.segment_signature(heap_index); segment_signature == heap::SegmentSignatureNtHeap)
        {
            if(auto const nt_heap = peb.nt_heap(heap_index); nt_heap.has_value())
            {
                print_nt_heap_line(get_process_marker(nt_heap.value().is_process_heap(peb.process_heap())), nt_heap.value());
                print_nt_heap_segments_list(hex_length, nt_heap.value(), 2);
                print_nt_heap_lfh_segments_list(hex_length, nt_heap.value(), 2);
            }
        }
        else if(segment_signature == heap::SegmentSignatureSegmentHeap)
        {
            if(auto const segment_heap = peb.segment_heap(heap_index); segment_heap.has_value())
            {
                print_segment_heap_line(get_process_marker(peb.process_heap() == heap_address), segment_heap.value());
            }
        }
        else
        {
            wcout << " Unknown Heap Type (" << stream_hex_dump::to_hex(segment_signature) << ")\n";
        }
    }

    wcout << '\n';

    for(uint32_t heap_index = 0; heap_index < peb.number_of_heaps(); ++heap_index)
    {
        if(auto const nt_heap = peb.nt_heap(heap_index); nt_heap.has_value())
        {
            print_nt_heap(hex_length, get_process_marker(nt_heap.value().is_process_heap(peb.process_heap())), nt_heap.value(), 0);

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
                print_heap_segment(hex_length, segment_index, segment, lfh_data, options, 4);
                ++segment_index;
            }

            auto printed_lfh_output{false};
            for(auto& data : lfh_data)
            {
                if(!data.printed_subsegment)
                {
                    print_lfh_heap_segment(hex_length, data, options, 4);
                    printed_lfh_output = true;
                }
            }

            if(printed_lfh_output)
            {
                wcout << '\n';
            }

            print_nt_heap_uncommitted_ranges(nt_heap.value(), 2);

            print_nt_heap_virtual_allocated_blocks(hex_length, nt_heap.value(), options, 2);

            print_nt_heap_free_list(hex_length, nt_heap.value(), options, 2);
        }
        else if(auto const segment_heap = peb.segment_heap(heap_index); segment_heap.has_value())
        {
            print_segment_heap(hex_length, get_process_marker(peb.process_heap() == segment_heap.value().segment_heap_address()), segment_heap.value(), options, 0);
        }
    }

    for(auto const& heap : heap::dph_heap::dph_heaps(peb))
    {
        print_debug_page_heap(hex_length, heap, options, 2);
    }

    wcout << '\n';
}

void print_process_entry(heap::process_heap_entry const& entry, process::process_environment_block const& peb, std::streamsize const hex_length, dump_file_options const& options, size_t const indent)
{
    std::wstring const indent_str(indent, L' ');
    using namespace size_units::base_16;
    wcout << indent_str << stream_hex_dump::to_hex(entry.user_address(), hex_length) << " size(" << entry.user_requested_size() << ") overhead(" << entry.overhead_size() << ")";
    if(!entry.filename().empty())
    {
        wcout << ' ' << entry.filename() << ':' << entry.line_number();
    }
    wcout << (entry.allocation_stack_trace().empty() ? "" : " (has stack)") << '\n';


    if(options.display_symbols() && !entry.allocation_stack_trace().empty())
    {
        wcout << indent_str << "  Allocation Stack Trace:\n";
        hex_dump_stack(wcout, peb.walker(), entry.allocation_stack_trace(), peb.is_x86_target(), indent + 4);
        wcout << '\n';
    }

    if(options.hex_dump_memory_data())
    {
        if(auto stream = entry.user_data(); !stream.eof())
        {
            hex_dump::hex_dump(wcout, stream, options.hex_dump_memory_size(entry.user_requested_size().count()), indent + 4);
            wcout << L'\n';
        }
    }
}

void dump_mini_dump_heap_entries(mini_dump const& mini_dump, dump_file_options const& options, dbg_help::symbol_engine& symbol_engine)
{
    heap::process_heaps const heaps{mini_dump, symbol_engine, options.system_module_list(), options.statistic_view_options()};
    auto const hex_length = heaps.peb().machine_hex_printable_length();

    cout << "Heap Allocated Entries:\n";
    for(auto const& entry : heaps.entries())
    {
        print_process_entry(entry, heaps.peb(), hex_length, options, 2);
    }

    cout << "Heap Free Entries:\n";
    for(auto const& entry : heaps.free_entries())
    {
        print_process_entry(entry, heaps.peb(), hex_length, options, 2);
    }

    wcout << '\n';
}

void dump_mini_dump_crtheap(mini_dump const& mini_dump, dump_file_options const& options, dbg_help::symbol_engine& symbol_engine)
{
    process::process_environment_block const peb{mini_dump, symbol_engine};
    auto const hex_length = peb.machine_hex_printable_length();

    heap::crt_heap const heap{peb};

    if(!heap.is_using_crt_heap())
    {
        wcout << "Not using CRT Heap.\n";
        return;
    }

    cout << "CRT Heap Entries:\n";
    std::wstring const indent_str(2, L' ');
    for (auto const& entry : heap.entries())
    {
        using namespace size_units::base_16;
        if(options.debug_heap_data())
        {
            wcout << indent_str << "CRT Entry: " << stream_hex_dump::to_hex(entry.entry_address(), hex_length) << '\n';
            wcout << indent_str << "  Block In Use: " << std::boolalpha << entry.block_use() << '\n';
            wcout << indent_str << "  Filename: " << entry.filename() << '\n';
            wcout << indent_str << "  Line Number: " << entry.line_number() << '\n';
            wcout << indent_str << "  Request Number: " << entry.request_number() << '\n';
            wcout << indent_str << "  Data Size: " << entry.data_size() << " (" << stream_hex_dump::to_hex(entry.data_size().count()) << ")" << '\n';
            wcout << indent_str << "  User Address: " << stream_hex_dump::to_hex(entry.user_address(), hex_length) << '\n';
        }
        else
        {
            wcout << indent_str << "CRT Entry: " << stream_hex_dump::to_hex(entry.entry_address(), hex_length)
                  << (entry.block_use() ? " in use" : " free")
                  << " " << entry.data_size()
                  << " UserPtr(" << stream_hex_dump::to_hex(entry.user_address(), hex_length) << ')';
            if(!entry.filename().empty())
            {
                wcout << " : " << entry.filename() << ':' <<  entry.line_number();
            }
            wcout << '\n';
        }

        if(entry.block_use() && options.hex_dump_memory_data())
        {
            uint64_t const size = entry.data_size().count();
            if(auto stream = entry.walker().get_process_memory_stream(entry.user_address(), size); !stream.eof())
            {
                hex_dump::hex_dump(wcout, stream, options.hex_dump_memory_size(size), 4);
                wcout << L'\n';
            }
        }
    }
}

void dump_mini_dump_heap_statistics_view(process::process_environment_block const& peb, heap::process_heaps_statistic_view const& view_by_size_frequency, dump_file_options const& options, bool const is_x86_target, streamsize const hex_length)
{
    using namespace size_units::base_16;
    wcout << "  " << heap::process_heaps_statistic_view::to_wstring(view_by_size_frequency.view()) << ":\n";
    auto const single_line_range_title_length = 2 + hex_length;

    if(view_by_size_frequency.is_range_single_value())
    {
        wcout << std::format(L" {0:<10} ", L"");
        wcout << std::format(L"{0:<{1}} ", L"", single_line_range_title_length);
    }
    else
    {
        wcout << std::format(L" {0:<10} {1:<10} ", L"range", L"range");
    }

    wcout << std::format(L"{:<10} ", L"allocated");
    wcout << std::format(L"{:<10} ", L"allocated");
    wcout << std::format(L"{:<10} ", L"allocated");
    wcout << std::format(L"{:<10} ", L"overhead");
    wcout << std::format(L"{:<8} ", L"free");
    wcout << std::format(L"{:<10} ", L"free");
    wcout << std::format(L"{:<7} ", L"count");
    wcout << std::format(L"{:<7} ", L"size");
    wcout << L"\n";

    if(view_by_size_frequency.is_range_single_value())
    {
        wcout << std::format(L" {0:<10} ", L"size");
        wcout << std::format(L"{0:<{1}} ", L"size hex", single_line_range_title_length);
    }
    else
    {
        wcout << std::format(L" {0:<10} {1:<10} ", L"start", L"size");
    }

    wcout << std::format(L"{:<10} ", L"count");
    wcout << std::format(L"{:<10} ", L"total");
    wcout << std::format(L"{:<10} ", L"average");
    wcout << std::format(L"{:<10} ", L"total");
    wcout << std::format(L"{:<8} ", L"count");
    wcout << std::format(L"{:<10} ", L"total");
    wcout << std::format(L"{:<7} ", L"percent");
    wcout << std::format(L"{:<7} ", L"percent");
    wcout << L" application call site\n";

    if(view_by_size_frequency.is_range_single_value())
    {
        wcout << std::format(L"={0:=<10}=", L"");
        wcout << std::format(L"{0:=<{1}}=", L"", single_line_range_title_length);
    }
    else
    {
        wcout << std::format(L" {0:=<10}={1:=<10}=", L"", L"");
    }

    wcout << std::format(L"{:=<10}=", L"");
    wcout << std::format(L"{:=<10}=", L"");
    wcout << std::format(L"{:=<10}=", L"");
    wcout << std::format(L"{:=<10}=", L"");
    wcout << std::format(L"{:=<8}=", L"");
    wcout << std::format(L"{:=<10}=", L"");
    wcout << std::format(L"{:=<7}=", L"");
    wcout << std::format(L"{:=<7}=", L"");
    wcout << L"======================\n";

    for(auto const& bucket : view_by_size_frequency.buckets())
    {
        if(view_by_size_frequency.is_range_single_value())
        {
            wcout << std::format(L" {:10} ", to_wstring(bucket.start_range()));
            wcout << std::format(L"{0:{1}} ", std::format(L"0x{0:x}", bucket.start_range().count()), single_line_range_title_length);
        }
        else
        {
            wcout << std::format(L" {0:10} {1:10} ", to_wstring(bucket.start_range()), to_wstring(bucket.end_range() - bucket.start_range()));
        }
        wcout << std::format(L"{:10} ", locale_formatting::to_wstring(bucket.allocated_count()));
        wcout << std::format(L"{:10} ", to_wstring(bucket.allocated_total()));
        wcout << std::format(L"{:10} ", to_wstring(bucket.allocated_average()));
        wcout << std::format(L"{:10} ", to_wstring(bucket.overhead_total()));
        wcout << std::format(L"{:8} ", locale_formatting::to_wstring(bucket.free_count()));
        wcout << std::format(L"{:10} ", to_wstring(bucket.free_total()));
        wcout << std::format(L"{:<7.2f} ", bucket.range_count_percent());
        wcout << std::format(L"{:<7.2f} ", bucket.range_size_percent());
        if(bucket.common_allocation_callsite().has_value())
        {
            wcout << std::format(L" {0}", stream_stack_dump::hex_dump_stack_frame(bucket.common_allocation_callsite().value(), is_x86_target));
        }
        wcout << '\n';

        if(options.display_symbols() && !bucket.allocation_stack_trace().empty())
        {
            wcout << "  Allocation Stack Trace:\n";
            hex_dump_stack(wcout, peb.walker(), bucket.allocation_stack_trace(), is_x86_target, 2);
            wcout << '\n';
        }

        if(options.debug_heap_data())
        {
            wcout << "  Entries:\n";
            for(auto const& entry : bucket.entries())
            {
                print_process_entry(entry, peb, hex_length, options, 4);
            }

            wcout << "  Free Entries:\n";
            for(auto const& entry : bucket.free_entries())
            {
                print_process_entry(entry, peb, hex_length, options, 4);
            }
        }
    }

    if(view_by_size_frequency.is_range_single_value())
    {
        wcout << std::format(L"={0:=<10}=", L"");
        wcout << std::format(L"{0:=<{1}}=", L"", single_line_range_title_length);
    }
    else
    {
        wcout << std::format(L" {0:=<10}={1:=<10}=", L"", L"");
    }

    wcout << std::format(L"{:=<10}=", L"");
    wcout << std::format(L"{:=<10}=", L"");
    wcout << std::format(L"{:=<10}=", L"");
    wcout << std::format(L"{:=<10}=", L"");
    wcout << std::format(L"{:=<8}=", L"");
    wcout << std::format(L"{:=<10}=", L"");
    wcout << std::format(L"{:=<7}=", L"");
    wcout << std::format(L"{:=<7}=", L"");
    wcout << L"======================\n";

    if(view_by_size_frequency.is_range_single_value())
    {
        wcout << std::format(L" {:10} ", L"");
        wcout << std::format(L"{0:>{1}} ", L"Total:", single_line_range_title_length);
    }
    else
    {
        wcout << std::format(L" {0:10} {1:>10} ", L"", L"Total:");
    }
    wcout << std::format(L"{:10} ", locale_formatting::to_wstring(view_by_size_frequency.allocated_count()));
    wcout << std::format(L"{:10} ", to_wstring(view_by_size_frequency.allocated_total()));
    wcout << std::format(L"{:10} ", to_wstring(view_by_size_frequency.allocated_average()));
    wcout << std::format(L"{:10} ", to_wstring(view_by_size_frequency.overhead_total()));
    wcout << std::format(L"{:8} ", locale_formatting::to_wstring(view_by_size_frequency.free_count()));
    wcout << std::format(L"{:10} ", to_wstring(view_by_size_frequency.free_total()));
    wcout << '\n';
}

void dump_mini_dump_heap_statistics(mini_dump const& mini_dump, dump_file_options const& options, dbg_help::symbol_engine& symbol_engine)
{
    heap::process_heaps const heaps{mini_dump, symbol_engine, options.system_module_list(), options.statistic_view_options()};
    auto const hex_length = heaps.peb().machine_hex_printable_length();
    auto const is_x86_target = heaps.peb().is_x86_target();
    auto const statistics = heaps.statistics();
    wcout << "Heap Statistics:\n";
    if(options.display_heap_statistic_view(heap_statistics_view::by_size_frequency_view))
    {
        dump_mini_dump_heap_statistics_view(heaps.peb(), statistics.view_by_size_frequency(), options, is_x86_target, hex_length);
        wcout << '\n';
    }
    if(options.display_heap_statistic_view(heap_statistics_view::by_size_ranges_frequency_view))
    {
        dump_mini_dump_heap_statistics_view(heaps.peb(), statistics.view_by_size_ranges_frequency(), options, is_x86_target, hex_length);
        wcout << '\n';
    }
    if(options.display_heap_statistic_view(heap_statistics_view::by_stacktrace_frequency_view))
    {
        dump_mini_dump_heap_statistics_view(heaps.peb(), statistics.view_by_stacktrace_frequency(), options, is_x86_target, hex_length);
        wcout << '\n';
    }
    if(options.display_heap_statistic_view(heap_statistics_view::by_application_callsite_frequency_view))
    {
        dump_mini_dump_heap_statistics_view(heaps.peb(), statistics.view_by_application_callsite_frequency(), options, is_x86_target, hex_length);
        wcout << '\n';
    }
}

