#pragma once
// ReSharper disable once CppUnusedIncludeDirective
#include <compare>

#include "process_heap_entry.h"
#include "process_heap_graph_node.h"

namespace dlg_help_utils::heap::allocation_graph
{
    enum class process_heap_graph_heap_entry_type
    {
        allocation,
        system_allocation,
    };

    class process_heap_graph_heap_entry : public process_heap_graph_node
    {
    public:
        process_heap_graph_heap_entry(process_heap_entry heap_entry, process_heap_graph_heap_entry_type type);

        [[nodiscard]] process_heap_entry const& heap_entry() const { return heap_entry_; }

        std::strong_ordering operator<=>(process_heap_graph_heap_entry const& b) const
        {
            return get_sort_key() <=> b.get_sort_key();
        }

    private:
        [[nodiscard]] uint64_t get_sort_key() const { return heap_entry().user_address(); }

    private:
        process_heap_entry heap_entry_;
    };
}

