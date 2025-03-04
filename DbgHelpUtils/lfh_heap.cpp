#include "lfh_heap.h"

#include "cache_manager.h"
#include "common_symbol_names.h"
#include "heap_subsegment.h"
#include "lfh_segment.h"
#include "list_entry_walker.h"
#include "nt_heap.h"
#include "stream_utils.h"

namespace dlg_help_utils::heap
{
    std::wstring const& lfh_heap::symbol_name = common_symbol_names::lfh_heap_structure_symbol_name;

    lfh_heap::lfh_heap(nt_heap const& heap, uint64_t const lfh_heap_address)
    : cache_data_{&heap.cache().get_cache<cache_data>()}
    , heap_{&heap}
    , lfh_heap_address_{lfh_heap_address}
    {
    }

    stream_stack_dump::mini_dump_memory_walker const& lfh_heap::walker() const
    {
        return heap().walker();
    }

    process::process_environment_block const& lfh_heap::peb() const
    {
        return heap().peb();
    }

    generator<lfh_segment> lfh_heap::lfh_segments() const
    {
        for (ntdll_utilities::list_entry_walker const list_walker{heap().cache(), walker(), stream_utils::get_field_address(*this, cache_data_->lfh_heap_sub_segment_zones_field_data, common_symbol_names::lfh_heap_sub_segment_zones_field_symbol_name), lfh_segment::symbol_name, common_symbol_names::lfh_block_zone_list_entry_field_symbol_name}; 
            auto const entry_address : list_walker.entries())
        {
            co_yield lfh_segment{*this, entry_address};
        }
    }

    void lfh_heap::setup_globals(nt_heap const& heap)
    {
        if(!heap.cache().has_cache<cache_data>())
        {
            auto& data = heap.cache().get_cache<cache_data>();
            data.lfh_heap_symbol_type = stream_utils::get_type(heap.walker(), symbol_name);
            data.lfh_key = stream_utils::read_static_variable_machine_size_value(heap.walker(), common_symbol_names::rtl_p_lfh_key_global_symbol_name);

            data.lfh_heap_sub_segment_zones_field_data = data.lfh_heap_symbol_type.get_field_in_type(symbol_name, common_symbol_names::lfh_heap_sub_segment_zones_field_symbol_name);
        }
    }
}
