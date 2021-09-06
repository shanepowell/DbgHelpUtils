#pragma once
#include <cstdint>

#include "symbol_type_info.h"

namespace dlg_help_utils::stream_stack_dump
{
    class mini_dump_stack_walk;
}

namespace dlg_help_utils::process
{

    class process_parameters
    {
    public:
        process_parameters(stream_stack_dump::mini_dump_stack_walk const& walker, uint64_t process_parameters_address);

        [[nodiscard]] stream_stack_dump::mini_dump_stack_walk const& walker() const { return walker_; }
        [[nodiscard]] uint64_t process_parameters_address() const { return process_parameters_address_; }

        [[nodiscard]] std::experimental::generator<std::wstring> environment() const;

    private:
        stream_stack_dump::mini_dump_stack_walk const& walker_;
        uint64_t process_parameters_address_;
        dbg_help::symbol_type_info const process_parameters_symbol_info_;
    };

}
