#pragma once
#include <cstdint>
#include <map>
#include <string>

#include "symbol_type_info.h"
#include "tagged_bool.h"

namespace dlg_help_utils::heap::allocation_graph
{
    struct variable_symbol_data
    {
        uint64_t variable_offset{};
        uint64_t variable_address{};
        dbg_help::symbol_type_info variable_symbol;
        std::wstring name;
    };

    using is_root_symbol_t = tagged_bool<struct is_root_symbol_type>;
    using is_metadata_symbol_t = tagged_bool<struct is_metadata_symbol_type>;

    class process_heap_entry_symbol_address_reference
    {
    public:
        process_heap_entry_symbol_address_reference(is_root_symbol_t is_root_symbol, is_metadata_symbol_t is_metadata_symbol, uint64_t address, uint64_t node_index, dbg_help::symbol_type_info symbol_type, uint32_t thread_id, std::wstring thread_name);
        process_heap_entry_symbol_address_reference(is_root_symbol_t is_root_symbol, is_metadata_symbol_t is_metadata_symbol, uint64_t address, uint64_t node_index, dbg_help::symbol_type_info symbol_type);

        [[nodiscard]] uint64_t address() const { return address_; }
        [[nodiscard]] uint64_t node_index() const { return node_index_; }
        [[nodiscard]] bool is_root_symbol() const { return is_root_symbol_; }
        [[nodiscard]] bool is_metadata_symbol() const { return is_metadata_symbol_; }
        [[nodiscard]] dbg_help::symbol_type_info const& symbol_type() const { return symbol_type_; }
        [[nodiscard]] std::optional<uint32_t> const& thread_id() const { return thread_id_; }
        [[nodiscard]] std::wstring const& thread_name() const { return thread_name_; }

        void add_variable_symbol_reference(uint64_t variable_address, uint64_t variable_address_offset, size_t pointer_size, dbg_help::symbol_type_info pointer_symbol_type, std::wstring name);
        [[nodiscard]] std::map<uint64_t, variable_symbol_data> const& variable_symbol_references() const { return variable_symbol_references_; }

    private:
        uint64_t address_;
        uint64_t node_index_;
        bool is_root_symbol_;
        bool is_metadata_symbol_;
        dbg_help::symbol_type_info symbol_type_;
        std::optional<uint32_t> thread_id_;
        std::wstring thread_name_;
        std::map<uint64_t, variable_symbol_data> variable_symbol_references_;
    };
}
