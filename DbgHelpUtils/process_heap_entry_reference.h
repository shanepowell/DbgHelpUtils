#pragma once
#include <cstdint>

namespace dlg_help_utils::heap
{
    class process_heap_graph_node;

    class process_heap_entry_reference
    {
    public:
        process_heap_entry_reference(uint64_t offset, uint64_t pointer, process_heap_graph_node const& heap_entry);

        [[nodiscard]] uint64_t node_index() const { return node_index_; }
        [[nodiscard]] uint64_t offset() const { return offset_; }
        [[nodiscard]] uint64_t pointer() const { return pointer_; }

    private:
        uint64_t offset_;
        uint64_t pointer_;
        uint64_t node_index_;
    };
}
