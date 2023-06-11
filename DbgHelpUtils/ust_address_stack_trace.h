﻿#pragma once
// ReSharper disable CppClangTidyCppcoreguidelinesAvoidConstOrRefDataMembers

#include <vector>

#include "stream_utils.h"
#include "symbol_type_info.h"

namespace dlg_help_utils
{
    class cache_manager;
}

namespace dlg_help_utils::process
{
    class process_environment_block;
}

namespace dlg_help_utils::gflags_utils
{
    enum class gflags : uint32_t;
}

namespace dlg_help_utils::stream_stack_dump
{
    class mini_dump_memory_walker;
}

namespace dlg_help_utils::heap
{
    class ust_address_stack_trace
    {
    public:
        ust_address_stack_trace(cache_manager& cache, stream_stack_dump::mini_dump_memory_walker const& walker);

        [[nodiscard]] std::vector<uint64_t> read_allocation_stack_trace(process::process_environment_block const& peb, uint64_t ust_address) const;

        [[nodiscard]] bool is_valid_ust_address(process::process_environment_block const& peb, uint64_t ust_address) const;
        [[nodiscard]] static bool has_ust_setup(process::process_environment_block const& peb);

    private:
        [[nodiscard]] stream_utils::symbol_type_and_base_type_field_offset const& get_ust_address_depth_field(process::process_environment_block const& peb) const;

        struct cache_data
        {
            dbg_help::symbol_type_info rtl_stack_trace_entry_symbol_type;
            size_t rtl_stack_trace_entry_symbol_length{};

            stream_utils::symbol_type_and_base_type_field_offset rtl_stack_trace_entry_depth_field_data;
            stream_utils::symbol_type_and_base_type_field_offset rtl_stack_trace_entry_index_field_data;
            stream_utils::symbol_type_and_base_type_field_offset rtl_stack_trace_entry_back_trace_field_data;
        };
        [[nodiscard]] cache_data const& setup_globals() const;

    private:
        cache_manager& cache_manager_;
        stream_stack_dump::mini_dump_memory_walker const& walker_;
        cache_data const& cache_data_{setup_globals()};
    };
}
