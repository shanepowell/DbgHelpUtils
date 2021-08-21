#pragma once
#include <cstdint>

#include "symbol_type_info.h"

namespace dlg_help_utils::heap
{
    class lfh_heap;
    class heap_subsegment;
    class nt_heap;

    class lfh_segment
    {
    public:
        lfh_segment(lfh_heap const& heap, uint64_t lfh_segment_address);

        [[nodiscard]] uint64_t address() const { return lfh_segment_address_; }
        [[nodiscard]] size_t subsegments_count() const;
        [[nodiscard]] std::experimental::generator<heap_subsegment> subsegments() const;

    private:
        [[nodiscard]] std::pair<uint64_t, uint64_t> get_subsegment_range() const;

        [[nodiscard]] uint64_t get_lfh_block_zone_size() const;

    private:
        lfh_heap const& lfh_heap_;
        uint64_t const lfh_segment_address_;
        dbg_help::symbol_type_info const lfh_block_zone_symbol_type_;
        uint64_t const lfh_block_zone_size_;
        uint64_t const heap_subsegment_size_;
    };
}
