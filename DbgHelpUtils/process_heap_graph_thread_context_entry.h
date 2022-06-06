#pragma once
#include <cvconst.h>
#include <string>

#include "process_heap_graph_node.h"

namespace dlg_help_utils::heap::allocation_graph
{
    class process_heap_graph_thread_context_entry : public process_heap_graph_node
    {
    public:
        process_heap_graph_thread_context_entry(CV_HREG_e register_type, uint64_t register_data, uint32_t thread_id, std::wstring thread_name);

        [[nodiscard]] CV_HREG_e register_type() const { return register_type_; }
        [[nodiscard]] uint64_t register_data() const { return register_data_; }
        [[nodiscard]] uint32_t thread_id() const { return thread_id_; }
        [[nodiscard]] std::wstring const& thread_name() const { return thread_name_; }

    private:
        CV_HREG_e register_type_;
        uint64_t register_data_;
        uint32_t thread_id_;
        std::wstring thread_name_;
    };
}
