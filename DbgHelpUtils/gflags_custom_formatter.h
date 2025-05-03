#pragma once
#include "gflags_utils.h"
#include "symbol_type_custom_formatter.h"


namespace dlg_help_utils::ntdll_utilities
{
    class gflags_custom_formatter : public symbol_type_utils::symbol_type_custom_formatter
    {
    public:
        bool is_custom_type(dbg_help::symbol_type_info const& type
            , std::wstring_view const& path
            , std::wstring_view const& name
            , std::vector<dbg_help::symbol_type_info> const& parents) override;

        symbol_type_utils::dump_variable_symbol_data_result format(
            std::function<std::wstring()> original_render_line
            , stream_stack_dump::mini_dump_memory_walker const& walker
            , dbg_help::symbol_type_info const& type
            , dbg_help::sym_tag_enum tag
            , uint64_t variable_address
            , mini_dump_memory_stream& variable_stream
            , std::wstring_view const& pat
            , std::wstring_view const& name
            , std::vector<dbg_help::symbol_type_info> const& parents
            , i_value_type_formatter const& formatter) override;

    private:
        static generator<symbol_type_utils::dump_variable_symbol_data> values(gflags_utils::gflags nt_global_flag);
    };
}
