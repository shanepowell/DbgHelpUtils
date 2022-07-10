#pragma once
#include <cstdint>

#include "symbol_type_info.h"

namespace dlg_help_utils::heap::allocation_graph
{
    struct graph_node_variable_symbol_reference_data
    {
        uint64_t parent_node_index{};
        uint64_t parent_address{};
        uint64_t variable_offset{};
        uint64_t variable_address{};
        dbg_help::symbol_type_info parent_symbol_type;
        dbg_help::symbol_type_info variable_symbol_type;
        std::wstring name;
    };
}
