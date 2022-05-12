#pragma once
#include <variant>
#include <vector>

#include "process_heap_graph_heap_entry.h"

namespace dlg_help_utils::heap
{
    class process_heaps;
    using process_heap_graph_entry_type = std::variant<process_heap_graph_heap_entry>;

    [[nodiscard]] inline process_heap_graph_node const& get_graph_node(process_heap_graph_entry_type const& node)
    {
        return std::visit([](auto const& _) -> process_heap_graph_node const& { return _; }, node);
    }


    class process_heap_graph
    {
    public:
        process_heap_graph(heap::process_heaps const& process);

        void generate_graph();

        [[nodiscard]] std::vector<process_heap_graph_entry_type> const& nodes() const { return nodes_; }

    private:
        heap::process_heaps const* process_;
        std::vector<process_heap_graph_entry_type> nodes_{};
    };
}
