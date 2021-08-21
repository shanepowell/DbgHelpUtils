#include "dph_heap.h"

#include "common_symbol_names.h"
#include "dph_entry.h"
#include "nt_heap.h"
#include "stream_utils.h"

namespace dlg_help_utils::heap
{
    dph_heap::dph_heap(process::process_environment_block const& peb, uint64_t const dph_heap_address)
    : peb_{peb}
    , dph_heap_address_{dph_heap_address}
    , dph_heap_root_symbol_type_{stream_utils::get_type(walker(), common_symbol_names::dph_heap_root_structure_symbol_name)}
    , rtl_balanced_links_symbol_type_{stream_utils::get_type(walker(), common_symbol_names::rtl_balanced_links_structure_symbol_name)}
    , busy_nodes_table_offset_{stream_utils::get_field_offset(dph_heap_root_symbol_type_, common_symbol_names::dph_heap_root_structure_symbol_name, common_symbol_names::dph_heap_root_busy_nodes_table_field_symbol_name)}
    , stack_trace_{walker()}
    {
    }

    template <typename T>
    T dph_heap::get_field_value(std::wstring const& field_name) const
    {
        auto const value = stream_utils::find_basic_type_field_value_in_type<T>(walker(), dph_heap_root_symbol_type_, field_name, dph_heap_address_);
        if(!value.has_value())
        {
            throw_cant_get_field_data(field_name);
        }

        return value.value();
    }

    uint64_t dph_heap::get_machine_size_field_value(std::wstring const& field_name) const
    {
        auto const value = stream_utils::find_machine_size_field_value(peb(), dph_heap_root_symbol_type_, field_name, dph_heap_address_);
        if(!value.has_value())
        {
            throw_cant_get_field_data(field_name);
        }

        return value.value();
    }

    uint64_t dph_heap::normal_heap() const
    {
        return stream_utils::get_field_pointer(walker(), dph_heap_address_, dph_heap_root_symbol_type_, common_symbol_names::dph_heap_root_structure_symbol_name, common_symbol_names::dph_heap_root_normal_heap_field_symbol_name);
    }

    uint32_t dph_heap::flags() const
    {
        return get_field_value<uint32_t>(common_symbol_names::dph_heap_root_flags_field_symbol_name);
    }

    uint32_t dph_heap::extra_flags() const
    {
        return get_field_value<uint32_t>(common_symbol_names::dph_heap_root_extra_flags_field_symbol_name);
    }

    uint32_t dph_heap::seed() const
    {
        return get_field_value<uint32_t>(common_symbol_names::dph_heap_root_seed_field_symbol_name);
    }

    uint32_t dph_heap::busy_allocations() const
    {
        return get_field_value<uint32_t>(common_symbol_names::dph_heap_root_busy_allocations_field_symbol_name);
    }

    size_units::base_10::bytes dph_heap::busy_allocations_committed() const
    {
        return size_units::base_10::bytes{get_machine_size_field_value(common_symbol_names::dph_heap_root_busy_allocations_bytes_committed_field_symbol_name)};
    }

    uint32_t dph_heap::virtual_storage_ranges() const
    {
        return get_field_value<uint32_t>(common_symbol_names::dph_heap_root_virtual_storage_ranges_field_symbol_name);
    }

