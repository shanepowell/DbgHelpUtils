#include "process_heap_graph_thread_context_entry.h"

namespace dlg_help_utils::heap
{
    process_heap_graph_thread_context_entry::process_heap_graph_thread_context_entry(CV_HREG_e const register_type, uint64_t const register_data, uint32_t const thread_id, std::wstring thread_name)
        : process_heap_graph_node{process_heap_graph_node_type::root}
        , register_type_{register_type}
        , register_data_{register_data}
        , thread_id_{thread_id}
        , thread_name_{std::move(thread_name)}
    {
    }
}
