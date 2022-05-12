﻿#pragma once
#include "process_heap_entry.h"
#include "process_heap_graph_node.h"

namespace dlg_help_utils::heap
{
    class process_heap_graph_heap_entry : public process_heap_graph_node
    {
    public:
        process_heap_graph_heap_entry(heap::process_heap_entry heap_entry);

        [[nodiscard]] heap::process_heap_entry const& heap_entry() const { return heap_entry_; }

    private:
        heap::process_heap_entry heap_entry_;
    };

    inline std::strong_ordering operator<=>(process_heap_graph_heap_entry const& a, process_heap_graph_heap_entry const& b)
    {
        return a.heap_entry().user_address() <=> b.heap_entry().user_address();
    }

    inline bool operator<(process_heap_graph_heap_entry const& a, process_heap_graph_heap_entry const& b)
    {
        return a.heap_entry().user_address() < b.heap_entry().user_address();
    }

    inline bool operator==(process_heap_graph_heap_entry const& a, process_heap_graph_heap_entry const& b)
    {
        return a.heap_entry().user_address() == b.heap_entry().user_address();
    }
}

