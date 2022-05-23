#pragma once
#include "global_variable.h"
#include "process_heap_graph_heap_entry.h"

namespace dlg_help_utils::heap
{
    class process_heap_graph_global_variable_entry : public process_heap_graph_node
    {
    public:
        process_heap_graph_global_variable_entry(process::global_variable variable, std::optional<process_heap_graph_heap_entry> base_heap_entry);
        [[nodiscard]] process::global_variable const& variable() const { return variable_; }
        [[nodiscard]] std::wstring get_sort_key() const { return name_; }
        [[nodiscard]] std::optional<process_heap_graph_heap_entry> const& base_heap_entry() const { return base_heap_entry_; }

    private:
        [[nodiscard]] std::wstring generate_global_variable_name() const;

    private:
        process::global_variable variable_;
        std::wstring name_;
        std::optional<process_heap_graph_heap_entry> base_heap_entry_;
    };

    inline std::strong_ordering operator<=>(process_heap_graph_global_variable_entry const& a, process_heap_graph_global_variable_entry const& b)
    {
        return a.get_sort_key() <=> b.get_sort_key();
    }

    inline bool operator<(process_heap_graph_global_variable_entry const& a, process_heap_graph_global_variable_entry const& b)
    {
        return a.get_sort_key() < b.get_sort_key();
    }

    inline bool operator==(process_heap_graph_global_variable_entry const& a, process_heap_graph_global_variable_entry const& b)
    {
        return a.get_sort_key() == b.get_sort_key();
    }
}