#pragma once
#include "process_heap_entry.h"
#include "process_heap_graph_node.h"

namespace dlg_help_utils::heap
{
    class process_heap_graph_heap_entry : public process_heap_graph_node
    {
    public:
        process_heap_graph_heap_entry(process_heap_entry heap_entry);

        [[nodiscard]] process_heap_entry const& heap_entry() const { return heap_entry_; }

        [[nodiscard]] uint64_t get_sort_key() const { return heap_entry().user_address(); }

    private:
        process_heap_entry heap_entry_;
    };

    inline std::strong_ordering operator<=>(process_heap_graph_heap_entry const& a, process_heap_graph_heap_entry const& b)
    {
        return a.get_sort_key() <=> b.get_sort_key();
    }

    inline bool operator<(process_heap_graph_heap_entry const& a, process_heap_graph_heap_entry const& b)
    {
        return a.get_sort_key() < b.get_sort_key();
    }

    inline bool operator==(process_heap_graph_heap_entry const& a, process_heap_graph_heap_entry const& b)
    {
        return a.get_sort_key() == b.get_sort_key();
    }
}

