#include "process_heap_graph_heap_entry.h"

namespace dlg_help_utils::heap
{
    process_heap_graph_heap_entry::process_heap_graph_heap_entry(process_heap_entry heap_entry)
    : process_heap_graph_node{false}
    , heap_entry_{std::move(heap_entry)}
    {
    }
}
