#include "process_heap_entry_reference.h"

#include "process_heap_graph_node.h"

namespace dlg_help_utils::heap::allocation_graph
{
    process_heap_entry_reference::process_heap_entry_reference(uint64_t const offset, uint64_t const pointer, process_heap_graph_node const& heap_entry)
        : offset_{offset}
        , pointer_{pointer}
        , node_index_{heap_entry.index()}
    {
    }
}
