#include "process_heap_graph_thread_stack_entry.h"

namespace dlg_help_utils::heap
{
    process_heap_graph_thread_stack_entry::process_heap_graph_thread_stack_entry(mini_dump_memory_stream stack_stream, uint32_t const thread_id, std::wstring thread_name)
        : process_heap_graph_node{true}
        , stack_stream_{std::move(stack_stream)}
        , thread_id_{thread_id}
        , thread_name_{std::move(thread_name)}
    {
    }
}
