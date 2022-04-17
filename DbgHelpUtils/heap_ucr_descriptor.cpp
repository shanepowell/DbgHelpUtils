#include "heap_ucr_descriptor.h"

#include "cache_manager.h"
#include "common_symbol_names.h"
#include "nt_heap.h"
#include "stream_utils.h"

namespace dlg_help_utils::heap
{
    std::wstring const& heap_ucr_descriptor::symbol_name = common_symbol_names::heap_ucr_descriptor_structure_symbol_name;

    heap_ucr_descriptor::heap_ucr_descriptor(nt_heap const& heap, uint64_t const heap_ucr_descriptor_address)
    : cache_data_{heap.cache().get_cache<cache_data>()}
    , heap_{heap}
    , heap_ucr_descriptor_address_{heap_ucr_descriptor_address}
    {
    }

    stream_stack_dump::mini_dump_memory_walker const& heap_ucr_descriptor::walker() const
    {
        return heap().walker();
    }

    process::process_environment_block const& heap_ucr_descriptor::peb() const
    {
        return heap().peb();
    }

    uint64_t heap_ucr_descriptor::address() const
    {
        return get_field_pointer(walker(), heap_ucr_descriptor_address_, cache_data_.heap_ucr_descriptor_address_field_data, symbol_name, common_symbol_names::heap_ucr_descriptor_address_field_symbol_name);
    }

    size_units::base_16::bytes heap_ucr_descriptor::size() const
    {
        return size_units::base_16::bytes {get_machine_size_field_value(*this, cache_data_.heap_ucr_descriptor_size_field_data, common_symbol_names::heap_ucr_descriptor_size_field_symbol_name) };
    }

    void heap_ucr_descriptor::setup_globals(nt_heap const& heap)
    {
        if(!heap.cache().has_cache<cache_data>())
        {
            auto& data = heap.cache().get_cache<cache_data>();
            data.heap_ucr_descriptor_symbol_type = stream_utils::get_type(heap.walker(), symbol_name);
            data.heap_ucr_descriptor_address_field_data = stream_utils::get_field_type_and_offset_in_type(data.heap_ucr_descriptor_symbol_type, symbol_name, common_symbol_names::heap_ucr_descriptor_address_field_symbol_name, dbg_help::sym_tag_enum::PointerType);
            data.heap_ucr_descriptor_size_field_data = stream_utils::get_field_type_and_offset_in_type(data.heap_ucr_descriptor_symbol_type, symbol_name, common_symbol_names::heap_ucr_descriptor_size_field_symbol_name, dbg_help::sym_tag_enum::BaseType);
        }
    }
}
