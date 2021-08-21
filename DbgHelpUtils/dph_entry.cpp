#include "dph_entry.h"

#include "common_symbol_names.h"
#include "dph_heap.h"
#include "stream_utils.h"
#include "ust_address_stack_trace.h"
#include "wide_runtime_error.h"

namespace dlg_help_utils::heap
{
    dph_entry::dph_entry(dph_heap const& heap, uint64_t const entry_address)
    : heap_{heap}
    , dph_heap_block_symbol_type_{stream_utils::get_type(heap_.walker(), common_symbol_names::dph_heap_block_structure_symbol_name)}
    , dph_block_information_symbol_type_{stream_utils::get_type(heap_.walker(), common_symbol_names::dph_block_information_structure_symbol_name)}
    , dph_block_information_symbol_length_{get_dph_block_information_symbol_length()}
    , entry_address_{entry_address}
    , virtual_block_address_{get_virtual_block_address()}
    , virtual_block_size_{get_virtual_block_size()}
    , user_address_{get_user_address()}
    , user_requested_size_{get_user_requested_size()}
    , ust_address_{get_ust_address()}
    , next_alloc_address_{get_next_alloc_address()}
    , allocated_{get_is_allocated()}
    , allocation_stack_trace_{get_allocation_stack_trace()}
    {
    }

    template <typename T>
    T dph_entry::get_field_value(std::wstring const& field_name) const
    {
        auto const value = stream_utils::find_basic_type_field_value_in_type<T>(heap_.walker(), dph_heap_block_symbol_type_, field_name, entry_address());
        if(!value.has_value())
        {
            stream_utils::throw_cant_get_field_data(common_symbol_names::dph_heap_block_structure_symbol_name, field_name);
        }

        return value.value();
    }

    uint64_t dph_entry::get_virtual_block_address() const
    {
        return stream_utils::get_field_pointer_raw(heap_.walker(), entry_address(), dph_heap_block_symbol_type_, common_symbol_names::dph_heap_block_structure_symbol_name, common_symbol_names::dph_heap_block_virtual_block_field_symbol_name);
    }

    size_units::base_10::bytes dph_entry::get_virtual_block_size() const
    {
        return size_units::base_10::bytes{get_machine_size_field_value(common_symbol_names::dph_heap_block_virtual_block_size_field_symbol_name)};
    }

    uint64_t dph_entry::get_user_address() const
    {
        return stream_utils::get_field_pointer_raw(heap_.walker(), entry_address(), dph_heap_block_symbol_type_, common_symbol_names::dph_heap_block_structure_symbol_name, common_symbol_names::dph_heap_block_user_allocation_field_symbol_name);
    }

    size_units::base_10::bytes dph_entry::get_user_requested_size() const
    {
        return size_units::base_10::bytes{get_machine_size_field_value(common_symbol_names::dph_heap_block_user_requested_size_field_symbol_name)};
    }

    uint64_t dph_entry::get_ust_address() const
    {
        return stream_utils::get_field_pointer_raw(heap_.walker(), entry_address(), dph_heap_block_symbol_type_, common_symbol_names::dph_heap_block_structure_symbol_name, common_symbol_names::dph_heap_block_stack_trace_field_symbol_name);
    }

    uint64_t dph_entry::get_next_alloc_address() const
    {
        return stream_utils::get_field_pointer_raw(heap_.walker(), entry_address(), dph_heap_block_symbol_type_, common_symbol_names::dph_heap_block_structure_symbol_name, common_symbol_names::dph_heap_block_next_alloc_field_symbol_name);
    }

    bool dph_entry::get_is_allocated() const
    {
        if(user_address() == 0)
        {
            return false;
        }

        auto const value = stream_utils::find_basic_type_field_value_in_type<uint32_t>(heap_.walker(), dph_block_information_symbol_type_, common_symbol_names::dph_block_information_start_stamp_field_symbol_name, user_address() - dph_block_information_symbol_length_);
        auto const start_stamp = value.value_or(0);
        return start_stamp == StampAllocFullPageMode || start_stamp == StampAllocNormalPageMode;
    }

    std::vector<uint64_t> dph_entry::get_allocation_stack_trace() const
    {
        return heap_.stack_trace().read_allocation_stack_trace(heap_.peb(), ust_address());
    }

    uint64_t dph_entry::get_machine_size_field_value(std::wstring const& field_name) const
    {
        auto const value = stream_utils::find_machine_size_field_value(heap_.peb(), dph_heap_block_symbol_type_, field_name, entry_address());
        if(!value.has_value())
        {
            stream_utils::throw_cant_get_field_data(common_symbol_names::dph_heap_block_structure_symbol_name, field_name);
        }

        return value.value();
    }

    uint64_t dph_entry::get_dph_block_information_symbol_length() const
    {
        if(auto const length = dph_block_information_symbol_type_.length(); length.has_value())
        {
            return length.value();
        }

        throw exceptions::wide_runtime_error{(std::wostringstream{} << "Error: symbol " << common_symbol_names::dph_block_information_structure_symbol_name << " can't get length data").str()};
    }
}
