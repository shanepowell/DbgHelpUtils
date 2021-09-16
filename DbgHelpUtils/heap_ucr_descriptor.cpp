#include "heap_ucr_descriptor.h"

#include "common_symbol_names.h"
#include "nt_heap.h"
#include "stream_utils.h"

namespace dlg_help_utils::heap
{
    std::wstring const& heap_ucr_descriptor::symbol_name = common_symbol_names::heap_ucr_descriptor_structure_symbol_name;

    heap_ucr_descriptor::heap_ucr_descriptor(nt_heap const& heap, uint64_t const heap_ucr_descriptor_address)
    : heap_{heap}
    , heap_ucr_descriptor_address_{heap_ucr_descriptor_address}
    , heap_ucr_descriptor_symbol_type_{stream_utils::get_type(walker(), symbol_name)}
    {
    }

    stream_stack_dump::mini_dump_stack_walk const& heap_ucr_descriptor::walker() const
    {
        return heap().walker();
    }

    process::process_environment_block const& heap_ucr_descriptor::peb() const
    {
        return heap().peb();
    }

    uint64_t heap_ucr_descriptor::address() const
    {
        return stream_utils::get_field_pointer(walker(), heap_ucr_descriptor_address_, heap_ucr_descriptor_symbol_type_, symbol_name, common_symbol_names::heap_ucr_descriptor_address_field_symbol_name);
    }

    size_units::base_16::bytes heap_ucr_descriptor::size() const
    {
        return size_units::base_16::bytes {stream_utils::get_machine_size_field_value(*this, common_symbol_names::heap_ucr_descriptor_size_field_symbol_name) };
    }
}
