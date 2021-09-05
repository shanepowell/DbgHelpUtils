#include "rtl_balanced_links_walker.h"

#include "common_symbol_names.h"
#include "stream_utils.h"

namespace dlg_help_utils::ntdll_utilities
{
    std::wstring const& rtl_balanced_links_walker::symbol_name = common_symbol_names::rtl_balanced_links_structure_symbol_name;

    rtl_balanced_links_walker::rtl_balanced_links_walker(stream_stack_dump::mini_dump_stack_walk const& walker, uint64_t const head_node_address, std::wstring const& entry_symbol_name, std::wstring const& entry_field_name)
    : walker_{walker}
    , head_node_address_{head_node_address}
    , rtl_balanced_links_symbol_type_{stream_utils::get_type(walker_, symbol_name)}
    , rtl_balanced_link_entry_offset_{stream_utils::get_field_offset(stream_utils::get_type(walker_, entry_symbol_name), entry_symbol_name, entry_field_name)}
    {
    }

    rtl_balanced_links_walker::rtl_balanced_links_walker(stream_stack_dump::mini_dump_stack_walk const& walker, uint64_t const head_node_address)
    : walker_{walker}
    , head_node_address_{head_node_address}
    , rtl_balanced_links_symbol_type_{stream_utils::get_type(walker_, symbol_name)}
    , rtl_balanced_link_entry_offset_{0}
    {
    }

    std::experimental::generator<uint64_t> rtl_balanced_links_walker::entries() const
    {
        if(head_node_address_ == 0)
        {
            co_return;
        }

        auto const& right_field = common_symbol_names::rtl_balanced_links_right_child_field_symbol_name;
        auto const& left_field = common_symbol_names::rtl_balanced_links_left_child_field_symbol_name;
        std::vector<uint64_t> nodes;
        if(auto const right_node = stream_utils::get_field_pointer_raw(walker_, head_node_address_, rtl_balanced_links_symbol_type_, symbol_name, right_field);
            right_node != 0)
        {
            nodes.push_back(right_node);
        }

        if(auto const left_node = stream_utils::get_field_pointer_raw(walker_, head_node_address_, rtl_balanced_links_symbol_type_, symbol_name, left_field);
            left_node != 0)
        {
            nodes.push_back(left_node);
        }

        while(!nodes.empty())
        {
            auto const node_address = nodes.back();
            nodes.pop_back();

            co_yield node_address - rtl_balanced_link_entry_offset_;
            
            if(auto const right_node = stream_utils::get_field_pointer_raw(walker_, node_address, rtl_balanced_links_symbol_type_, symbol_name, right_field);
                right_node != 0)
            {
                nodes.push_back(right_node);
            }

            if(auto const left_node = stream_utils::get_field_pointer_raw(walker_, node_address, rtl_balanced_links_symbol_type_, symbol_name, left_field);
                left_node != 0)
            {
                nodes.push_back(left_node);
            }
        }
    }
}
