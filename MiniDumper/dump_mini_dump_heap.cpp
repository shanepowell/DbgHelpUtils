#include "dump_mini_dump_heap.h"

#include <iostream>

#include "common_symbol_lookup_utils.h"
#include "dump_file_options.h"
#include "DbgHelpUtils/common_symbol_names.h"
#include "DbgHelpUtils/common_symbol_utils.h"
#include "DbgHelpUtils/function_table_stream.h"
#include "DbgHelpUtils/heap_entry.h"
#include "DbgHelpUtils/heap_segment.h"
#include "DbgHelpUtils/heap_subsegment.h"
#include "DbgHelpUtils/heap_ucr_descriptor.h"
#include "DbgHelpUtils/heap_virtual_block.h"
#include "DbgHelpUtils/hex_dump.h"
#include "DbgHelpUtils/lfh_heap.h"
#include "DbgHelpUtils/lfh_segment.h"
#include "DbgHelpUtils/memory64_list_stream.h"
#include "DbgHelpUtils/memory_list_stream.h"
#include "DbgHelpUtils/mini_dump_stack_walk.h"
#include "DbgHelpUtils/module_list_stream.h"
#include "DbgHelpUtils/nt_heap.h"
#include "DbgHelpUtils/pe_file_memory_mapping.h"
#include "DbgHelpUtils/size_units.h"
#include "DbgHelpUtils/stream_hex_dump.h"
#include "DbgHelpUtils/stream_stack_dump.h"
#include "DbgHelpUtils/stream_utils.h"
#include "DbgHelpUtils/thread_names_list_stream.h"
#include "DbgHelpUtils/unloaded_module_list_stream.h"

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
        using namespace size_units::base_10;
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
        wcout << process_heap_marker << '\n';
    }

    void print_nt_heap_segment_line(std::streamsize const hex_length, size_t const segment_index, heap::heap_segment const& segment, std::wstring const& indent_str)
    {
        auto const first_entry = segment.first_entry();
        auto const last_entry = segment.last_entry();
        using namespace size_units::base_10;
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
        using namespace size_units::base_10;
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
        using namespace size_units::base_10;
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

    void print_dump_entry(std::streamsize const hex_length, heap::heap_entry const& entry, bool const entry_contains_lfh_subsegments, dump_file_options const& options, size_t const indent)
    {
        using namespace size_units::base_10;
        wcout << std::wstring(indent, L' ') << stream_hex_dump::to_hex(entry.address(), hex_length) << " sz(" << stream_hex_dump::to_hex(entry.size().count()) << " - " << entry.size() << ")";
        if (entry.is_uncommitted())
        {
            wcout << " sz(" << entry.size() << ")";
            wcout << " - uncommitted\n";
        }
        else if (entry.is_unknown())
        {
            wcout << " sz(" << entry.size() << ")";
            wcout << " - unknown range\n";

            if(options.hex_dump_memory_data())
            {
                uint64_t size = entry.size().count();
                auto const* data = entry.heap().walker().get_process_memory_range(entry.address(), size);
                hex_dump::hex_dump(wcout, data, size, indent + 2);
                wcout << L'\n';
            }
        }
        else
        {
            if (entry.is_busy())
            {
                wcout << " req(" << stream_hex_dump::to_hex(entry.requested_size().count()) << " - " << entry.requested_size() << ")";
                wcout << " u(" << entry.unused_bytes() << ")";
                if (entry.end_unused_bytes() > bytes{ 0 })
                {
                    wcout << " ue(" << entry.end_unused_bytes() << ")";
                }
                if (entry.user_address() != 0)
                {
                    wcout << " UsrPtr(" << stream_hex_dump::to_hex(entry.user_address(), hex_length) << ")";
                }
                if (entry.ust_address() != 0)
                {
                    wcout << " UstPtr(" << stream_hex_dump::to_hex(entry.ust_address(), hex_length) << ")";
                }
            }

            if (entry.is_lfh_entry())
            {
                wcout << (entry.is_lfh_busy() ? " busy" : " free");
            }
            else
            {
                wcout << " f(" << stream_hex_dump::to_hex(entry.flags()) << ")";

                if ((entry.flags() & heap::heap_entry::FlagBusy) == heap::heap_entry::FlagBusy)
                {
                    wcout << ", busy";
                }
                else
                {
                    wcout << ", free";
                }
                if ((entry.flags() & heap::heap_entry::FlagVirtualAlloc) == heap::heap_entry::FlagVirtualAlloc)
                {
                    wcout << ", internal";
                }
                if ((entry.flags() & heap::heap_entry::FlagExtraPresent) == heap::heap_entry::FlagExtraPresent)
                {
                    wcout << ", tail fill";
                }
                if ((entry.flags() & heap::heap_entry::FlagFillPattern) == heap::heap_entry::FlagFillPattern)
                {
                    wcout << ", fill";
                }
                if ((entry.flags() & heap::heap_entry::FlagLastEntry) == heap::heap_entry::FlagLastEntry)
                {
                    wcout << ", last";
                }
                if ((entry.flags() & heap::heap_entry::FlagUserMask) != 0)
                {
                    wcout << ", user flag " << static_cast<uint16_t>(entry.flags() >> heap::heap_entry::FlagUserBitOffset);
                }

                if (entry.is_front_padded())
                {
                    wcout << ", front padded";
                }

                if (!entry.is_valid())
                {
                    wcout << ", invalid checksum";
                }
            }

            wcout << '\n';

            if(options.display_symbols() && entry.is_busy() && !entry.allocation_stack_trace().empty())
            {
                wcout << std::wstring(indent + 2, L' ') << "allocation stack trace:\n";
                hex_dump_stack(wcout, entry.heap().walker(), entry.allocation_stack_trace(), entry.heap().is_x86_target(), indent + 2);
                wcout << '\n';
            }

            if(options.hex_dump_memory_data() && entry.is_busy() && entry.user_address() != 0 && ((entry.flags() & heap::heap_entry::FlagVirtualAlloc) != heap::heap_entry::FlagVirtualAlloc || !entry_contains_lfh_subsegments))
            {
                uint64_t size = entry.requested_size().count();
                auto const* data = entry.heap().walker().get_process_memory_range(entry.user_address(), size);
                hex_dump::hex_dump(wcout, data, size, indent + 2);
                wcout << L'\n';
            }
        }
    }

    void print_lfh_heap_segment(std::streamsize const hex_length, LfhSubsegmentData& data, dump_file_options const& options, size_t const indent)
    {
        using namespace size_units::base_10;
        std::wstring const indent_str(indent, L' ');

        data.printed_subsegment = true;
        bytes const total{ data.subsegment.address() ? data.subsegment.block_count() * data.subsegment.block_stride() : 0 };
        wcout << indent_str << "LFH Segment " << data.segment->segment_index << " Subsegment " << stream_hex_dump::to_hex(data.subsegment.address(), hex_length) << ", block count " << data.subsegment.block_count() << ", block size " << bytes{ data.subsegment.block_size() } << ", entry data " << stream_hex_dump::to_hex(data.subsegment.entry_start_address(), hex_length) << ", total " << total << '\n';

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
        using namespace size_units::base_10;
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

    char const* get_process_marker(std::optional<uint64_t> const process_heap, std::optional<std::pair<dbg_help::symbol_type_info, uint64_t>> const heap)
    {
        return heap.value().second == process_heap.value() ? " (process heap)" : "";
    }

    void print_nt_heap_uncommitted_ranges(heap::nt_heap const& nt_heap, size_t const indent)
    {
        using namespace size_units::base_10;
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
        using namespace size_units::base_10;
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
}

void dump_mini_dump_heap(mini_dump const& mini_dump, dump_file_options const& options, dbg_help::symbol_engine& symbol_engine)
{
    thread_names_list_stream const names_list{mini_dump};
    memory_list_stream const memory_list{mini_dump};
    memory64_list_stream const memory64_list{mini_dump};
    function_table_stream const function_table{ mini_dump };
    module_list_stream const module_list{ mini_dump };
    unloaded_module_list_stream const unloaded_module_list{ mini_dump };
    pe_file_memory_mapping pe_file_memory_mappings{};
    stream_stack_dump::mini_dump_stack_walk const walker{
        0, nullptr, 0, memory_list, memory64_list, function_table, module_list,
        unloaded_module_list, pe_file_memory_mappings, symbol_engine
    };

    // Find TEB address
    auto const teb_address = common_symbol_utils::get_teb_address(mini_dump, names_list, memory_list, memory64_list);
    if(!teb_address.has_value())
    {
        wcout << "No TEB address found\n";
        return;
    }

    auto const heap_symbol_info = walker.get_type_info(common_symbol_names::heap_structure_symbol_name);
    if(!heap_symbol_info.has_value())
    {
        wcout << "No _HEAP symbol found\n";
        return;
    }

    auto const peb_address = find_field_pointer(walker, common_symbol_names::teb_structure_symbol_name, teb_address.value(), common_symbol_names::teb_structure_process_environment_block_field_symbol_name);
    if(!peb_address.has_value())
    {
        return;
    }

    auto const peb_symbol_info = get_type_info(walker, common_symbol_names::peb_structure_symbol_name);
    if(!peb_symbol_info.has_value())
    {
        return;
    }

    auto const nt_global_flag = stream_utils::find_basic_type_field_value_in_type<uint32_t>(walker, peb_symbol_info.value(), common_symbol_names::peb_structure_nt_global_flag_field_symbol_name, peb_address.value());
    if(!nt_global_flag.has_value())
    {
        return;
    }

    auto const process_heaps = find_field_pointer_and_type(walker, peb_symbol_info.value(), common_symbol_names::peb_structure_symbol_name, peb_address.value(), common_symbol_names::peb_structure_process_heaps_field_symbol_name);
    if(!process_heaps.has_value())
    {
        return;
    }

    auto const number_of_heaps = stream_utils::find_basic_type_field_value_in_type<uint32_t>(walker, peb_symbol_info.value(), common_symbol_names::peb_structure_number_of_heaps_field_symbol_name, peb_address.value());
    if(!number_of_heaps.has_value())
    {
        return;
    }

    auto const process_heap = find_field_pointer(walker, peb_symbol_info.value(), common_symbol_names::peb_structure_symbol_name, peb_address.value(), common_symbol_names::peb_structure_process_heap_field_symbol_name);
    if(!process_heap.has_value())
    {
        return;
    }

    std::streamsize hex_length;
    if(auto const pointer_length = process_heaps.value().first.length(); pointer_length.has_value())
    {
        hex_length = static_cast<std::streamsize>(pointer_length.value()) * 2;
    }
    else
    {
        return;
    }

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
    for(uint32_t heap_index = 0; heap_index < number_of_heaps.value(); ++heap_index)
    {
        if(auto const heap = stream_utils::get_field_pointer_type_and_value(walker, process_heaps.value().first, process_heaps.value().second, heap_index); heap.has_value())
        {
            wcout << ' ' << stream_hex_dump::to_hex(heap.value().second, hex_length);
            auto const segment_signature = stream_utils::find_basic_type_field_value_in_type<uint32_t>(walker, heap_symbol_info.value(), common_symbol_names::heap_segment_signature_field_symbol_name, heap.value().second);
            if(!segment_signature.has_value())
            {
                return;
            }

            if(segment_signature.value() == heap::SegmentSignatureNtHeap)
            {

                heap::nt_heap nt_heap{walker, heap.value().second, static_cast<gflags_utils::gflags>(nt_global_flag.value())};
                print_nt_heap_line(get_process_marker(process_heap, heap), nt_heap);
                print_nt_heap_segments_list(hex_length, nt_heap, 2);
                print_nt_heap_lfh_segments_list(hex_length, nt_heap, 2);
            }
            else if(segment_signature.value() == heap::SegmentSignatureSegmentHeap)
            {
                wcout << " Segment Heap" << get_process_marker(process_heap, heap) << '\n';
            }
            else
            {
                wcout << " Unknown Heap Type (" << stream_hex_dump::to_hex(segment_signature.value()) << ")\n";
            }
        }
        else
        {
            wcout << "Failed to find heap index " << heap_index << "\n";
        }
    }

    wcout << '\n';

    for(uint32_t heap_index = 0; heap_index < number_of_heaps.value(); ++heap_index)
    {
        if(auto const heap = stream_utils::get_field_pointer_type_and_value(walker, process_heaps.value().first, process_heaps.value().second, heap_index); heap.has_value())
        {
            auto const segment_signature = stream_utils::find_basic_type_field_value_in_type<uint32_t>(walker, heap_symbol_info.value(), common_symbol_names::heap_segment_signature_field_symbol_name, heap.value().second);
            if(!segment_signature.has_value())
            {
                return;
            }

            if(segment_signature.value() == heap::SegmentSignatureNtHeap)
            {

                heap::nt_heap nt_heap{walker, heap.value().second, static_cast<gflags_utils::gflags>(nt_global_flag.value())};
                print_nt_heap(hex_length, get_process_marker(process_heap, heap), nt_heap, 0);

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

                print_nt_heap_uncommitted_ranges(nt_heap, 2);

                print_nt_heap_virtual_allocated_blocks(hex_length, nt_heap, options, 2);

                print_nt_heap_free_list(hex_length, nt_heap, options, 2);
            }
        }
    }

    wcout << '\n';
}

