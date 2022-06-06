#pragma once
#include "mini_dump_memory_stream.h"
#include "process_heap_graph_node.h"

namespace dlg_help_utils::heap::allocation_graph
{

    class process_heap_graph_thread_stack_entry : public process_heap_graph_node
    {
    public:
        process_heap_graph_thread_stack_entry(mini_dump_memory_stream stack_stream, uint32_t thread_id, std::wstring thread_name);

        [[nodiscard]] mini_dump_memory_stream stack_stream() const { return stack_stream_; }
        [[nodiscard]] uint32_t thread_id() const { return thread_id_; }
        [[nodiscard]] std::wstring const& thread_name() const { return thread_name_; }

    private:
        mini_dump_memory_stream stack_stream_;
        uint32_t thread_id_;
        std::wstring thread_name_;
    };

}
