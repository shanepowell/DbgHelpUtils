#pragma once
#include <cstdint>
#include <experimental/generator>

namespace dlg_help_utils::stream_stack_dump
{
    class mini_dump_stack_walk;
}

namespace dlg_help_utils::process
{
    class process_environment_block;
}

namespace dlg_help_utils::heap
{
    class crt_entry;

    class crt_heap
    {
    public:
        crt_heap(process::process_environment_block const& peb);

        [[nodiscard]] process::process_environment_block const& peb() const { return peb_; }
        [[nodiscard]] stream_stack_dump::mini_dump_stack_walk const& walker() const;

        [[nodiscard]] bool is_using_crt_heap() const { return crt_first_block_ != 0; }

        [[nodiscard]] std::experimental::generator<crt_entry> entries() const;

    private:
        [[nodiscard]] uint64_t get_crt_first_block() const;

    private:
        process::process_environment_block const& peb_;
        uint64_t const crt_first_block_;
    };
}
