#include "heap_lfh_affinity_slot.h"

#include "cache_manager.h"
#include "common_symbol_names.h"
#include "heap_lfh_context.h"
#include "heap_lfh_subsegment.h"
#include "list_entry_walker.h"
#include "segment_heap.h"
#include "stream_utils.h"

namespace dlg_help_utils::heap
{
    std::wstring const& heap_lfh_affinity_slot::symbol_name = common_symbol_names::heap_lfh_affinity_slot_structure_symbol_name;

    heap_lfh_affinity_slot::heap_lfh_affinity_slot(heap_lfh_context const& heap, uint64_t const heap_lfh_affinity_slot_address)
    : cache_data_{heap.heap().cache().get_cache<cache_data>()}
    , heap_{heap}
    , heap_lfh_affinity_slot_address_{heap_lfh_affinity_slot_address}
    {
    }

    stream_stack_dump::mini_dump_stack_walk const& heap_lfh_affinity_slot::walker() const
    {
        return heap().walker();
    }

    process::process_environment_block const& heap_lfh_affinity_slot::peb() const
    {
        return heap().peb();
    }

    size_t heap_lfh_affinity_slot::bucket_index() const
    {
        return stream_utils::get_field_value<uint8_t>(*this, cache_data_.heap_lfh_affinity_slot_bucket_index_field_data, common_symbol_names::heap_lfh_affinity_slot_bucket_index_field_symbol_name);
    }

    size_t heap_lfh_affinity_slot::slot_index() const
    {
        return stream_utils::get_field_value<uint8_t>(*this, cache_data_.heap_lfh_affinity_slot_slot_index_field_data, common_symbol_names::heap_lfh_affinity_slot_slot_index_field_symbol_name);
    }

    uint64_t heap_lfh_affinity_slot::available_subsegment_count() const
    {
        return stream_utils::get_machine_size_field_value(*this, cache_data_.heap_lfh_affinity_slot_available_subsegment_count_field_data, common_symbol_names::heap_lfh_affinity_slot_available_subsegment_count_field_symbol_name);
    }

    std::experimental::generator<heap_lfh_subsegment> heap_lfh_affinity_slot::subsegments() const
    {
        // enumerate available subsegment list
        for (ntdll_utilities::list_entry_walker const list_walker{heap().heap().cache(), walker(), stream_utils::get_field_address(*this, cache_data_.heap_lfh_affinity_slot_available_subsegment_list_field_data, common_symbol_names::heap_lfh_affinity_slot_available_subsegment_list_field_symbol_name), heap_lfh_subsegment::symbol_name, common_symbol_names::heap_lfh_subsegment_list_entry_field_symbol_name}; 
            auto const entry_address : list_walker.entries())
        {
            co_yield heap_lfh_subsegment{heap(), entry_address};
        }

        // enumerate full subsegment list
        for (ntdll_utilities::list_entry_walker const list_walker{heap().heap().cache(), walker(), stream_utils::get_field_address(*this, cache_data_.heap_lfh_affinity_slot_full_subsegment_list_field_data, common_symbol_names::heap_lfh_affinity_slot_full_subsegment_list_field_symbol_name), heap_lfh_subsegment::symbol_name, common_symbol_names::heap_lfh_subsegment_list_entry_field_symbol_name}; 
            auto const entry_address : list_walker.entries())
        {
            co_yield heap_lfh_subsegment{heap(), entry_address};
        }
    }

    void heap_lfh_affinity_slot::setup_globals(segment_heap const& heap)
    {
        if(!heap.cache().has_cache<cache_data>())
        {
            auto& data = heap.cache().get_cache<cache_data>();
            data.heap_lfh_affinity_slot_symbol_type = stream_utils::get_type(heap.walker(), symbol_name);
            data.heap_lfh_affinity_slot_bucket_index_field_data = stream_utils::find_field_type_and_offset_in_type(data.heap_lfh_affinity_slot_symbol_type, common_symbol_names::heap_lfh_affinity_slot_bucket_index_field_symbol_name, dbg_help::sym_tag_enum::BaseType);
            data.heap_lfh_affinity_slot_slot_index_field_data = stream_utils::find_field_type_and_offset_in_type(data.heap_lfh_affinity_slot_symbol_type, common_symbol_names::heap_lfh_affinity_slot_slot_index_field_symbol_name, dbg_help::sym_tag_enum::BaseType);
            data.heap_lfh_affinity_slot_available_subsegment_count_field_data = stream_utils::find_field_type_and_offset_in_type(data.heap_lfh_affinity_slot_symbol_type, common_symbol_names::heap_lfh_affinity_slot_available_subsegment_count_field_symbol_name, dbg_help::sym_tag_enum::BaseType);

            data.heap_lfh_affinity_slot_available_subsegment_list_field_data = data.heap_lfh_affinity_slot_symbol_type.find_field_in_type(common_symbol_names::heap_lfh_affinity_slot_available_subsegment_list_field_symbol_name);
            data.heap_lfh_affinity_slot_full_subsegment_list_field_data = data.heap_lfh_affinity_slot_symbol_type.find_field_in_type(common_symbol_names::heap_lfh_affinity_slot_full_subsegment_list_field_symbol_name);
        }
    }
}
