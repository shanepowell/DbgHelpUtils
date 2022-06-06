#pragma once
#include <vector>

#include "process_heap_entry_reference.h"

namespace dlg_help_utils::heap::allocation_graph
{
    enum class process_heap_graph_node_type
    {
        allocation,
        system_allocation,
        root
    };

    class process_heap_graph_node
    {
    public:
        process_heap_graph_node(process_heap_graph_node_type type);

        void add_from_reference(process_heap_entry_reference const& reference);
        void add_to_reference(process_heap_entry_reference const& reference);
        void remove_references(uint64_t node_index);

        [[nodiscard]] uint64_t index() const { return index_; }

        [[nodiscard]] bool is_root_node() const { return from_references_.empty(); }
        [[nodiscard]] bool is_non_allocation_root_node() const { return type_ == process_heap_graph_node_type::root; }
        [[nodiscard]] bool is_system_allocation() const { return type_ == process_heap_graph_node_type::system_allocation; }

        [[nodiscard]] bool is_system() const { return is_system_; }
        [[nodiscard]] bool& is_system() { return is_system_; }

        [[nodiscard]] bool& is_cyclic_node_graphs_only() { return is_cyclic_node_graphs_only_; }
        [[nodiscard]] bool is_cyclic_node_graphs_only() const { return is_cyclic_node_graphs_only_; }

        [[nodiscard]] bool& is_disconnected_node_graph() { return is_disconnected_node_graph_; }
        [[nodiscard]] bool is_disconnected_node_graph() const { return is_disconnected_node_graph_; }

        [[nodiscard]] std::vector<process_heap_entry_reference> const& from_references() const { return from_references_; }
        [[nodiscard]] std::vector<process_heap_entry_reference> const& to_references() const { return to_references_; }

        void mark_as_system_allocation();

    private:
        static [[nodiscard]] uint64_t get_next_process_heap_graph_node_index();
        static void remove_all_references_from(uint64_t node_index, std::vector<process_heap_entry_reference>& references);

    private:
        process_heap_graph_node_type type_{};
        uint64_t index_{get_next_process_heap_graph_node_index()};
        std::vector<process_heap_entry_reference> from_references_;
        std::vector<process_heap_entry_reference> to_references_;
        bool is_cyclic_node_graphs_only_{false};
        bool is_disconnected_node_graph_{false};
        bool is_system_{false};
    };
}
