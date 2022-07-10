#pragma once
#include <cstdint>
#include <optional>

#include "graph_node_variable_symbol_reference_data.h"

namespace dlg_help_utils::heap::allocation_graph
{
    class process_heap_graph_node;

    class process_heap_entry_reference
    {
    public:
        process_heap_entry_reference(uint64_t offset, uint64_t pointer, process_heap_graph_node const& heap_entry);
        process_heap_entry_reference(uint64_t offset, uint64_t pointer, process_heap_graph_node const& heap_entry, graph_node_variable_symbol_reference_data const& variable_symbol_info);

        [[nodiscard]] uint64_t node_index() const { return node_index_; }
        [[nodiscard]] uint64_t offset() const { return offset_; }
        [[nodiscard]] uint64_t pointer() const { return pointer_; }
        [[nodiscard]] std::optional<graph_node_variable_symbol_reference_data> const& variable_symbol_info() const { return variable_symbol_info_; }

    private:
        uint64_t offset_;
        uint64_t pointer_;
        uint64_t node_index_;
        std::optional<graph_node_variable_symbol_reference_data> variable_symbol_info_;
    };
}
