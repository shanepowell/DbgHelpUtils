#include "list_entry_custom_formatter.h"

#include "cache_manager.h"
#include "common_symbol_names.h"
#include "i_value_type_formatter.h"
#include "list_entry_walker.h"
#include "symbol_type_utils.h"

using namespace std::literals;

namespace dlg_help_utils::ntdll_utilities
{
    std::wstring const& list_entry_custom_formatter::symbol_name = common_symbol_names::list_entry_structure_symbol_name;
    const std::unordered_map<std::wstring_view, list_entry_custom_formatter::known_name_type> list_entry_custom_formatter::g_known_names =
        {
            {L"ntdll!_PEB/*Ldr/InLoadOrderModuleList"sv, {.symbol_name = L"ntdll!_LDR_DATA_TABLE_ENTRY"s, .field_name = L"InLoadOrderLinks"s } },
            {L"ntdll!_PEB/*Ldr/InMemoryOrderModuleList"sv, {.symbol_name = L"ntdll!_LDR_DATA_TABLE_ENTRY"s, .field_name = L"InMemoryOrderLinks"s } },
            {L"ntdll!_PEB/*/Ldr/InInitializationOrderModuleList"sv, {.symbol_name = L"ntdll!_LDR_DATA_TABLE_ENTRY"s, .field_name = L"InInitializationOrderLinks"s } },
            // ReSharper disable once GrammarMistakeInComment
            //{L"ntdll!_TEB/*ProcessEnvironmentBlock/*Ldr/InLoadOrderModuleList"sv, {.symbol_name = L"ntdll!_LDR_DATA_TABLE_ENTRY"s, .field_name = L"InLoadOrderLinks"s } },
            //{L"ntdll!_TEB/*ProcessEnvironmentBlock/*Ldr/InMemoryOrderModuleList"sv, {.symbol_name = L"ntdll!_LDR_DATA_TABLE_ENTRY"s, .field_name = L"InMemoryOrderLinks"s } },
            //{L"ntdll!_TEB/*ProcessEnvironmentBlock/*Ldr/InInitializationOrderModuleList"sv, {.symbol_name = L"ntdll!_LDR_DATA_TABLE_ENTRY"s, .field_name = L"InInitializationOrderLinks"s } }
        };

    list_entry_custom_formatter::list_entry_custom_formatter(cache_manager& cache)
    : cache_manager_{&cache}
    {
    }

    bool list_entry_custom_formatter::is_custom_type(
        stream_stack_dump::mini_dump_memory_walker const& walker
        , dbg_help::symbol_type_info const& type
        , [[maybe_unused]] std::wstring_view const& path
        , [[maybe_unused]] std::wstring_view const& name
        , [[maybe_unused]] std::vector<dbg_help::symbol_type_info> const& parents)
    {
        return is_type_equal(walker, type, path, name, parents, get_cache_data(walker).list_entry_symbol_type);
    }

