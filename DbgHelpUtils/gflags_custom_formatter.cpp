#include "gflags_custom_formatter.h"

#include <format>

#include "gflags_utils.h"
#include "mini_dump_memory_stream.h"
#include "string_compare.h"

namespace dlg_help_utils::ntdll_utilities
{
    bool gflags_custom_formatter::is_custom_type(
        [[maybe_unused]] stream_stack_dump::mini_dump_memory_walker const& walker
        , [[maybe_unused]] dbg_help::symbol_type_info const& type 
        , std::wstring_view const& path
        , [[maybe_unused]] std::wstring_view const& name
        , [[maybe_unused]] std::vector<dbg_help::symbol_type_info> const& parents)
    {
        return string_utils::equals(path, L"ntdll!_PEB/NtGlobalFlag");
    }

    symbol_type_utils::dump_variable_symbol_data_result gflags_custom_formatter::format(
        std::function<std::wstring()> original_render_line
        , [[maybe_unused]] stream_stack_dump::mini_dump_memory_walker const& walker
        , [[maybe_unused]] symbol_type_utils::symbol_visit_flags::flags const options
        , [[maybe_unused]] dbg_help::symbol_type_info const& type
        , [[maybe_unused]] dbg_help::sym_tag_enum const tag
        , [[maybe_unused]] uint64_t const variable_address
        , mini_dump_memory_stream& variable_stream
        , [[maybe_unused]] std::wstring_view const& path
        , [[maybe_unused]] std::wstring_view const& name
        , [[maybe_unused]] std::unordered_set<uint64_t>& visited_pointers
        , [[maybe_unused]] size_t const max_symbol_dump_depth
        , [[maybe_unused]] std::vector<dbg_help::symbol_type_info> const& parents
        , [[maybe_unused]] symbol_type_utils::symbol_data_dumper const& dumper)
    {
        if(variable_stream.eof())
        {
            return {};
        }

        uint32_t value{};
        if(variable_stream.read(&value, sizeof(value)) != sizeof(value))
        {
            return {};
        }

        return
        {
            .result = symbol_type_utils::symbol_type_custom_formatter_result::stop,
            .render_line = std::move(original_render_line),
            .sub_lines = [nt_global_flag = static_cast<gflags_utils::gflags>(value)] 
                {
                    return values(nt_global_flag);
                }
        };
    }

    generator<symbol_type_utils::dump_variable_symbol_data> gflags_custom_formatter::values(gflags_utils::gflags const nt_global_flag)
    {
        for (auto&& flag : gflags_utils::dump_gflags_to_strings(nt_global_flag))
        {
            co_yield symbol_type_utils::dump_variable_symbol_data
            {
                .render_line = [flag] { return flag; },
                .sub_lines = {}
            };
        }
    }
}
