#include "process_heap_entry_symbol_address_reference.h"

namespace dlg_help_utils::heap::allocation_graph
{
    process_heap_entry_symbol_address_reference::process_heap_entry_symbol_address_reference(uint64_t const address, dbg_help::symbol_type_info symbol_type, uint32_t const thread_id, std::wstring thread_name)
        : address_{address}
        , symbol_type_{std::move(symbol_type)}
        , thread_id_{thread_id}
        , thread_name_{std::move(thread_name)}
    {
    }

    process_heap_entry_symbol_address_reference::process_heap_entry_symbol_address_reference(uint64_t const address, dbg_help::symbol_type_info symbol_type)
        : address_{address}
        , symbol_type_{std::move(symbol_type)}
    {
    }

    void process_heap_entry_symbol_address_reference::add_variable_symbol_reference(uint64_t const variable_address, uint64_t const variable_address_offset, size_t const pointer_size, dbg_help::symbol_type_info pointer_symbol_type, std::wstring name)
    {
        auto const end_variable_address = variable_address + pointer_size - 1;
        variable_symbol_references_.insert(std::make_pair(end_variable_address, variable_symbol_data{variable_address_offset, variable_address, std::move(pointer_symbol_type), std::move(name)}));
    }
}
