#pragma once
#include <cstdint>

#include "size_units.h"
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
    class heap_entry;
    class lfh_heap;

    class heap_subsegment
    {
    public:
        heap_subsegment(lfh_heap const& heap, uint64_t heap_subsegment_address, uint64_t lfh_block_zone_size);

        [[nodiscard]] lfh_heap const& lfh_heap() const { return lfh_heap_; }
        [[nodiscard]] stream_stack_dump::mini_dump_stack_walk const& walker() const;
        [[nodiscard]] process::process_environment_block const& peb() const;

        [[nodiscard]] uint64_t address() const { return heap_subsegment_address_; }
        [[nodiscard]] size_units::base_16::bytes block_size() const;
        [[nodiscard]] uint16_t block_size_raw() const;
        [[nodiscard]] uint16_t block_count() const;

        [[nodiscard]] uint64_t entry_start_address() const { return entry_start_address_; }
        [[nodiscard]] uint16_t block_stride() const { return block_stride_; }

        [[nodiscard]] std::experimental::generator<heap_entry> entries() const;

        [[nodiscard]] uint64_t symbol_address() const { return address(); }
        [[nodiscard]] dbg_help::symbol_type_info const& symbol_type() const { return heap_subsegment_symbol_type_; }

        static std::wstring const& symbol_name;
        static std::wstring const& user_data_header_symbol_name;

    private:
        [[nodiscard]] std::tuple<uint64_t, uint16_t> get_entry_data() const;

    private:
        heap::lfh_heap const& lfh_heap_;
        uint64_t const heap_subsegment_address_;
        uint64_t const lfh_block_zone_size_;
        dbg_help::symbol_type_info const heap_subsegment_symbol_type_;
        dbg_help::symbol_type_info const heap_user_data_header_symbol_type_;
        uint64_t entry_start_address_;
        uint16_t block_stride_;
    };
}
