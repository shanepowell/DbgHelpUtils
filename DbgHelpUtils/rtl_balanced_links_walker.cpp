﻿#include "rtl_balanced_links_walker.h"

#include "cache_manager.h"
#include "common_symbol_names.h"
#include "rtl_rb_tree_walker.h"
#include "stream_utils.h"

namespace dlg_help_utils::ntdll_utilities
{
    std::wstring const& rtl_balanced_links_walker::symbol_name = common_symbol_names::rtl_balanced_links_structure_symbol_name;

    rtl_balanced_links_walker::rtl_balanced_links_walker(cache_manager& cache, stream_stack_dump::mini_dump_memory_walker const& walker, uint64_t const head_node_address, std::wstring const& entry_symbol_name, std::wstring const& entry_field_name)
    : cache_manager_{&cache}
    , walker_{&walker}
    , head_node_address_{head_node_address}
    , rtl_balanced_link_entry_offset_{stream_utils::get_field_offset_from_type(stream_utils::get_type(walker, entry_symbol_name), entry_symbol_name, entry_field_name)}
    {
    }

    rtl_balanced_links_walker::rtl_balanced_links_walker(cache_manager& cache, stream_stack_dump::mini_dump_memory_walker const& walker, uint64_t const head_node_address)
    : cache_manager_{&cache}
    , walker_{&walker}
    , head_node_address_{head_node_address}
    , rtl_balanced_link_entry_offset_{0}
    {
    }

    std::generator<uint64_t> rtl_balanced_links_walker::entries() const
    {
        if(head_node_address_ == 0)
        {
            co_return;
        }

        auto const& right_field = common_symbol_names::rtl_balanced_links_right_child_field_symbol_name;
        auto const& left_field = common_symbol_names::rtl_balanced_links_left_child_field_symbol_name;
        std::vector<uint64_t> nodes;
        if(auto const right_node = get_field_pointer_raw(walker(), head_node_address_, cache_data_->right_field_data, symbol_name, right_field);
            right_node != 0)
        {
            nodes.push_back(right_node);
        }

        if(auto const left_node = get_field_pointer_raw(walker(), head_node_address_, cache_data_->left_field_data, symbol_name, left_field);
            left_node != 0)
        {
            nodes.push_back(left_node);
        }

        while(!nodes.empty())
        {
            auto const node_address = nodes.back();
            nodes.pop_back();

            co_yield node_address - rtl_balanced_link_entry_offset_;
            
            if(auto const right_node = get_field_pointer_raw(walker(), node_address, cache_data_->right_field_data, symbol_name, right_field);
                right_node != 0)
            {
                nodes.push_back(right_node);
            }

            if(auto const left_node = get_field_pointer_raw(walker(), node_address, cache_data_->left_field_data, symbol_name, left_field);
                left_node != 0)
            {
                nodes.push_back(left_node);
            }
        }
    }

    rtl_balanced_links_walker::cache_data const& rtl_balanced_links_walker::setup_globals() const
    {
        if(!cache().has_cache<cache_data>())
        {
            auto& data = cache().get_cache<cache_data>();
            data.rtl_balanced_links_symbol_type = stream_utils::get_type(walker(), symbol_name);
            data.right_field_data = stream_utils::get_field_type_and_offset_in_type(data.rtl_balanced_links_symbol_type, symbol_name, common_symbol_names::rtl_balanced_links_right_child_field_symbol_name, dbg_help::sym_tag_enum::PointerType);
            data.left_field_data = stream_utils::get_field_type_and_offset_in_type(data.rtl_balanced_links_symbol_type, symbol_name, common_symbol_names::rtl_balanced_links_left_child_field_symbol_name, dbg_help::sym_tag_enum::PointerType);
        }

        return cache().get_cache<cache_data>();
    }
}
