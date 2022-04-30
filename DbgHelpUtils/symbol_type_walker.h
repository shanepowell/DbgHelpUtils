#pragma once
#include <string>
#include <vector>

namespace dlg_help_utils::dbg_help
{
    class symbol_type_info;
    enum class sym_tag_enum;
}

namespace dlg_help_utils::stream_stack_dump
{
    class mini_dump_memory_walker;

    class symbol_type_walker
    {
    public:
        symbol_type_walker(mini_dump_memory_walker const& walker, std::wstring const& match_pattern = {});

        [[nodiscard]] std::vector<dbg_help::symbol_type_info> const& all_symbols() const { return symbols_; }

    private:
        std::vector<dbg_help::symbol_type_info> symbols_;
    };
}
