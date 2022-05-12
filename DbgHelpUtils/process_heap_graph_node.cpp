#include "process_heap_graph_node.h"

#include <atomic>

#include "process_heap_entry_reference.h"

namespace dlg_help_utils::heap
{
    process_heap_graph_node::process_heap_graph_node(bool const is_non_allocation_root_node)
    : is_non_allocation_root_node_{is_non_allocation_root_node}
    {
    }

    void process_heap_graph_node::add_from_reference(process_heap_entry_reference const& reference)
    {
        from_references_.emplace_back(reference);
    }

    void process_heap_graph_node::add_to_reference(process_heap_entry_reference const& reference)
    {
        to_references_.emplace_back(reference);
    }

    uint64_t process_heap_graph_node::get_next_process_heap_graph_node_index()
    {
        static std::atomic_uint64_t next_index{0};
        return next_index++;
    }
}
