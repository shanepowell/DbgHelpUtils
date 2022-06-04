﻿#include "process_heap_graph_global_variable_entry.h"

#include <format>

using namespace std::string_literals;

namespace dlg_help_utils::heap
{
    process_heap_graph_global_variable_entry::process_heap_graph_global_variable_entry(process::global_variable variable, std::optional<process_heap_graph_heap_entry> base_heap_entry)
        : process_heap_graph_node{true}
        , variable_{std::move(variable)}
        , name_{generate_global_variable_name()}
        , base_heap_entry_{std::move(base_heap_entry)}
    {
    }

    std::wstring process_heap_graph_global_variable_entry::generate_global_variable_name() const
    {
        auto const name = variable().symbol_type().name();
        if(!name.has_value())
        {
            return std::format(L"<UnknownGlobalVariable:{0}>", variable().symbol_type().sym_index());
        }
        return std::wstring{name.value()};
    }
}