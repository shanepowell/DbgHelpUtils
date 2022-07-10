#include "dump_file_options.h"
#include "dump_mini_dump_heap.h"
#include "DbgHelpUtils/heap_entry.h"
#include "DbgHelpUtils/heap_segment.h"
#include "DbgHelpUtils/heap_subsegment.h"
#include "DbgHelpUtils/heap_ucr_descriptor.h"
#include "DbgHelpUtils/heap_virtual_block.h"
#include "DbgHelpUtils/hex_dump.h"
#include "DbgHelpUtils/lfh_heap.h"
#include "DbgHelpUtils/lfh_segment.h"
#include "DbgHelpUtils/locale_number_formatting.h"
#include "DbgHelpUtils/nt_heap.h"
#include "DbgHelpUtils/process_environment_block.h"
#include "DbgHelpUtils/stream_hex_dump.h"

using namespace std;
using namespace dlg_help_utils;

namespace detail
{
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


        void print_nt_heap_segment_line(std::wostream& log, std::streamsize const hex_length, size_t const segment_index, heap::heap_segment const& segment, std::wstring const& indent_str)
        {
            auto const first_entry = segment.first_entry();
            auto const last_entry = segment.last_entry();
            using namespace size_units::base_16;
            auto const total_size_bytes = bytes{ last_entry - first_entry };
            log << std::format(L"{0}Segment {1} {2}-{3} {4}\n", indent_str, locale_formatting::to_wstring(segment_index), stream_hex_dump::to_hex(first_entry, hex_length), stream_hex_dump::to_hex(last_entry, hex_length), to_wstring(total_size_bytes));
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

        void print_nt_heap_header(std::wostream& log, std::streamsize const hex_length, std::wstring_view const& process_heap_marker, heap::nt_heap const& nt_heap, size_t const indent)
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

        using entry_contains_lfh_subsegments_t = tagged_bool<struct entry_contains_lfh_subsegments_type>;

        void print_dump_entry(std::wostream& log
            , std::streamsize const hex_length
            , heap::heap_entry const& entry
            , entry_contains_lfh_subsegments_t const entry_contains_lfh_subsegments
            , dump_file_options const& options
            , size_t const indent)
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
                    hex_dump_stack(log, entry.walker(), entry.allocation_stack_trace(), stream_stack_dump::is_x86_target_t{entry.peb().is_x86_target()}, indent + 2);
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
                print_dump_entry(log, hex_length, entry, entry_contains_lfh_subsegments_t{false}, options, indent + 2);
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
                auto const entry_contains_lfh_subsegments = entry_contains_lfh_subsegments_t{ranges::any_of(lfh_data, [&entry](LfhSubsegmentData const& data) { return is_lfh_subsegment_in_entry(entry, data); })};
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
                    print_dump_entry(log, hex_length, entry, entry_contains_lfh_subsegments_t{false}, options, indent + 4);
                }
            }
            log << L'\n';
        }

        void print_nt_heap_free_list(std::wostream& log, std::streamsize const hex_length, heap::nt_heap const& nt_heap, dump_file_options const& options, size_t const indent)
        {
            log << std::format(L"{0:{1}}Free List Entries\n", ' ', indent);
            for (auto const& free_entry : nt_heap.free_entries())
            {
                print_dump_entry(log, hex_length, free_entry, entry_contains_lfh_subsegments_t{false}, options, indent + 4);
            }
            log << L'\n';
        }
    }

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
            , (nt_heap.debug_page_heap_available() ? L" (DPH)"sv : L""sv)
            , process_heap_marker
        );
    }

    void print_nt_heap(std::wostream& log, std::streamsize const hex_length, dump_file_options const& options, heap::nt_heap const& nt_heap, process::process_environment_block const& peb)
    {
        print_nt_heap_header(log, hex_length, get_process_marker(is_process_heap_t{nt_heap.is_process_heap(peb.process_heap())}), nt_heap, 0);

        vector<LfhSubsegmentData> lfh_data;
        if(auto const lfh_heap = nt_heap.lfh_heap(); lfh_heap.has_value())
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

        for(size_t segment_index = 0; auto const& segment : nt_heap.segments())
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

                for(bool found = false; auto const& segment : nt_heap.segments())
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

        print_nt_heap_uncommitted_ranges(log, nt_heap, 2);

        print_nt_heap_virtual_allocated_blocks(log, hex_length, nt_heap, options, 2);

        print_nt_heap_free_list(log, hex_length, nt_heap, options, 2);
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
}