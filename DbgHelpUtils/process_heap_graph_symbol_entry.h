#pragma once
#include <string>

#include "process_heap_graph_node.h"
#include "symbol_type_info.h"

namespace dlg_help_utils::heap::allocation_graph
{
    class process_heap_graph_symbol_entry : public process_heap_graph_node
    {
    public:
        process_heap_graph_symbol_entry(uint64_t address, dbg_help::symbol_type_info symbol_type, std::optional<uint32_t> thread_id = std::nullopt, std::wstring thread_name = {});

        [[nodiscard]] uint64_t address() const { return address_; }
        [[nodiscard]] dbg_help::symbol_type_info const& symbol_type() const { return symbol_type_; }
        [[nodiscard]] std::optional<uint32_t> thread_id() const { return thread_id_; }
        [[nodiscard]] std::wstring const& thread_name() const { return thread_name_; }

    private:
        uint64_t address_;
        dbg_help::symbol_type_info symbol_type_;
        std::optional<uint32_t> thread_id_;
        std::wstring thread_name_;
    };
}
