#pragma once

#include "stream_utils.h"
#include "symbol_type_custom_formatter.h"

namespace dlg_help_utils
{
    class cache_manager;
}

namespace dlg_help_utils::ntdll_utilities
{
    class single_list_entry_walker;

    class single_list_entry_custom_formatter : public symbol_type_utils::symbol_type_custom_formatter
    {
    public:
        single_list_entry_custom_formatter(cache_manager& cache);

        bool is_custom_type(
            stream_stack_dump::mini_dump_memory_walker const& walker
            , dbg_help::symbol_type_info const& type
            , std::wstring_view const& path
            , std::wstring_view const& name
            , std::vector<dbg_help::symbol_type_info> const& parents) override;

        symbol_type_utils::dump_variable_symbol_data_result format(
            std::function<std::wstring()> original_render_line
            , stream_stack_dump::mini_dump_memory_walker const& walker
            , symbol_type_utils::symbol_visit_flags::flags options
            , dbg_help::symbol_type_info const& type
            , dbg_help::sym_tag_enum tag
            , uint64_t variable_address
            , mini_dump_memory_stream& variable_stream
            , std::wstring_view const& pat
            , std::wstring_view const& name
            , std::unordered_set<uint64_t>& visited_pointers
            , size_t max_symbol_dump_depth
            , std::vector<dbg_help::symbol_type_info> const& parents
            , symbol_type_utils::symbol_data_dumper const& dumper) override;

        [[nodiscard]] cache_manager& cache() const { return *cache_manager_; }

        static std::wstring const& symbol_name;

    private:
        static generator<symbol_type_utils::dump_variable_symbol_data> generate_list_children_of_type(
            single_list_entry_walker const& list_entry_walker
            , symbol_type_utils::symbol_data_dumper const& dumper
            , stream_stack_dump::mini_dump_memory_walker const& walker
            , symbol_type_utils::symbol_visit_flags::flags options
            , std::wstring_view const& path
            , std::wstring_view const& name
            , std::unordered_set<uint64_t>& visited_pointers
            , size_t max_symbol_dump_depth
            , std::vector<dbg_help::symbol_type_info> const& parents);
        static generator<symbol_type_utils::dump_variable_symbol_data> generate_list_children(single_list_entry_walker const& list_entry_walker, i_value_type_formatter const& formatter);

        struct cache_data
        {
            dbg_help::symbol_type_info single_list_entry_symbol_type;
        };

        [[nodiscard]] cache_data const& get_cache_data(stream_stack_dump::mini_dump_memory_walker const& walker) const;
        [[nodiscard]] cache_data const& setup_globals(stream_stack_dump::mini_dump_memory_walker const& walker) const;

    private:
        cache_manager* cache_manager_;
        mutable cache_data const* cache_data_{nullptr};

        struct known_name_type
        {
            std::wstring symbol_name;
            std::wstring field_name;
        };

        static const std::unordered_map<std::wstring_view, known_name_type> g_known_names;
    };
}