#pragma once
#include <ostream>
#include <optional>
#include <string>

namespace dlg_help_utils
{
    namespace stream_utils
    {
        struct symbol_type_and_value;
    }
    namespace dbg_help
    {
        class symbol_type_info;
    }
}

namespace dlg_help_utils
{
    namespace stream_stack_dump
    {
        class mini_dump_memory_walker;
    }
}

std::optional<dlg_help_utils::dbg_help::symbol_type_info> get_type_info(std::wostream& log, dlg_help_utils::stream_stack_dump::mini_dump_memory_walker const& walker, std::wstring const& symbol_type_name);
std::optional<dlg_help_utils::stream_utils::symbol_type_and_value> find_field_pointer_and_type(std::wostream& log, dlg_help_utils::stream_stack_dump::mini_dump_memory_walker const& walker, dlg_help_utils::dbg_help::symbol_type_info const& type_symbol_info, std::wstring const& symbol_type_name, uint64_t address, std::wstring_view field_name);
std::optional<uint64_t> find_field_pointer(std::wostream& log, dlg_help_utils::stream_stack_dump::mini_dump_memory_walker const& walker, dlg_help_utils::dbg_help::symbol_type_info const& type_symbol_info, std::wstring const& symbol_type_name, uint64_t address, std::wstring_view field_name);
std::optional<uint64_t> find_field_pointer(std::wostream& log, dlg_help_utils::stream_stack_dump::mini_dump_memory_walker const& walker, std::wstring const& symbol_type_name, uint64_t address, std::wstring_view field_name);
std::optional<dlg_help_utils::dbg_help::symbol_type_info> dump_field(std::wostream& log, dlg_help_utils::stream_stack_dump::mini_dump_memory_walker const& walker, std::wstring const& symbol_type_name, uint64_t address);
