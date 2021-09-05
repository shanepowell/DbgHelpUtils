#include "rtl_rb_tree_walker.h"

#include "common_symbol_names.h"
#include "stream_utils.h"

namespace dlg_help_utils::ntdll_utilities
{
    std::wstring const& rtl_rb_tree_walker::symbol_name = common_symbol_names::rtl_rb_tree_structure_symbol_name;
    std::wstring const& rtl_rb_tree_walker::node_symbol_name = common_symbol_names::rtl_balanced_node_structure_symbol_name;

    rtl_rb_tree_walker::rtl_rb_tree_walker(stream_stack_dump::mini_dump_stack_walk const& walker, uint64_t const rb_tree_address, std::wstring const& entry_symbol_name, std::wstring const& entry_field_name)
    : walker_{walker}
    , rb_tree_address_{rb_tree_address}
    , rtl_rb_tree_symbol_type_{stream_utils::get_type(walker_, symbol_name)}
    , rtl_rb_balanced_node_symbol_type_{stream_utils::get_type(walker_, node_symbol_name)}
    , rtl_balanced_node_entry_offset_{stream_utils::get_field_offset(stream_utils::get_type(walker_, entry_symbol_name), entry_symbol_name, entry_field_name)}
    {
    }

    std::experimental::generator<uint64_t> rtl_rb_tree_walker::entries() const
    {
        auto const root_node = stream_utils::get_field_pointer_raw(walker_, rb_tree_address_, rtl_rb_tree_symbol_type_, symbol_name, common_symbol_names::rtl_rb_tree_root_field_symbol_name);
        if(root_node == 0)
        {
            co_return;
        }

        std::vector<uint64_t> nodes;
        nodes.emplace_back(root_node);

        while(!nodes.empty())
        {
            auto const node_address = nodes.back();
            nodes.pop_back();

            co_yield node_address - rtl_balanced_node_entry_offset_;
            
            if(auto const right_node = stream_utils::get_field_pointer_raw(walker_, node_address, rtl_rb_balanced_node_symbol_type_, symbol_name, common_symbol_names::rtl_balanced_node_right_field_symbol_name);
                right_node != 0)
            {
                nodes.push_back(right_node);
            }

            if(auto const left_node = stream_utils::get_field_pointer_raw(walker_, node_address, rtl_rb_balanced_node_symbol_type_, symbol_name, common_symbol_names::rtl_balanced_node_left_field_symbol_name);
                left_node != 0)
            {
                nodes.push_back(left_node);
            }
        }
    }
}
