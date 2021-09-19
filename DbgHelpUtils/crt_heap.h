#pragma once
#include <cstdint>
#include <vector>
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

        void set_base_diff_filter(crt_heap& base_diff_filter);
        void clear_base_diff_filter();

        [[nodiscard]] process::process_environment_block const& peb() const { return peb_; }
        [[nodiscard]] stream_stack_dump::mini_dump_stack_walk const& walker() const;

        [[nodiscard]] bool is_using_crt_heap() const { return crt_first_block_ != 0; }

        [[nodiscard]] std::experimental::generator<crt_entry> entries() const;

    private:
        [[nodiscard]] uint64_t get_crt_first_block() const;

        [[nodiscard]] std::experimental::generator<crt_entry> all_entries() const;
        [[nodiscard]] std::experimental::generator<crt_entry> filter_entries() const;

        [[nodiscard]] static bool is_filtered(std::vector<crt_entry> const& filters, crt_entry const& entry);

    private:
        process::process_environment_block const& peb_;
        uint64_t const crt_first_block_;
        crt_heap* base_diff_filter_{nullptr};
    };
}
