#pragma once
#include <map>
#include <variant>
#include <vector>

#include "process_heap_graph_global_variable_entry.h"
#include "process_heap_graph_heap_entry.h"
#include "vector_to_hash_set.h"

namespace dlg_help_utils::heap
{
    class process_heaps;
    using process_heap_graph_entry_type = std::variant<process_heap_graph_global_variable_entry, process_heap_graph_heap_entry>;

    [[nodiscard]] inline process_heap_graph_node const& get_graph_node(process_heap_graph_entry_type const& node)
    {
        return std::visit([](auto const& _) -> process_heap_graph_node const& { return _; }, node);
    }

    class process_heap_graph
    {
    public:
        process_heap_graph(process_heaps const& process);

        void generate_graph();

        [[nodiscard]] std::vector<process_heap_graph_entry_type> const& nodes() const { return nodes_; }

    private:
        [[nodiscard]] std::map<uint64_t, size_t> generate_allocation_references();
        [[nodiscard]] std::optional<process_heap_graph_heap_entry> find_allocation_node_allocation(std::map<uint64_t, size_t> const& heap_entries, range const& data_range) const;
        void generate_global_variable_references(std::map<uint64_t, size_t> const& heap_entries);
        void generate_node_references(std::map<uint64_t, size_t> const& heap_entries);
        void remove_all_non_allocation_with_empty_to_references();

    private:
        process_heaps const* process_;
        std::vector<process_heap_graph_entry_type> nodes_{};
    };
}
