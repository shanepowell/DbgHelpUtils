#pragma once
#include <vector>

#include "symbol_type_info.h"

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
    class mini_dump_stack_walk;
}

namespace dlg_help_utils::heap
{
    class ust_address_stack_trace
    {
    public:
        ust_address_stack_trace(stream_stack_dump::mini_dump_stack_walk const& walker);

        [[nodiscard]] std::vector<uint64_t> read_allocation_stack_trace(process::process_environment_block const& peb, uint64_t ust_address) const;

        [[nodiscard]] static bool is_valid_ust_address(process::process_environment_block const& peb, uint64_t ust_address);
        [[nodiscard]] static bool has_ust_setup(process::process_environment_block const& peb);

    private:
        [[nodiscard]] dbg_help::symbol_type_info get_rtl_stack_trace_entry_symbol_type() const;
        [[nodiscard]] static std::wstring const& get_ust_address_depth_field(process::process_environment_block const& peb);

    private:
        stream_stack_dump::mini_dump_stack_walk const& walker_;
        dbg_help::symbol_type_info const rtl_stack_trace_entry_symbol_type_{get_rtl_stack_trace_entry_symbol_type()};
    };
}
