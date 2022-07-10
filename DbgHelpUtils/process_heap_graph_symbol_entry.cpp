#include "process_heap_graph_symbol_entry.h"

namespace dlg_help_utils::heap::allocation_graph
{
    process_heap_graph_symbol_entry::process_heap_graph_symbol_entry(uint64_t const address, dbg_help::symbol_type_info symbol_type, std::optional<uint32_t> const thread_id, std::wstring thread_name)
        : process_heap_graph_node(process_heap_graph_node_type::root)
        , address_{address}
        , symbol_type_{std::move(symbol_type)}
        , thread_id_{thread_id}
        , thread_name_{std::move(thread_name)}
    {
    }
}
