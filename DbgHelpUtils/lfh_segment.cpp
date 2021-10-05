#include "lfh_segment.h"

#include "common_symbol_names.h"
#include "heap_entry.h"
#include "heap_segment.h"
#include "heap_subsegment.h"
#include "lfh_heap.h"
#include "nt_heap.h"
#include "process_environment_block.h"
#include "process_heaps.h"
#include "stream_utils.h"

namespace dlg_help_utils::heap
{
    std::wstring const& lfh_segment::symbol_name = common_symbol_names::lfh_block_zone_structure_symbol_name;

    lfh_segment::lfh_segment(heap::lfh_heap const& heap, uint64_t const lfh_segment_address)
    : lfh_heap_{heap}
    , lfh_segment_address_{lfh_segment_address}
    , lfh_block_zone_symbol_type_{stream_utils::get_type(walker(), symbol_name)}
    , heap_subsegment_size_{stream_utils::get_type_length(stream_utils::get_type(walker(), heap_subsegment::symbol_name), heap_subsegment::symbol_name)}
    {
    }

    stream_stack_dump::mini_dump_stack_walk const& lfh_segment::walker() const
    {
        return lfh_heap().walker();
    }

    process::process_environment_block const& lfh_segment::peb() const
    {
        return lfh_heap().peb();
    }

    size_t lfh_segment::subsegments_count() const
    {
        auto [start_subsegment_address, end_subsegment_address] = get_subsegment_range();
        // ReSharper disable once CppRedundantCastExpression
        return static_cast<size_t>((end_subsegment_address - start_subsegment_address) / heap_subsegment_size_);
    }

    std::experimental::generator<heap_subsegment> lfh_segment::subsegments() const
    {
        // filter out non-valid lfh segments, segments that are not in a HEAP internal allocated entry
        std::vector<heap_entry> all_heap_entries;
        for (auto const& segment : lfh_heap().heap().segments())
        {
            for (auto const& entry : segment.entries())
            {
                all_heap_entries.emplace_back(entry);
            }
        }

        auto [subsegment_address, end_subsegment_address] = get_subsegment_range();

        while(subsegment_address + heap_subsegment_size_ <= end_subsegment_address)
        {
            if(heap_subsegment subsegment{lfh_heap_, subsegment_address, lfh_block_zone_size_};
                subsegment.entry_start_address() != 0 && std::ranges::any_of(all_heap_entries, [&subsegment](heap_entry const& entry) { return process_heaps::is_lfh_subsegment_in_entry(entry, subsegment); }))
            {
                co_yield subsegment;
            }

            subsegment_address += heap_subsegment_size_;
        }
    }

    std::pair<uint64_t, uint64_t> lfh_segment::get_subsegment_range() const
    {
        auto start_subsegment_address = lfh_segment_address_ + lfh_block_zone_size_;
        auto end_subsegment_address{start_subsegment_address};

        if(auto const next_index = stream_utils::find_basic_type_field_value_in_type<uint32_t>(walker(), lfh_block_zone_symbol_type_, common_symbol_names::lfh_block_zone_next_index_field_symbol_name, lfh_segment_address_); next_index.has_value())
        {
            if(lfh_heap_.heap().peb().is_x64_target())
            {
                end_subsegment_address = start_subsegment_address = lfh_segment_address_ + 0x20;
            }

            end_subsegment_address += heap_subsegment_size_ * next_index.value();
        }
        else if(auto const free_pointer = stream_utils::find_field_pointer_value_in_type(walker(), lfh_block_zone_symbol_type_, common_symbol_names::lfh_block_zone_free_pointer_field_symbol_name, lfh_segment_address_); free_pointer.has_value())
        {
            end_subsegment_address = free_pointer.value();
        }

        return std::make_pair(start_subsegment_address, end_subsegment_address);
    }

    size_t lfh_segment::get_lfh_block_zone_size() const
    {
        if(lfh_heap_.heap().peb().is_x86_target())
        {
            return 0x10;
        }

        return stream_utils::get_type_length(lfh_block_zone_symbol_type_, symbol_name);
    }
}
