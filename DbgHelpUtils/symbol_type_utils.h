#pragma once
#include <set>
#include <string>
#include <vector>

#include "call_convention.h"
#include "data_kind.h"
#include "basic_type.h"
#include "sym_tag_enum.h"
#include "udt_kind_type.h"

namespace dlg_help_utils
{
    class mini_dump_memory_stream;

    namespace stream_stack_dump
    {
        class mini_dump_memory_walker;
    }

    class mini_dump;
}

namespace dlg_help_utils::dbg_help
{
    class symbol_engine;
    class symbol_type_info;
}

namespace dlg_help_utils::symbol_type_utils
{
    std::wstring_view sym_tag_to_string(dbg_help::sym_tag_enum type);
    std::wstring_view basic_type_to_string(dbg_help::basic_type type);
    std::wstring_view udt_kind_to_string(dbg_help::udt_kind_type type);
    std::wstring_view calling_convention_to_string(dbg_help::calling_convention type);
    std::wstring_view data_kind_convention_to_string(dbg_help::data_kind type);
    std::wstring get_symbol_type_friendly_name(dbg_help::symbol_type_info const& value);
    void dump_variable_type_at(std::wostream& os, mini_dump const& mini_dump, dbg_help::symbol_engine& symbol_engine, std::wstring const& symbol_type_name, uint64_t variable_address, size_t indent = 0, size_t visited_depth = 0);
    void dump_variable_type_at(std::wostream& os, mini_dump const& mini_dump, dbg_help::symbol_engine& symbol_engine, dbg_help::symbol_type_info const& symbol_info, uint64_t variable_address, size_t indent = 0, size_t visited_depth = 0);

    enum class dump_variable_symbol_options
    {
        NoHeader,
        ForceHeader,
        AutoHeader
    };

    void dump_variable_symbol_at(std::wostream& os, stream_stack_dump::mini_dump_memory_walker const& walker, dbg_help::symbol_type_info const& type, dbg_help::symbol_type_info const& display_type, uint64_t variable_address, mini_dump_memory_stream& variable_stream, size_t indent = 0, size_t visited_depth = 0, dump_variable_symbol_options options = dump_variable_symbol_options::AutoHeader);
    std::tuple<uint64_t, std::wstring, uint64_t, std::wstring> parse_address(std::wstring const& address);

    void gather_all_pointers_from_symbol(stream_stack_dump::mini_dump_memory_walker const& walker, dbg_help::symbol_type_info const& type, uint64_t variable_address, std::set<uint64_t>& pointers);
}
