﻿#include "dph_entry.h"

#include "common_symbol_names.h"
#include "dph_heap.h"
#include "stream_utils.h"
#include "ust_address_stack_trace.h"

namespace dlg_help_utils::heap
{
    std::wstring const& dph_entry::symbol_name = common_symbol_names::dph_heap_block_structure_symbol_name;
    std::wstring const& dph_entry::block_info_symbol_name = common_symbol_names::dph_block_information_structure_symbol_name;

    dph_entry::dph_entry(dph_heap const& heap, uint64_t const entry_address)
    : heap_{heap}
    , dph_heap_block_symbol_type_{stream_utils::get_type(walker(), symbol_name)}
    , dph_block_information_symbol_type_{stream_utils::get_type(walker(), block_info_symbol_name)}
    , dph_block_information_symbol_length_{stream_utils::get_type_length(dph_block_information_symbol_type_, block_info_symbol_name)}
    , entry_address_{entry_address}
    {
    }

    stream_stack_dump::mini_dump_stack_walk const& dph_entry::walker() const
    {
        return heap().walker();
    }

    process::process_environment_block const& dph_entry::peb() const
    {
        return heap().peb();
    }

    uint64_t dph_entry::get_virtual_block_address() const
    {
        return stream_utils::get_field_pointer_raw(walker(), entry_address(), dph_heap_block_symbol_type_, symbol_name, common_symbol_names::dph_heap_block_virtual_block_field_symbol_name);
    }

    size_units::base_16::bytes dph_entry::get_virtual_block_size() const
    {
        return size_units::base_16::bytes{stream_utils::get_machine_size_field_value(*this, common_symbol_names::dph_heap_block_virtual_block_size_field_symbol_name)};
    }

    uint64_t dph_entry::get_user_address() const
    {
        return stream_utils::get_field_pointer_raw(walker(), entry_address(), dph_heap_block_symbol_type_, symbol_name, common_symbol_names::dph_heap_block_user_allocation_field_symbol_name);
    }

    size_units::base_16::bytes dph_entry::get_user_requested_size() const
    {
        return size_units::base_16::bytes{stream_utils::get_machine_size_field_value(*this, common_symbol_names::dph_heap_block_user_requested_size_field_symbol_name)};
    }

    uint64_t dph_entry::get_ust_address() const
    {
        return stream_utils::get_field_pointer_raw(walker(), entry_address(), dph_heap_block_symbol_type_, symbol_name, common_symbol_names::dph_heap_block_stack_trace_field_symbol_name);
    }

    uint64_t dph_entry::get_next_alloc_address() const
    {
        return stream_utils::get_field_pointer_raw(walker(), entry_address(), dph_heap_block_symbol_type_, symbol_name, common_symbol_names::dph_heap_block_next_alloc_field_symbol_name);
    }

    bool dph_entry::get_is_allocated() const
    {
        if(user_address() == 0)
        {
            return false;
        }

        auto const value = stream_utils::find_basic_type_field_value_in_type<uint32_t>(walker(), dph_block_information_symbol_type_, common_symbol_names::dph_block_information_start_stamp_field_symbol_name, user_address() - dph_block_information_symbol_length_);
        auto const start_stamp = value.value_or(0);
        return start_stamp == StampAllocFullPageMode || start_stamp == StampAllocNormalPageMode;
    }

    std::vector<uint64_t> dph_entry::get_allocation_stack_trace() const
    {
        return heap_.stack_trace().read_allocation_stack_trace(peb(), ust_address());
    }
}
