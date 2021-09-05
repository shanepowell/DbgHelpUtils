#include "heap_lfh_affinity_slot.h"

#include "common_symbol_names.h"
#include "heap_lfh_context.h"
#include "heap_lfh_subsegment.h"
#include "list_entry_walker.h"
#include "stream_utils.h"

namespace dlg_help_utils::heap
{
    std::wstring const& heap_lfh_affinity_slot::symbol_name = common_symbol_names::heap_lfh_affinity_slot_structure_symbol_name;

    heap_lfh_affinity_slot::heap_lfh_affinity_slot(heap_lfh_context const& heap, uint64_t const heap_lfh_affinity_slot_address)
    : heap_{heap}
    , heap_lfh_affinity_slot_address_{heap_lfh_affinity_slot_address}
    , heap_lfh_affinity_slot_symbol_type_{stream_utils::get_type(walker(), symbol_name)}
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
        return stream_utils::get_field_value<uint8_t>(*this, common_symbol_names::heap_lfh_affinity_slot_bucket_index_field_symbol_name);
    }

    size_t heap_lfh_affinity_slot::slot_index() const
    {
        return stream_utils::get_field_value<uint8_t>(*this, common_symbol_names::heap_lfh_affinity_slot_slot_index_field_symbol_name);
    }

    uint64_t heap_lfh_affinity_slot::available_subsegment_count() const
    {
        return stream_utils::get_machine_size_field_value(*this, common_symbol_names::heap_lfh_affinity_slot_available_subsegment_count_field_symbol_name);
    }

    std::experimental::generator<heap_lfh_subsegment> heap_lfh_affinity_slot::subsegments() const
    {
        // enumerate available subsegment list
        for (ntdll_utilities::list_entry_walker const list_walker{walker(), stream_utils::get_field_address(*this, common_symbol_names::heap_lfh_affinity_slot_available_subsegment_list_field_symbol_name), heap_lfh_subsegment::symbol_name, common_symbol_names::heap_lfh_subsegment_list_entry_field_symbol_name}; 
            auto const entry_address : list_walker.entries())
        {
            co_yield heap_lfh_subsegment{heap(), entry_address};
        }

        // enumerate full subsegment list
        for (ntdll_utilities::list_entry_walker const list_walker{walker(), stream_utils::get_field_address(*this, common_symbol_names::heap_lfh_affinity_slot_full_subsegment_list_field_symbol_name), heap_lfh_subsegment::symbol_name, common_symbol_names::heap_lfh_subsegment_list_entry_field_symbol_name}; 
            auto const entry_address : list_walker.entries())
        {
            co_yield heap_lfh_subsegment{heap(), entry_address};
        }
    }
}
