#pragma once
#include <vector>

#include "graph_node_variable_symbol_reference_data.h"
#include "process_heap_entry_reference.h"
#include "process_heap_entry_symbol_address_reference.h"

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
        process_heap_graph_node(uint64_t start_address, process_heap_graph_node_type type);

        void add_from_reference(process_heap_entry_reference const& reference);
        void add_to_reference(process_heap_entry_reference const& reference);
        void remove_references(uint64_t node_index);
        void remove_metadata_symbol_references();

        [[nodiscard]] uint64_t index() const { return index_; }

        [[nodiscard]] uint64_t start_address() const { return start_address_; }

        [[nodiscard]] bool is_root_node() const { return from_references_.empty(); }
        [[nodiscard]] bool is_non_allocation_root_node() const { return type_ == process_heap_graph_node_type::root; }
        [[nodiscard]] bool is_system_allocation() const { return type_ == process_heap_graph_node_type::system_allocation; }

        [[nodiscard]] bool is_system() const { return is_system_; }
        void set_as_system(bool value = true);

        void mark_as_system_allocation();

        [[nodiscard]] bool can_contain_user_pointers() const { return can_contain_user_pointers_; }
        [[nodiscard]] bool& can_contain_user_pointers() { return can_contain_user_pointers_; }

        [[nodiscard]] bool& is_cyclic_node_graphs_only() { return is_cyclic_node_graphs_only_; }
        [[nodiscard]] bool is_cyclic_node_graphs_only() const { return is_cyclic_node_graphs_only_; }

        [[nodiscard]] bool& is_disconnected_node_graph() { return is_disconnected_node_graph_; }
        [[nodiscard]] bool is_disconnected_node_graph() const { return is_disconnected_node_graph_; }

        [[nodiscard]] std::vector<process_heap_entry_reference> const& from_references() const { return from_references_; }
        [[nodiscard]] std::vector<process_heap_entry_reference> const& to_references() const { return to_references_; }

        [[nodiscard]] std::vector<process_heap_entry_symbol_address_reference> const& symbol_references() const { return symbol_references_; }
        [[nodiscard]] std::vector<process_heap_entry_symbol_address_reference>& symbol_references() { return symbol_references_; }
        process_heap_entry_symbol_address_reference& add_symbol_address_reference(process_heap_entry_symbol_address_reference symbol_reference);
        [[nodiscard]] std::optional<graph_node_variable_symbol_reference_data> find_symbol_variable_reference(uint64_t address) const;

    private:
        void remove_all_symbol_references(uint64_t node_index);
        static [[nodiscard]] uint64_t get_next_process_heap_graph_node_index();
        static void remove_all_references_from(uint64_t node_index, std::vector<process_heap_entry_reference>& references);

    private:
        uint64_t start_address_;
        process_heap_graph_node_type type_{};
        uint64_t index_{get_next_process_heap_graph_node_index()};
        std::vector<process_heap_entry_reference> from_references_;
        std::vector<process_heap_entry_reference> to_references_;
        std::vector<process_heap_entry_symbol_address_reference> symbol_references_;
        bool is_cyclic_node_graphs_only_{false};
        bool is_disconnected_node_graph_{false};
        bool is_system_{false};
        bool can_contain_user_pointers_{false};
    };
}
