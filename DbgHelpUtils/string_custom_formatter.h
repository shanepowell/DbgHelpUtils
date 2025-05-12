#pragma once

#include "stream_utils.h"
#include "symbol_type_custom_formatter.h"

namespace dlg_help_utils
{
    class cache_manager;
}

namespace dlg_help_utils::ntdll_utilities
{
    class string_custom_formatter : public symbol_type_utils::symbol_type_custom_formatter
    {
    public:
        string_custom_formatter(cache_manager& cache);

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
        struct cache_data
        {
            dbg_help::symbol_type_info string_symbol_type;
            stream_utils::symbol_type_and_base_type_field_offset length_field_data;
            stream_utils::symbol_type_and_base_type_field_offset maximum_length_field_data;
            stream_utils::symbol_type_and_base_type_field_offset buffer_field_data;
        };

        [[nodiscard]] cache_data const& get_cache_data(stream_stack_dump::mini_dump_memory_walker const& walker) const;
        [[nodiscard]] cache_data const& setup_globals(stream_stack_dump::mini_dump_memory_walker const& walker) const;

    private:
        cache_manager* cache_manager_;
        mutable cache_data const* cache_data_{nullptr};
    };
}
