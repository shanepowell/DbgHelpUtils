#pragma once
#include <functional>
#include <unordered_set>

#include "symbol_type_info.h"
#include "symbol_visit_flags.h"

namespace dlg_help_utils
{
    class mini_dump_memory_stream;
    class i_value_type_formatter;
}

namespace dlg_help_utils::stream_stack_dump
{
    class mini_dump_memory_walker;
}

namespace dlg_help_utils::symbol_type_utils
{
    class symbol_data_dumper;

    enum class symbol_type_custom_formatter_result : uint8_t
    {
        process,
        completed,
        stop,
    };

    struct dump_variable_symbol_data
    {
        std::function<std::wstring()> render_line;
        std::function<generator<dump_variable_symbol_data>()> sub_lines;
    };

    struct dump_variable_symbol_data_result
    {
        symbol_type_custom_formatter_result result{symbol_type_custom_formatter_result::process};
        std::function<std::wstring()> render_line{};
        std::function<generator<dump_variable_symbol_data>()> sub_lines{};
    };

    class symbol_type_custom_formatter  // NOLINT(cppcoreguidelines-special-member-functions)
    {
    public:
        virtual ~symbol_type_custom_formatter() = default;

        virtual bool is_custom_type(
            stream_stack_dump::mini_dump_memory_walker const& walker
            , dbg_help::symbol_type_info const& type
            , std::wstring_view const& path
            , std::wstring_view const& name
            , std::vector<dbg_help::symbol_type_info> const& parents) = 0;

        virtual dump_variable_symbol_data_result format(
            std::function<std::wstring()> original_render_line
            , stream_stack_dump::mini_dump_memory_walker const& walker
            , symbol_visit_flags::flags options
            , dbg_help::symbol_type_info const& type
            , dbg_help::sym_tag_enum tag
            , uint64_t variable_address
            , mini_dump_memory_stream& variable_stream
            , std::wstring_view const& path
            , std::wstring_view const& name
            , std::unordered_set<uint64_t>& visited_pointers
            , size_t max_symbol_dump_depth
            , std::vector<dbg_help::symbol_type_info> const& parents
            , symbol_data_dumper const& dumper) = 0;

    protected:
        bool is_type_equal(
            stream_stack_dump::mini_dump_memory_walker const& walker
            , dbg_help::symbol_type_info const& type
            , std::wstring_view const& path
            , std::wstring_view const& name
            , std::vector<dbg_help::symbol_type_info> const& parents
            , dbg_help::symbol_type_info const& equal_to_type);

        static uint64_t get_address(
            stream_stack_dump::mini_dump_memory_walker const& walker
            , dbg_help::symbol_type_info const& type
            , uint64_t variable_address
            , mini_dump_memory_stream& variable_stream
            , i_value_type_formatter const& formatter);

    };
}
