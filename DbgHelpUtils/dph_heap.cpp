#include "dph_heap.h"

#include "cache_manager.h"
#include "common_symbol_names.h"
#include "dph_entry.h"
#include "list_entry_walker.h"
#include "process_environment_block.h"
#include "rtl_balanced_links_walker.h"
#include "stream_utils.h"

namespace dlg_help_utils::heap
{
    std::wstring const& dph_heap::symbol_name = common_symbol_names::dph_heap_root_structure_symbol_name;

    dph_heap::dph_heap(cache_manager& cache, process::process_environment_block const& peb, uint64_t const dph_heap_address)
    : cache_manager_{&cache}
    , peb_{&peb}
    , dph_heap_address_{dph_heap_address}
    {
    }

    stream_stack_dump::mini_dump_memory_walker const& dph_heap::walker() const
    {
        return peb().walker();
    }

    uint64_t dph_heap::normal_heap() const
    {
        return get_field_pointer(walker(), dph_heap_address_, cache_data_->dph_heap_root_normal_heap_field_data, symbol_name, common_symbol_names::dph_heap_root_normal_heap_field_symbol_name);
    }

    uint32_t dph_heap::flags() const
    {
        return stream_utils::get_field_value<uint32_t>(*this, cache_data_->dph_heap_root_flags_field_data, common_symbol_names::dph_heap_root_flags_field_symbol_name);
    }

    uint32_t dph_heap::extra_flags() const
    {
        return stream_utils::get_field_value<uint32_t>(*this, cache_data_->dph_heap_root_extra_flags_field_data, common_symbol_names::dph_heap_root_extra_flags_field_symbol_name);
    }

    uint32_t dph_heap::seed() const
    {
        return stream_utils::get_field_value<uint32_t>(*this, cache_data_->dph_heap_root_seed_field_data, common_symbol_names::dph_heap_root_seed_field_symbol_name);
    }

    uint32_t dph_heap::busy_allocations() const
    {
        return stream_utils::get_field_value<uint32_t>(*this, cache_data_->dph_heap_root_busy_allocations_field_data, common_symbol_names::dph_heap_root_busy_allocations_field_symbol_name);
    }

    size_units::base_16::bytes dph_heap::busy_allocations_committed() const
    {
        return size_units::base_16::bytes{get_machine_size_field_value(*this, cache_data_->dph_heap_root_busy_allocations_bytes_committed_field_data, common_symbol_names::dph_heap_root_busy_allocations_bytes_committed_field_symbol_name)};
    }

    uint32_t dph_heap::virtual_storage_ranges() const
    {
        return stream_utils::get_field_value<uint32_t>(*this, cache_data_->dph_heap_root_virtual_storage_ranges_field_data, common_symbol_names::dph_heap_root_virtual_storage_ranges_field_symbol_name);
    }

    size_units::base_16::bytes dph_heap::virtual_storage_total() const
    {
        return size_units::base_16::bytes{get_machine_size_field_value(*this, cache_data_->dph_heap_root_virtual_storage_bytes_field_data, common_symbol_names::dph_heap_root_virtual_storage_bytes_field_symbol_name)};
    }

    uint32_t dph_heap::free_allocations() const
    {
        //return get_field_value<uint32_t>(common_symbol_names::dph_heap_root_free_allocations_field_symbol_name);
        //return std::ranges::count(free_entries());
        uint32_t index = 0;
        for ([[maybe_unused]] auto const& free_entry : free_entries())
        {
            ++index;
        }
        return index;
    }

    size_units::base_16::bytes dph_heap::free_allocations_committed() const
    {
        return size_units::base_16::bytes{get_machine_size_field_value(*this, cache_data_->dph_heap_root_free_allocations_bytes_committed_field_data, common_symbol_names::dph_heap_root_free_allocations_bytes_committed_field_symbol_name)};
    }

