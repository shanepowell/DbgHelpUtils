#include "heap_ucr_descriptor.h"

#include "common_symbol_names.h"
#include "nt_heap.h"
#include "stream_utils.h"

namespace dlg_help_utils::heap
{
    heap_ucr_descriptor::heap_ucr_descriptor(nt_heap const& heap, uint64_t const heap_ucr_descriptor_address)
    : heap_{heap}
    , heap_ucr_descriptor_address_{heap_ucr_descriptor_address}
    , heap_ucr_descriptor_symbol_type_{stream_utils::get_type(heap_.walker(), common_symbol_names::heap_ucr_descriptor_structure_symbol_name)}
    {
    }

    uint64_t heap_ucr_descriptor::get_machine_size_field_value(std::wstring const& field_name) const
    {
        auto const value = stream_utils::find_machine_size_field_value(heap_.peb(), heap_ucr_descriptor_symbol_type_, field_name, heap_ucr_descriptor_address_);
        if(!value.has_value())
        {
            throw_cant_get_field_data(field_name);
        }

        return value.value();
    }

    uint64_t heap_ucr_descriptor::address() const
    {
        return stream_utils::get_field_pointer(heap_.walker(), heap_ucr_descriptor_address_, heap_ucr_descriptor_symbol_type_, common_symbol_names::heap_ucr_descriptor_structure_symbol_name, common_symbol_names::heap_ucr_descriptor_address_field_symbol_name);
    }

    size_units::base_10::bytes heap_ucr_descriptor::size() const
    {
        return size_units::base_10::bytes { get_machine_size_field_value(common_symbol_names::heap_ucr_descriptor_size_field_symbol_name) };
    }

    void heap_ucr_descriptor::throw_cant_get_field_data(std::wstring const& field_name)
    {
        stream_utils::throw_cant_get_field_is_null(common_symbol_names::heap_ucr_descriptor_structure_symbol_name, field_name);
    }
}
