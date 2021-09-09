﻿#include "large_alloc_entry.h"

#include "common_symbol_names.h"
#include "segment_heap.h"
#include "segment_heap_utils.h"
#include "stream_utils.h"

namespace dlg_help_utils::heap
{
    std::wstring const& large_alloc_entry::symbol_name = common_symbol_names::heap_large_alloc_structure_symbol_name;

    large_alloc_entry::large_alloc_entry(segment_heap const& heap, uint64_t const large_alloc_entry_address)
    : heap_{heap}
    , large_alloc_entry_address_{large_alloc_entry_address}
    , heap_large_alloc_symbol_type_{stream_utils::get_type(walker(), symbol_name)}
    , heap_large_alloc_length_{stream_utils::get_type_length(heap_large_alloc_symbol_type_, symbol_name)}
    , size_{get_size()}
    , ust_address_{get_ust_address()}
    , allocation_stack_trace_{get_allocation_stack_trace()}
    {
    }

    stream_stack_dump::mini_dump_stack_walk const& large_alloc_entry::walker() const
    {
        return heap().walker();
    }

    process::process_environment_block const& large_alloc_entry::peb() const
    {
        return heap().peb();
    }

    uint64_t large_alloc_entry::virtual_address() const
    {
        return stream_utils::get_machine_size_field_value(*this, common_symbol_names::heap_large_alloc_virtual_address_field_symbol_name);
    }

    size_units::base_10::bytes large_alloc_entry::unused_bytes() const
    {
        return size_units::base_10::bytes{stream_utils::get_bit_field_value<uint64_t>(*this, common_symbol_names::heap_large_alloc_unused_bytes_field_symbol_name)};
    }

    bool large_alloc_entry::extra_present() const
    {
        return stream_utils::get_bit_field_value<uint64_t>(*this, common_symbol_names::heap_large_alloc_extra_present_field_symbol_name) != 0x0;
    }

    uint32_t large_alloc_entry::guard_page_count() const
    {
        return static_cast<uint32_t>(stream_utils::get_bit_field_value<uint64_t>(*this, common_symbol_names::heap_large_alloc_guard_page_count_field_symbol_name));
    }

    uint32_t large_alloc_entry::guard_page_alignment() const
    {
        return static_cast<uint32_t>(stream_utils::get_bit_field_value<uint64_t>(*this, common_symbol_names::heap_large_alloc_guard_page_alignment_field_symbol_name));
    }

    uint32_t large_alloc_entry::spare() const
    {
        return static_cast<uint32_t>(stream_utils::get_bit_field_value<uint64_t>(*this, common_symbol_names::heap_large_alloc_spare_field_symbol_name));
    }

    uint64_t large_alloc_entry::allocated_pages() const
    {
        return stream_utils::get_bit_field_value<uint64_t>(*this, common_symbol_names::heap_large_alloc_allocated_pages_field_symbol_name);
    }

    uint64_t large_alloc_entry::block_address() const
    {
        return virtual_address() - unused_bytes().count();
    }

    uint64_t large_alloc_entry::block_size() const
    {
        return size().count();
    }

    uint64_t large_alloc_entry::user_address() const
    {
        return block_address() + read_front_padding();
    }

    size_units::base_10::bytes large_alloc_entry::user_requested_size() const
    {
        return size_units::base_10::bytes{block_size() - unused_bytes().count() - read_front_padding()};
    }

    size_units::base_10::bytes large_alloc_entry::get_size() const
    {
        return size_units::base_10::bytes{allocated_pages() * heap().peb().page_size()};
    }

    uint64_t large_alloc_entry::read_front_padding() const
    {
        return segment_heap_utils::read_front_padding_size(peb(), block_address());
    }

    uint64_t large_alloc_entry::get_ust_address() const
    {
        if(!peb().user_stack_db_enabled() || !extra_present())
        {
            return 0;
        }

        return segment_heap_utils::read_ust_address(peb(), block_address(), block_size(), unused_bytes().count());
    }

    std::vector<uint64_t> large_alloc_entry::get_allocation_stack_trace() const
    {
        return heap().stack_trace().read_allocation_stack_trace(peb(), ust_address());
    }
}