#include "dph_entry.h"

#include "cache_manager.h"
#include "common_symbol_names.h"
#include "dph_heap.h"
#include "stream_utils.h"
#include "ust_address_stack_trace.h"

namespace dlg_help_utils::heap
{
    std::wstring const& dph_entry::symbol_name = common_symbol_names::dph_heap_block_structure_symbol_name;
    std::wstring const& dph_entry::block_info_symbol_name = common_symbol_names::dph_block_information_structure_symbol_name;

    dph_entry::dph_entry(dph_heap const& heap, uint64_t const entry_address, is_virtual_allocation const is_virtual_allocation)
    : cache_data_{&heap.cache().get_cache<cache_data>()}
    , heap_{&heap}
    , entry_address_{entry_address}
    , is_virtual_allocation_{is_virtual_allocation}
    {
    }

    stream_stack_dump::mini_dump_memory_walker const& dph_entry::walker() const
    {
        return heap().walker();
    }

    process::process_environment_block const& dph_entry::peb() const
    {
        return heap().peb();
    }

    void dph_entry::setup_globals(dph_heap const& heap)
    {
        if(!heap.cache().has_cache<cache_data>())
        {
            auto& data = heap.cache().get_cache<cache_data>();
            data.dph_heap_block_symbol_type = stream_utils::get_type(heap.walker(), symbol_name);
            data.dph_heap_block_virtual_block_size_field_data = stream_utils::get_field_type_and_offset_in_type(data.dph_heap_block_symbol_type, symbol_name, common_symbol_names::dph_heap_block_virtual_block_size_field_symbol_name, dbg_help::sym_tag_enum::BaseType);
            data.dph_heap_block_user_requested_size_field_data = stream_utils::get_field_type_and_offset_in_type(data.dph_heap_block_symbol_type, symbol_name, common_symbol_names::dph_heap_block_user_requested_size_field_symbol_name, dbg_help::sym_tag_enum::BaseType);
            data.dph_heap_block_virtual_block_field_data = stream_utils::get_field_type_and_offset_in_type(data.dph_heap_block_symbol_type, symbol_name, common_symbol_names::dph_heap_block_virtual_block_field_symbol_name, dbg_help::sym_tag_enum::PointerType);
            data.dph_heap_block_user_allocation_field_data = stream_utils::get_field_type_and_offset_in_type(data.dph_heap_block_symbol_type, symbol_name, common_symbol_names::dph_heap_block_user_allocation_field_symbol_name, dbg_help::sym_tag_enum::PointerType);
            data.dph_heap_block_stack_trace_field_data = stream_utils::get_field_type_and_offset_in_type(data.dph_heap_block_symbol_type, symbol_name, common_symbol_names::dph_heap_block_stack_trace_field_symbol_name, dbg_help::sym_tag_enum::PointerType);
            data.dph_heap_block_next_alloc_field_data = stream_utils::get_field_type_and_offset_in_type(data.dph_heap_block_symbol_type, symbol_name, common_symbol_names::dph_heap_block_next_alloc_field_symbol_name, dbg_help::sym_tag_enum::PointerType);

            data.dph_block_information_symbol_type = stream_utils::get_type(heap.walker(), block_info_symbol_name);
            data.dph_block_information_symbol_length = stream_utils::get_type_length(data.dph_block_information_symbol_type, block_info_symbol_name);
            data.dph_block_information_start_stamp_field_data = stream_utils::get_field_type_and_offset_in_type(data.dph_block_information_symbol_type, block_info_symbol_name, common_symbol_names::dph_block_information_start_stamp_field_symbol_name, dbg_help::sym_tag_enum::BaseType);
        }
    }

    uint64_t dph_entry::get_virtual_block_address() const
    {
        return get_field_pointer_raw(walker(), entry_address(), cache_data_->dph_heap_block_virtual_block_field_data, symbol_name, common_symbol_names::dph_heap_block_virtual_block_field_symbol_name);
    }

    size_units::base_16::bytes dph_entry::get_virtual_block_size() const
    {
        return size_units::base_16::bytes{get_machine_size_field_value(*this, cache_data_->dph_heap_block_virtual_block_size_field_data, common_symbol_names::dph_heap_block_virtual_block_size_field_symbol_name)};
    }

    uint64_t dph_entry::get_user_address() const
    {
        return get_field_pointer_raw(walker(), entry_address(), cache_data_->dph_heap_block_user_allocation_field_data, symbol_name, common_symbol_names::dph_heap_block_user_allocation_field_symbol_name);
    }

    size_units::base_16::bytes dph_entry::get_user_requested_size() const
    {
        return size_units::base_16::bytes{get_machine_size_field_value(*this, cache_data_->dph_heap_block_user_requested_size_field_data, common_symbol_names::dph_heap_block_user_requested_size_field_symbol_name)};
    }

    uint64_t dph_entry::get_ust_address() const
    {
        return get_field_pointer_raw(walker(), entry_address(), cache_data_->dph_heap_block_stack_trace_field_data, symbol_name, common_symbol_names::dph_heap_block_stack_trace_field_symbol_name);
    }

    uint64_t dph_entry::get_next_alloc_address() const
    {
        return get_field_pointer_raw(walker(), entry_address(), cache_data_->dph_heap_block_next_alloc_field_data, symbol_name, common_symbol_names::dph_heap_block_next_alloc_field_symbol_name);
    }

    bool dph_entry::get_is_allocated() const
    {
        if(user_address() == 0)
        {
            return false;
        }

        auto const value = stream_utils::find_basic_type_field_value_in_type<uint32_t>(walker(), cache_data_->dph_block_information_start_stamp_field_data, user_address() - cache_data_->dph_block_information_symbol_length);
        auto const start_stamp = value.value_or(0);
        return start_stamp == StampAllocFullPageMode || start_stamp == StampAllocNormalPageMode;
    }

    std::vector<uint64_t> dph_entry::get_allocation_stack_trace() const
    {
        return heap().stack_trace().read_allocation_stack_trace(peb(), ust_address());
    }
}
