#pragma once
#include <functional>

#include "symbol_type_info.h"

namespace dlg_help_utils
{
    class mini_dump_memory_stream;
}

namespace dlg_help_utils::stream_stack_dump
{
    class mini_dump_memory_walker;
}

namespace dlg_help_utils::symbol_type_utils
{
    class i_value_type_formatter;

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

    class symbol_type_custom_formatter  // NOLINT(cppcoreguidelines-special-member-functions)
    {
    public:
        virtual ~symbol_type_custom_formatter() = default;

        virtual bool is_custom_type(dbg_help::symbol_type_info const& type) = 0;
        virtual symbol_type_custom_formatter_result format(
            std::function<std::wstring()>& render_line
            , std::function<generator<dump_variable_symbol_data>()>& sub_lines
            , std::function<std::wstring()> original_render_line
            , stream_stack_dump::mini_dump_memory_walker const& walker
            , dbg_help::symbol_type_info const& type
            , dbg_help::sym_tag_enum tag
            , uint64_t variable_address
            , mini_dump_memory_stream& variable_stream
            , std::wstring_view const& path
            , std::wstring_view const& name
            , std::vector<dbg_help::symbol_type_info> const& parents
            , i_value_type_formatter const& formatter) = 0;
    };
}
