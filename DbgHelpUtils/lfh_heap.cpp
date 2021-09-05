#include "lfh_heap.h"

#include "common_symbol_names.h"
#include "lfh_segment.h"
#include "list_entry_walker.h"
#include "nt_heap.h"
#include "stream_utils.h"

namespace dlg_help_utils::heap
{
    std::wstring const& lfh_heap::symbol_name = common_symbol_names::lfh_heap_structure_symbol_name;

    lfh_heap::lfh_heap(nt_heap const& heap, uint64_t const lfh_heap_address)
    : heap_{heap}
    , lfh_heap_address_{lfh_heap_address}
    , lfh_heap_symbol_type_{stream_utils::get_type(walker(), symbol_name)}
    {
    }

    stream_stack_dump::mini_dump_stack_walk const& lfh_heap::walker() const
    {
        return heap().walker();
    }

    process::process_environment_block const& lfh_heap::peb() const
    {
        return heap().peb();
    }

    std::experimental::generator<lfh_segment> lfh_heap::lfh_segments() const
    {
        for (ntdll_utilities::list_entry_walker const list_walker{walker(), stream_utils::get_field_address(*this, common_symbol_names::lfh_heap_sub_segment_zones_field_symbol_name), lfh_segment::symbol_name, common_symbol_names::lfh_block_zone_list_entry_field_symbol_name}; 
            auto const entry_address : list_walker.entries())
        {
            co_yield lfh_segment{*this, entry_address};
        }
    }

    std::optional<uint64_t> lfh_heap::get_lfh_key() const
    {
        return stream_utils::read_static_variable_machine_size_value(walker(), common_symbol_names::rtl_p_lfh_key_global_symbol_name);
    }
}