    symbol_type_utils::dump_variable_symbol_data_result list_entry_custom_formatter::format(
        std::function<std::wstring()> original_render_line
        , stream_stack_dump::mini_dump_memory_walker const& walker
        , symbol_type_utils::symbol_visit_flags::flags const options
        , dbg_help::symbol_type_info const& type
        , [[maybe_unused]] dbg_help::sym_tag_enum const tag
        , uint64_t variable_address
        , mini_dump_memory_stream& variable_stream
        , std::wstring_view const& path
        , std::wstring_view const& name
        , std::unordered_set<uint64_t>& visited_pointers
        , size_t const max_symbol_dump_depth
        , std::vector<dbg_help::symbol_type_info> const& parents
        , symbol_type_utils::symbol_data_dumper const& dumper)
    {
        variable_address = get_address(walker, type, variable_address, variable_stream, dumper.formatter());

        if (walker.find_memory_range(variable_address, get_cache_data(walker).list_entry_symbol_type.length().value_or(1),1)  == 0)
        {
            return
            {
                .result = symbol_type_utils::symbol_type_custom_formatter_result::stop,
                .render_line = [original_render_line = std::move(original_render_line), variable_address, &formatter = dumper.formatter()]
                {
                    // ReSharper disable once StringLiteralTypo
                    return std::format(L"{}: {} - {}", original_render_line(), formatter.format_pointer_value(variable_address, false), symbol_type_utils::resources::get_variable_unknown());
                },
                .sub_lines = {}
            };
        }

        if (auto it = g_known_names.find(path);
            it != g_known_names.end())
        {
            list_entry_walker list_entry_walker{cache(), walker, variable_address, it->second.symbol_name, it->second.field_name};

            return
            {
                .result = symbol_type_utils::symbol_type_custom_formatter_result::stop,
                .render_line = [original_render_line = std::move(original_render_line), count = list_entry_walker.size(), &formatter = dumper.formatter()]
                {
                    // ReSharper disable once StringLiteralTypo
                    return std::format(L"{}: [{}]", original_render_line(), formatter.format_value(count));
                },
                .sub_lines = [list_entry_walker, &dumper, &walker, options, path, name, visited_pointers, max_symbol_dump_depth, parents] () mutable 
                {
                    return generate_list_children_of_type(
                        list_entry_walker, 
                        dumper, 
                        walker, 
                        options, 
                        path, 
                        name,
                        visited_pointers,
                        max_symbol_dump_depth,
                        parents);
                }
            };
        }

        list_entry_walker list_entry_walker{cache(), walker, variable_address};

        return
        {
            .result = symbol_type_utils::symbol_type_custom_formatter_result::stop,
            .render_line = [original_render_line = std::move(original_render_line), count = list_entry_walker.size(), &formatter = dumper.formatter()]
            {
                // ReSharper disable once StringLiteralTypo
                return std::format(L"{}: [{}]", original_render_line(), formatter.format_value(count));
            },
            .sub_lines = [list_entry_walker, &formatter = dumper.formatter()]
            {
                return generate_list_children(list_entry_walker, formatter);
            }
        };
    }

    generator<symbol_type_utils::dump_variable_symbol_data> list_entry_custom_formatter::generate_list_children_of_type(
        list_entry_walker const& list_entry_walker
        , symbol_type_utils::symbol_data_dumper const& dumper
        , stream_stack_dump::mini_dump_memory_walker const& walker
        , symbol_type_utils::symbol_visit_flags::flags const options
        , std::wstring_view const& path
        , std::wstring_view const& name
        , std::unordered_set<uint64_t>& visited_pointers
        , size_t const max_symbol_dump_depth
        , std::vector<dbg_help::symbol_type_info> const& parents)
    {
        uint64_t index = 0;
        for (auto const entry_address : list_entry_walker.entries())
        {
            co_yield dumper.variable_symbol_at(
                walker, 
                options,
                std::format(L"[{}]: ", index),
                list_entry_walker.entry_type(),
                list_entry_walker.entry_type(),
                entry_address,
                walker.get_process_memory_stream(entry_address, list_entry_walker.entry_type().length().value_or(1)),
                symbol_type_utils::is_head_t{true},
                std::format(L"{}[{}]", path, index),
                std::format(L"{}[{}]", name, index),
                visited_pointers,
                max_symbol_dump_depth,
                parents);
            index++;
        }
    }

    generator<symbol_type_utils::dump_variable_symbol_data> list_entry_custom_formatter::generate_list_children(list_entry_walker const& list_entry_walker, i_value_type_formatter const& formatter)
    {
        uint64_t index = 0;
        for (auto const entry_address : list_entry_walker.entries())
        {
            co_yield symbol_type_utils::dump_variable_symbol_data
            {
                .render_line = [index, entry_address, &formatter]
                {
                    // ReSharper disable once StringLiteralTypo
                    return std::format(L"[{}]: {}", index, formatter.format_pointer_value(entry_address, true));
                },
                .sub_lines = {}
            };

            index++;
        }
    }

    list_entry_custom_formatter::cache_data const& list_entry_custom_formatter::get_cache_data(stream_stack_dump::mini_dump_memory_walker const& walker) const
    {
        if (!cache_data_)
        {
            cache_data_ = &setup_globals(walker);
        }
        return *cache_data_;
    }

    list_entry_custom_formatter::cache_data const& list_entry_custom_formatter::setup_globals(stream_stack_dump::mini_dump_memory_walker const& walker) const
    {
        if(!cache().has_cache<cache_data>())
        {
            auto& data = cache().get_cache<cache_data>();
            data.list_entry_symbol_type = stream_utils::get_type(walker, symbol_name);
        }

        return cache().get_cache<cache_data>();
    }
}
