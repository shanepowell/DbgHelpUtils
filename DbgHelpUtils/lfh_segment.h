#pragma once
#include <cstdint>

#include "symbol_type_info.h"

namespace dlg_help_utils::process
{
    class process_environment_block;
}

namespace dlg_help_utils::stream_stack_dump
{
    class mini_dump_stack_walk;
}

namespace dlg_help_utils::heap
{
    class lfh_heap;
    class heap_subsegment;
    class nt_heap;

    class lfh_segment
    {
    public:
        lfh_segment(lfh_heap const& heap, uint64_t lfh_segment_address);

        [[nodiscard]] lfh_heap const& lfh_heap() const { return lfh_heap_; }
        [[nodiscard]] stream_stack_dump::mini_dump_stack_walk const& walker() const;
        [[nodiscard]] process::process_environment_block const& peb() const;

        [[nodiscard]] uint64_t address() const { return lfh_segment_address_; }
        [[nodiscard]] size_t subsegments_count() const;
        [[nodiscard]] std::experimental::generator<heap_subsegment> subsegments() const;

        [[nodiscard]] uint64_t symbol_address() const { return address(); }
        [[nodiscard]] dbg_help::symbol_type_info const& symbol_type() const { return lfh_block_zone_symbol_type_; }

        static std::wstring const& symbol_name;

    private:
        [[nodiscard]] std::pair<uint64_t, uint64_t> get_subsegment_range() const;

        [[nodiscard]] uint64_t get_lfh_block_zone_size() const;

    private:
        heap::lfh_heap const& lfh_heap_;
        uint64_t const lfh_segment_address_;
        dbg_help::symbol_type_info const lfh_block_zone_symbol_type_;
        uint64_t const lfh_block_zone_size_{get_lfh_block_zone_size()};
        uint64_t const heap_subsegment_size_;
    };
}