    generator<dph_heap> dph_heap::dph_heaps(cache_manager& cache, process::process_environment_block const& peb)
    {
        auto const symbol = peb.walker().get_symbol_info(common_symbol_names::av_rfp_dph_page_heap_list_global_symbol_name);
        if(!symbol.has_value())
        {
            co_return;
        }

        auto const dph_page_heap_list_address = symbol.value().address();
        if(!dph_page_heap_list_address.has_value() || dph_page_heap_list_address.value() == 0)
        {
            co_return;
        }

        for (ntdll_utilities::list_entry_walker const list_walker{cache, peb.walker(), dph_page_heap_list_address.value(), symbol_name, common_symbol_names::dph_heap_root_next_heap_field_symbol_name}; 
            auto const entry_address : list_walker.entries())
        {
            co_yield dph_heap{cache, peb, entry_address};
        }
    }

    generator<dph_entry> dph_heap::busy_entries() const
    {
        for(ntdll_utilities::rtl_balanced_links_walker const balanced_link_walker{cache(), walker(), address() + stream_utils::get_field_offset(cache_data_->dph_heap_root_busy_nodes_table_field_data, symbol_name, common_symbol_names::dph_heap_root_busy_nodes_table_field_symbol_name)};
            auto const entry_address : balanced_link_walker.entries())
        {
            co_yield dph_entry{*this, entry_address, dph_entry::is_virtual_allocation{false}};
        }
    }

    generator<dph_entry> dph_heap::free_entries() const
    {
        auto const head = get_field_pointer_raw(walker(), dph_heap_address_, cache_data_->dph_heap_root_free_allocations_list_head_field_data, symbol_name, common_symbol_names::dph_heap_root_free_allocations_list_head_field_symbol_name);
        if(head == 0)
        {
            co_return;
        }

        auto const tail = get_field_pointer_raw(walker(), dph_heap_address_, cache_data_->dph_heap_root_free_allocations_list_tail_field_data, symbol_name, common_symbol_names::dph_heap_root_free_allocations_list_tail_field_symbol_name);
        if(tail == 0)
        {
            co_return;
        }

        for(auto const& entry : walk_list(head, tail, dph_entry::is_virtual_allocation{false}))
        {
            co_yield entry;
        }
    }

    generator<dph_entry> dph_heap::virtual_ranges() const
    {
        auto const head = get_field_pointer_raw(walker(), dph_heap_address_, cache_data_->dph_heap_root_virtual_storage_list_head_field_data, symbol_name, common_symbol_names::dph_heap_root_virtual_storage_list_head_field_symbol_name);
        if(head == 0)
        {
            co_return;
        }

        auto const tail = get_field_pointer_raw(walker(), dph_heap_address_, cache_data_->dph_heap_root_virtual_storage_list_tail_field_data, symbol_name, common_symbol_names::dph_heap_root_virtual_storage_list_tail_field_symbol_name);
        if(tail == 0)
        {
            co_return;
        }

        for(auto const& entry : walk_list(head, tail, dph_entry::is_virtual_allocation{true}))
        {
            co_yield entry;
        }
    }
    
    generator<dph_entry> dph_heap::walk_list(uint64_t const head, uint64_t const tail, dph_entry::is_virtual_allocation const is_virtual_allocation) const
    {
        auto entry_address = head;
        while(entry_address != tail)
        {
            dph_entry entry{*this, entry_address, is_virtual_allocation};
            co_yield entry;
            entry_address = entry.next_alloc_address();
        }

        co_yield dph_entry{*this, entry_address, is_virtual_allocation};
    }

