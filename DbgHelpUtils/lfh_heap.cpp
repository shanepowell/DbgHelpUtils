#include "lfh_heap.h"

#include "common_symbol_names.h"
#include "lfh_segment.h"
#include "nt_heap.h"
#include "stream_utils.h"

namespace dlg_help_utils::heap
{
    lfh_heap::lfh_heap(nt_heap const& heap, uint64_t const lfh_heap_address)
    : heap_{heap}
    , dph_heap_address_{lfh_heap_address}
    , lfh_heap_symbol_type_{stream_utils::get_type(walker(), common_symbol_names::lfh_heap_structure_symbol_name)}
    , list_entry_symbol_type_{stream_utils::get_type(walker(), common_symbol_names::list_entry_structure_symbol_name)}
    , lfh_key_{get_lfh_key()}
    {
    }

    stream_stack_dump::mini_dump_stack_walk const& lfh_heap::walker() const
    {
        return heap().walker();
    }

    std::experimental::generator<lfh_segment> lfh_heap::lfh_segments() const
    {
        auto const start = get_field_address(common_symbol_names::lfh_heap_sub_segment_zones_field_symbol_name);
        auto flink = get_field_pointer(common_symbol_names::lfh_heap_sub_segment_zones_flink_field_symbol_name);

        while(flink != start)
        {
            co_yield lfh_segment{*this, flink};

            flink = stream_utils::get_field_pointer(walker(), flink, list_entry_symbol_type_, common_symbol_names::list_entry_structure_symbol_name, common_symbol_names::list_entry_flink_field_symbol_name);
        }
    }

    uint64_t lfh_heap::get_field_pointer(std::wstring const& field_name) const
    {
        return stream_utils::get_field_pointer(walker(), dph_heap_address_, lfh_heap_symbol_type_, common_symbol_names::lfh_heap_structure_symbol_name, field_name);
    }

    uint64_t lfh_heap::get_field_address(std::wstring const& field_name) const
    {
        return dph_heap_address_ + stream_utils::get_field_offset(lfh_heap_symbol_type_, common_symbol_names::lfh_heap_structure_symbol_name, field_name);
    }

    std::optional<uint64_t> lfh_heap::get_lfh_key() const
    {
        return stream_utils::read_static_variable_value(walker(), common_symbol_names::rt_lp_lfh_key_global_symbol_name);
    }
}
