#pragma once
#include <cstdint>

#include "page_range_flags_utils.h"
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
    class segment_heap;

    class page_range_descriptor
    {
    public:
        page_range_descriptor(segment_heap const& heap, uint64_t page_range_descriptor_address, size_t index, uint64_t heap_page_segment_address);

        [[nodiscard]] segment_heap const& heap() const { return heap_; }
        [[nodiscard]] stream_stack_dump::mini_dump_stack_walk const& walker() const;
        [[nodiscard]] process::process_environment_block const& peb() const;

        [[nodiscard]] uint64_t page_range_descriptor_address() const { return page_range_descriptor_address_; }
        [[nodiscard]] size_t index() const { return index_; }

        [[nodiscard]] bool extra_present() const;
        [[nodiscard]] size_units::base_10::bytes extra_bytes() const;
        [[nodiscard]] page_range_flags_utils::page_range_flags range_flags() const;
        [[nodiscard]] uint8_t committed_page_count() const;
        [[nodiscard]] uint8_t unit_offset() const;
        [[nodiscard]] size_units::base_10::bytes unit_size() const;
        [[nodiscard]] uint64_t block_address() const;
        [[nodiscard]] size_units::base_10::bytes block_size() const;
        [[nodiscard]] uint64_t user_address() const;
        [[nodiscard]] size_units::base_10::bytes user_size() const;

        [[nodiscard]] bool is_start_of_range() const;

        [[nodiscard]] uint64_t symbol_address() const { return page_range_descriptor_address(); }
        [[nodiscard]] dbg_help::symbol_type_info const& symbol_type() const { return heap_page_range_descriptor_symbol_type_; }

        static std::wstring const& symbol_name;

    private:
        [[nodiscard]] int64_t read_front_padding_size() const;
        [[nodiscard]] int64_t get_unit_block_size() const;

    private:
        segment_heap const& heap_;
        uint64_t const page_range_descriptor_address_;
        size_t const index_;
        uint64_t const heap_page_segment_address_;
        dbg_help::symbol_type_info const heap_page_range_descriptor_symbol_type_;
    };
}

