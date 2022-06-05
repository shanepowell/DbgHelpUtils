#include "process_heap_graph_node.h"

#include <algorithm>
#include <atomic>

#include "process_heap_entry_reference.h"
#include "wide_runtime_error.h"

namespace dlg_help_utils::heap
{
    process_heap_graph_node::process_heap_graph_node(process_heap_graph_node_type const type)
    : type_{type}
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

    void process_heap_graph_node::remove_references(uint64_t const node_index)
    {
        remove_all_references_from(node_index, from_references_);
        remove_all_references_from(node_index, to_references_);
    }

    void process_heap_graph_node::mark_as_system_allocation()
    {
        if(type_ != process_heap_graph_node_type::allocation)
        {
            throw exceptions::wide_runtime_error{std::format(L"graph node type not allocation [{}]", static_cast<int>(type_))};
        }

        type_ = process_heap_graph_node_type::system_allocation;
    }

    uint64_t process_heap_graph_node::get_next_process_heap_graph_node_index()
    {
        static std::atomic_uint64_t next_index{0};
        return next_index++;
    }

    void process_heap_graph_node::remove_all_references_from(uint64_t const node_index, std::vector<process_heap_entry_reference>& references)
    {
        auto const [first, last] = std::ranges::remove_if(references, [node_index](auto const& reference) { return reference.node_index() == node_index; });
        references.erase(first, last);
    }
}
