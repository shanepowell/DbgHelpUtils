#pragma once
#include <optional>
#include <string>

namespace dlg_help_utils
{
    namespace dbg_help
    {
        class symbol_type_info;
    }
}

namespace dlg_help_utils
{
    namespace stream_stack_dump
    {
        class mini_dump_stack_walk;
    }
}

std::optional<dlg_help_utils::dbg_help::symbol_type_info> get_type_info(dlg_help_utils::stream_stack_dump::mini_dump_stack_walk const& walker, std::wstring const& symbol_type_name);
std::optional<std::pair<dlg_help_utils::dbg_help::symbol_type_info, uint64_t>> find_field_pointer_and_type(dlg_help_utils::stream_stack_dump::mini_dump_stack_walk const& walker, dlg_help_utils::dbg_help::symbol_type_info const& type_symbol_info, std::wstring const& symbol_type_name, uint64_t address, std::wstring_view field_name);
std::optional<uint64_t> find_field_pointer(dlg_help_utils::stream_stack_dump::mini_dump_stack_walk const& walker, dlg_help_utils::dbg_help::symbol_type_info const& type_symbol_info, std::wstring const& symbol_type_name, uint64_t address, std::wstring_view field_name);
std::optional<uint64_t> find_field_pointer(dlg_help_utils::stream_stack_dump::mini_dump_stack_walk const& walker, std::wstring const& symbol_type_name, uint64_t address, std::wstring_view field_name);
std::optional<dlg_help_utils::dbg_help::symbol_type_info> dump_field(dlg_help_utils::stream_stack_dump::mini_dump_stack_walk const& walker, std::wstring const& symbol_type_name, uint64_t address);
