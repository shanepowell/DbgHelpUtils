﻿#include "process_heap_graph_heap_entry.h"

namespace dlg_help_utils::heap
{
    process_heap_graph_heap_entry::process_heap_graph_heap_entry(process_heap_entry heap_entry, process_heap_graph_heap_entry_type const type)
    : process_heap_graph_node{type == process_heap_graph_heap_entry_type::system_allocation ? process_heap_graph_node_type::system_allocation : process_heap_graph_node_type::allocation}
    , heap_entry_{std::move(heap_entry)}
    {
    }
}
