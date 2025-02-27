#include "rtl_rb_tree_walker.h"

#include "cache_manager.h"
#include "common_symbol_names.h"
#include "stream_utils.h"

namespace dlg_help_utils::ntdll_utilities
{
    std::wstring const& rtl_rb_tree_walker::symbol_name = common_symbol_names::rtl_rb_tree_structure_symbol_name;
    std::wstring const& rtl_rb_tree_walker::node_symbol_name = common_symbol_names::rtl_balanced_node_structure_symbol_name;

    rtl_rb_tree_walker::rtl_rb_tree_walker(cache_manager& cache, stream_stack_dump::mini_dump_memory_walker const& walker, uint64_t const rb_tree_address, std::wstring const& entry_symbol_name, std::wstring const& entry_field_name)
    : cache_manager_{&cache}
    , walker_{&walker}
    , rb_tree_address_{rb_tree_address}
    , rtl_balanced_node_entry_offset_{stream_utils::get_field_offset_from_type(stream_utils::get_type(walker, entry_symbol_name), entry_symbol_name, entry_field_name)}
    {
    }

    dlg_help_utils::generator<uint64_t> rtl_rb_tree_walker::entries() const
    {
        auto const root_node = get_field_pointer_raw(walker(), rb_tree_address_, cache_data_->root_field_data, symbol_name, common_symbol_names::rtl_rb_tree_root_field_symbol_name);
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
            
            if(auto const right_node = get_field_pointer_raw(walker(), node_address, cache_data_->right_field_data, node_symbol_name, common_symbol_names::rtl_balanced_node_right_field_symbol_name);
                right_node != 0)
            {
                nodes.push_back(right_node);
            }

            if(auto const left_node = get_field_pointer_raw(walker(), node_address, cache_data_->left_field_data, node_symbol_name, common_symbol_names::rtl_balanced_node_left_field_symbol_name);
                left_node != 0)
            {
                nodes.push_back(left_node);
            }
        }
    }

    rtl_rb_tree_walker::cache_data const& rtl_rb_tree_walker::setup_globals() const
    {
        if(!cache().has_cache<cache_data>())
        {
            auto& data = cache().get_cache<cache_data>();
            data.rtl_rb_tree_symbol_type = stream_utils::get_type(walker(), symbol_name);
            data.rtl_rb_balanced_node_symbol_type = stream_utils::get_type(walker(), node_symbol_name);
            data.root_field_data = stream_utils::get_field_type_and_offset_in_type(data.rtl_rb_tree_symbol_type, symbol_name, common_symbol_names::rtl_rb_tree_root_field_symbol_name, dbg_help::sym_tag_enum::PointerType);
            data.right_field_data = stream_utils::get_field_type_and_offset_in_type(data.rtl_rb_balanced_node_symbol_type, symbol_name, common_symbol_names::rtl_balanced_node_right_field_symbol_name, dbg_help::sym_tag_enum::PointerType);
            data.left_field_data = stream_utils::get_field_type_and_offset_in_type(data.rtl_rb_balanced_node_symbol_type, symbol_name, common_symbol_names::rtl_balanced_node_left_field_symbol_name, dbg_help::sym_tag_enum::PointerType);
        }

        return cache().get_cache<cache_data>();
    }
}
