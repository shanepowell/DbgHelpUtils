#pragma once
#include <map>
#include <string>
#include <unordered_set>
#include <functional>

#include "call_convention.h"
#include "data_kind.h"
#include "basic_type.h"
#include "symbol_type_info.h"
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
}

namespace dlg_help_utils::symbol_type_utils
{
    std::wstring get_symbol_type_friendly_name(dbg_help::symbol_type_info const& value);
    void dump_variable_type_at(std::wostream& os
        , mini_dump const& mini_dump
        , dbg_help::symbol_engine& symbol_engine
        , std::wstring const& symbol_type_name
        , uint64_t variable_address
        , size_t indent = 0
        , size_t visited_depth = 0);
    void dump_variable_type_at(std::wostream& os
        , mini_dump const& mini_dump
        , dbg_help::symbol_engine& symbol_engine
        , dbg_help::symbol_type_info const& symbol_info
        , uint64_t variable_address
        , size_t indent = 0
        , size_t visited_depth = 0);

    enum class dump_variable_symbol_options : uint8_t
    {
        NoHeader,
        ForceHeader,
        AutoHeader
    };

    struct dump_variable_symbol_data
    {
        std::wstring line;
        std::function<std::generator<dump_variable_symbol_data>()> sub_lines;
    };

    dump_variable_symbol_data variable_symbol_at(stream_stack_dump::mini_dump_memory_walker const& walker
        , std::wstring_view const& prefix
        , dbg_help::symbol_type_info const& type
        , dbg_help::symbol_type_info const& display_type
        , uint64_t variable_address
        , mini_dump_memory_stream const& variable_stream
        , size_t visited_depth = 0);

    void dump_variable_symbol_at(std::wostream& os
        , stream_stack_dump::mini_dump_memory_walker const& walker
        , dbg_help::symbol_type_info const& type
        , dbg_help::symbol_type_info const& display_type
        , uint64_t variable_address
        , mini_dump_memory_stream const& variable_stream
        , size_t indent = 0
        , size_t visited_depth = 0
        , dump_variable_symbol_options options = dump_variable_symbol_options::AutoHeader);
    std::tuple<uint64_t, std::wstring, uint64_t, std::wstring> parse_address(std::wstring const& address);

    struct pointer_info
    {
        std::wstring name;
        uint64_t base_address;
        uint64_t base_variable_address_offset;
        dbg_help::symbol_type_info base_variable_type;
        uint64_t pointer_address;
        dbg_help::symbol_type_info pointer_type;
    };

    void gather_all_pointers_from_symbol(stream_stack_dump::mini_dump_memory_walker const& walker
        , dbg_help::symbol_type_info const& base_type
        , dbg_help::symbol_type_info const& type
        , uint64_t base_address
        , uint64_t variable_address_offset
        , std::map<uint64_t, pointer_info>& pointers
        , std::wstring const& name_prefix
        , std::unordered_set<uint64_t> const& ignore_pointers);

    namespace resources
    {
        std::wstring sym_tag_to_string(dbg_help::sym_tag_enum type);
        std::wstring basic_type_to_string(dbg_help::basic_type type);
        std::wstring udt_kind_to_string(dbg_help::udt_kind_type type);
        std::wstring calling_convention_to_string(dbg_help::calling_convention type);
        std::wstring data_kind_convention_to_string(dbg_help::data_kind type);
        std::wstring_view get_symbol_name_none();
        std::wstring get_unsupported_type();
        std::wstring get_on_type();
        std::wstring get_variable_unknown();
        std::wstring get_bcd_value_unsupported();
        std::wstring get_currency_value_unsupported();
        std::wstring get_date_value_unsupported();
        std::wstring get_variant_value_unsupported();
        std::wstring get_complex_value_unsupported();
        std::wstring get_bstr_value_unsupported();
        std::wstring get_bits_title();
        std::wstring get_bit_title();
        std::wstring get_enum_title();
        std::wstring get_reference_title();
        std::wstring get_pointer_title();
        std::wstring get_int8_title();
        std::wstring get_int16_title();
        std::wstring get_int32_title();
        std::wstring get_int64_title();
        std::wstring get_int128_title();
        std::wstring get_int256_title();
        std::wstring get_uint8_title();
        std::wstring get_uint16_title();
        std::wstring get_uint32_title();
        std::wstring get_uint64_title();
        std::wstring get_uint128_title();
        std::wstring get_uint256_title();
        std::wstring get_float32_title();
        std::wstring get_float64_title();
        std::wstring get_float128_title();
        std::wstring get_float256_title();
        std::wstring get_failed_to_find_string();
        std::wstring get_symbol_title();
        std::wstring get_is_zero_length_string();
        std::wstring get_memory_address_in_range_string();
        std::wstring get_fields_string();
    }
}