    dph_heap::cache_data const& dph_heap::setup_globals() const
    {
        if(!cache().has_cache<cache_data>())
        {
            auto& data = cache().get_cache<cache_data>();
            data.dph_heap_root_symbol_type = stream_utils::get_type(walker(), symbol_name);

            data.dph_heap_root_flags_field_data = stream_utils::get_field_type_and_offset_in_type(data.dph_heap_root_symbol_type, symbol_name, common_symbol_names::dph_heap_root_flags_field_symbol_name, dbg_help::sym_tag_enum::BaseType);
            data.dph_heap_root_extra_flags_field_data = stream_utils::get_field_type_and_offset_in_type(data.dph_heap_root_symbol_type, symbol_name, common_symbol_names::dph_heap_root_extra_flags_field_symbol_name, dbg_help::sym_tag_enum::BaseType);
            data.dph_heap_root_seed_field_data = stream_utils::get_field_type_and_offset_in_type(data.dph_heap_root_symbol_type, symbol_name, common_symbol_names::dph_heap_root_seed_field_symbol_name, dbg_help::sym_tag_enum::BaseType);
            data.dph_heap_root_busy_allocations_field_data = stream_utils::get_field_type_and_offset_in_type(data.dph_heap_root_symbol_type, symbol_name, common_symbol_names::dph_heap_root_busy_allocations_field_symbol_name, dbg_help::sym_tag_enum::BaseType);
            data.dph_heap_root_busy_allocations_bytes_committed_field_data = stream_utils::get_field_type_and_offset_in_type(data.dph_heap_root_symbol_type, symbol_name, common_symbol_names::dph_heap_root_busy_allocations_bytes_committed_field_symbol_name, dbg_help::sym_tag_enum::BaseType);
            data.dph_heap_root_virtual_storage_ranges_field_data = stream_utils::get_field_type_and_offset_in_type(data.dph_heap_root_symbol_type, symbol_name, common_symbol_names::dph_heap_root_virtual_storage_ranges_field_symbol_name, dbg_help::sym_tag_enum::BaseType);
            data.dph_heap_root_virtual_storage_bytes_field_data = stream_utils::get_field_type_and_offset_in_type(data.dph_heap_root_symbol_type, symbol_name, common_symbol_names::dph_heap_root_virtual_storage_bytes_field_symbol_name, dbg_help::sym_tag_enum::BaseType);
            data.dph_heap_root_free_allocations_bytes_committed_field_data = stream_utils::get_field_type_and_offset_in_type(data.dph_heap_root_symbol_type, symbol_name, common_symbol_names::dph_heap_root_free_allocations_bytes_committed_field_symbol_name, dbg_help::sym_tag_enum::BaseType);
            data.dph_heap_root_normal_heap_field_data = stream_utils::get_field_type_and_offset_in_type(data.dph_heap_root_symbol_type, symbol_name, common_symbol_names::dph_heap_root_normal_heap_field_symbol_name, dbg_help::sym_tag_enum::PointerType);
            data.dph_heap_root_free_allocations_list_head_field_data = stream_utils::get_field_type_and_offset_in_type(data.dph_heap_root_symbol_type, symbol_name, common_symbol_names::dph_heap_root_free_allocations_list_head_field_symbol_name, dbg_help::sym_tag_enum::PointerType);
            data.dph_heap_root_free_allocations_list_tail_field_data = stream_utils::get_field_type_and_offset_in_type(data.dph_heap_root_symbol_type, symbol_name, common_symbol_names::dph_heap_root_free_allocations_list_tail_field_symbol_name, dbg_help::sym_tag_enum::PointerType);
            data.dph_heap_root_virtual_storage_list_head_field_data = stream_utils::get_field_type_and_offset_in_type(data.dph_heap_root_symbol_type, symbol_name, common_symbol_names::dph_heap_root_virtual_storage_list_head_field_symbol_name, dbg_help::sym_tag_enum::PointerType);
            data.dph_heap_root_virtual_storage_list_tail_field_data = stream_utils::get_field_type_and_offset_in_type(data.dph_heap_root_symbol_type, symbol_name, common_symbol_names::dph_heap_root_virtual_storage_list_tail_field_symbol_name, dbg_help::sym_tag_enum::PointerType);

            data.dph_heap_root_busy_nodes_table_field_data = data.dph_heap_root_symbol_type.get_field_in_type(symbol_name, common_symbol_names::dph_heap_root_busy_nodes_table_field_symbol_name);

            dph_entry::setup_globals(*this);
        }

        return cache().get_cache<cache_data>();
    }
}
