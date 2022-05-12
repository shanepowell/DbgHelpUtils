#pragma once
#include <vector>

#include "process_heap_entry_reference.h"

namespace dlg_help_utils::heap
{
    class process_heap_graph_node
    {
    public:
        process_heap_graph_node(bool is_non_allocation_root_node);

        void add_from_reference(process_heap_entry_reference const& reference);
        void add_to_reference(process_heap_entry_reference const& reference);

        [[nodiscard]] uint64_t index() const { return index_; }

        [[nodiscard]] bool is_root_node() const { return from_references_.empty(); }
        [[nodiscard]] bool is_non_allocation_root_node() const { return is_non_allocation_root_node_; }

        [[nodiscard]] bool& is_cyclic_node_graphs_only() { return is_cyclic_node_graphs_only_; }
        [[nodiscard]] bool is_cyclic_node_graphs_only() const { return is_cyclic_node_graphs_only_; }

        [[nodiscard]] bool& is_disconnected_node_graph() { return is_disconnected_node_graph_; }
        [[nodiscard]] bool is_disconnected_node_graph() const { return is_disconnected_node_graph_; }

        [[nodiscard]] std::vector<process_heap_entry_reference> const& from_references() const { return from_references_; }
        [[nodiscard]] std::vector<process_heap_entry_reference> const& to_references() const { return to_references_; }

    private:
        static [[nodiscard]] uint64_t get_next_process_heap_graph_node_index();

    private:
        bool is_non_allocation_root_node_{};
        uint64_t index_{get_next_process_heap_graph_node_index()};
        std::vector<process_heap_entry_reference> from_references_;
        std::vector<process_heap_entry_reference> to_references_;
        bool is_cyclic_node_graphs_only_{false};
        bool is_disconnected_node_graph_{false};
    };
}
