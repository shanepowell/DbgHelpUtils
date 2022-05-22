#include "process_heap_graph_global_variable_entry.h"

#include <format>

using namespace std::string_literals;

namespace dlg_help_utils::heap
{
    process_heap_graph_global_variable_entry::process_heap_graph_global_variable_entry(process::global_variable variable)
        : process_heap_graph_node{true}
        , variable_{std::move(variable)}
        , name_{generate_global_variable_name()}
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