    size_units::base_10::bytes dph_heap::virtual_storage_total() const
    {
        return size_units::base_10::bytes{get_machine_size_field_value(common_symbol_names::dph_heap_root_virtual_storage_bytes_field_symbol_name)};
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

    size_units::base_10::bytes dph_heap::free_allocations_committed() const
    {
        return size_units::base_10::bytes{get_machine_size_field_value(common_symbol_names::dph_heap_root_free_allocations_bytes_committed_field_symbol_name)};
    }

    std::experimental::generator<dph_heap> dph_heap::dph_heaps(process::process_environment_block const& peb)
    {
        auto const symbol = peb.walker().get_symbol_info(common_symbol_names::av_rfp_dph_page_heap_list_global_symbol_name);
        if(!symbol.has_value())
        {
            co_return;
        }

        auto const dph_page_heap_list_address = symbol.value().address();
        if(!dph_page_heap_list_address.has_value())
        {
            co_return;
        }

        auto const list_entry_symbol_type = stream_utils::get_type(peb.walker(), common_symbol_names::list_entry_structure_symbol_name);
        auto const dph_heap_root_next_heap_offset = stream_utils::get_field_offset(stream_utils::get_type(peb.walker(), common_symbol_names::dph_heap_root_structure_symbol_name), common_symbol_names::dph_heap_root_structure_symbol_name, common_symbol_names::dph_heap_root_next_heap_field_symbol_name);
        auto const start = dph_page_heap_list_address.value();

        auto const flink_value = stream_utils::find_field_pointer_type_and_value_in_type(peb.walker(), list_entry_symbol_type, common_symbol_names::list_entry_flink_field_symbol_name, start);
        if(!flink_value.has_value())
        {
            stream_utils::throw_cant_get_field_data(common_symbol_names::list_entry_structure_symbol_name, common_symbol_names::list_entry_flink_field_symbol_name);
        }

        auto flink = std::get<1>(flink_value.value());
        if(flink == 0)
        {
            stream_utils::throw_cant_get_field_is_null(common_symbol_names::list_entry_structure_symbol_name, common_symbol_names::list_entry_flink_field_symbol_name);
        }

        while(flink != start)
        {
            auto const dph_heap_address = flink - dph_heap_root_next_heap_offset;
            co_yield dph_heap{peb, dph_heap_address};

            flink = stream_utils::get_field_pointer(peb.walker(), flink, list_entry_symbol_type, common_symbol_names::list_entry_structure_symbol_name, common_symbol_names::list_entry_flink_field_symbol_name);
        }
    }

    std::experimental::generator<dph_entry> dph_heap::busy_entries() const
    {
        for(auto const entry_address : walk_balanced_table())
        {
            co_yield dph_entry{*this, entry_address};
        }
    }

    std::experimental::generator<dph_entry> dph_heap::free_entries() const
    {
        auto const head = stream_utils::get_field_pointer_raw(walker(), dph_heap_address_, dph_heap_root_symbol_type_, common_symbol_names::dph_heap_root_structure_symbol_name, common_symbol_names::dph_heap_root_free_allocations_list_head_field_symbol_name);
        if(head == 0)
        {
            co_return;
        }

        auto const tail = stream_utils::get_field_pointer_raw(walker(), dph_heap_address_, dph_heap_root_symbol_type_, common_symbol_names::dph_heap_root_structure_symbol_name, common_symbol_names::dph_heap_root_free_allocations_list_tail_field_symbol_name);
        if(tail == 0)
        {
            co_return;
        }

        for(auto const& entry : walk_list(head, tail))
        {
            co_yield entry;
        }
    }

    std::experimental::generator<dph_entry> dph_heap::virtual_ranges() const
    {
        auto const head = stream_utils::get_field_pointer_raw(walker(), dph_heap_address_, dph_heap_root_symbol_type_, common_symbol_names::dph_heap_root_structure_symbol_name, common_symbol_names::dph_heap_root_virtual_storage_list_head_field_symbol_name);
        if(head == 0)
        {
            co_return;
        }

        auto const tail = stream_utils::get_field_pointer_raw(walker(), dph_heap_address_, dph_heap_root_symbol_type_, common_symbol_names::dph_heap_root_structure_symbol_name, common_symbol_names::dph_heap_root_virtual_storage_list_tail_field_symbol_name);
        if(tail == 0)
        {
            co_return;
        }

        for(auto const& entry : walk_list(head, tail))
        {
            co_yield entry;
        }
    }

    std::experimental::generator<uint64_t> dph_heap::walk_balanced_table() const
    {
        std::vector<uint64_t> nodes;
        auto const head_node = address() + busy_nodes_table_offset_;
        if(auto const right_node = stream_utils::get_field_pointer_raw(walker(), head_node, rtl_balanced_links_symbol_type_, common_symbol_names::rtl_balanced_links_structure_symbol_name, common_symbol_names::rtl_balanced_links_right_child_field_symbol_name);
            right_node != 0)
        {
            nodes.push_back(right_node);
        }

        if(auto const left_node = stream_utils::get_field_pointer_raw(walker(), head_node, rtl_balanced_links_symbol_type_, common_symbol_names::rtl_balanced_links_structure_symbol_name, common_symbol_names::rtl_balanced_links_left_child_field_symbol_name);
            left_node != 0)
        {
            nodes.push_back(left_node);
        }

        while(!nodes.empty())
        {
            auto const node_address = nodes.back();
            nodes.pop_back();

            co_yield node_address;
            
            if(auto const right_node = stream_utils::get_field_pointer_raw(walker(), node_address, rtl_balanced_links_symbol_type_, common_symbol_names::rtl_balanced_links_structure_symbol_name, common_symbol_names::rtl_balanced_links_right_child_field_symbol_name);
                right_node != 0)
            {
                nodes.push_back(right_node);
            }

            if(auto const left_node = stream_utils::get_field_pointer_raw(walker(), node_address, rtl_balanced_links_symbol_type_, common_symbol_names::rtl_balanced_links_structure_symbol_name, common_symbol_names::rtl_balanced_links_left_child_field_symbol_name);
                left_node != 0)
            {
                nodes.push_back(left_node);
            }
        }
    }

    std::experimental::generator<dph_entry> dph_heap::walk_list(uint64_t const head, uint64_t const tail) const
    {
        auto entry_address = head;
        while(entry_address != tail)
        {
            dph_entry entry{*this, entry_address};
            co_yield entry;
            entry_address = entry.next_alloc_address();
        }

        co_yield dph_entry{*this, entry_address};
    }

    void dph_heap::throw_cant_get_field_data(std::wstring const& field_name)
    {
        stream_utils::throw_cant_get_field_data(common_symbol_names::dph_heap_root_structure_symbol_name, field_name);
    }

}
