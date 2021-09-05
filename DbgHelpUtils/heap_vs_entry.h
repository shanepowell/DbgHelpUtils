#pragma once

#include <cstdint>
#include <string>

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

    class heap_vs_entry
    {
    public:
        heap_vs_entry(segment_heap const& heap, uint64_t heap_vs_entry_address, std::unique_ptr<uint8_t[]> buffer, uint16_t previous_size);
        heap_vs_entry(segment_heap const& heap, uint64_t heap_vs_entry_address, std::unique_ptr<uint8_t[]> buffer);
        heap_vs_entry(segment_heap const& heap, uint64_t heap_vs_entry_address, uint64_t uncommitted_size);

        [[nodiscard]] segment_heap const& heap() const { return heap_; }
        [[nodiscard]] stream_stack_dump::mini_dump_stack_walk const& walker() const;
        [[nodiscard]] process::process_environment_block const& peb() const;

        [[nodiscard]] uint64_t heap_vs_entry_address() const { return heap_vs_entry_address_; }

        [[nodiscard]] bool is_valid() const { return is_valid_; }

        [[nodiscard]] size_units::base_10::bytes size() const { return size_; }
        [[nodiscard]] size_units::base_10::bytes previous_size() const { return previous_size_; }
        [[nodiscard]] bool uncommitted_range() const { return uncommitted_range_; }
        [[nodiscard]] uint16_t memory_cost() const;
        [[nodiscard]] bool allocated() const;
        [[nodiscard]] uint8_t segment_page_offset() const;
        [[nodiscard]] bool has_unused_bytes() const;
        [[nodiscard]] size_units::base_10::bytes unused_bytes() const;
        [[nodiscard]] bool skip_during_walk() const;
        [[nodiscard]] uint32_t spare() const;
        [[nodiscard]] uint32_t allocated_chunk_bits() const;

        [[nodiscard]] uint64_t user_address() const;
        [[nodiscard]] size_units::base_10::bytes user_size() const;

        [[nodiscard]] uint16_t raw_size() const;

        [[nodiscard]] uint64_t symbol_address() const { return heap_vs_entry_address(); }
        [[nodiscard]] dbg_help::symbol_type_info const& symbol_type() const { return heap_vs_chunk_header_symbol_type_; }

        static std::wstring const& symbol_name;

    private:
        [[nodiscard]] size_units::base_10::bytes get_size() const;
        [[nodiscard]] size_units::base_10::bytes get_previous_size() const;
        [[nodiscard]] uint16_t get_previous_size_raw() const;
        [[nodiscard]] bool get_is_valid(uint16_t previous_size) const;
        [[nodiscard]] uint64_t read_front_padding_size() const;

    private:
        segment_heap const& heap_;
        uint64_t const heap_vs_entry_address_;
        std::unique_ptr<uint8_t[]> buffer_;
        dbg_help::symbol_type_info const heap_vs_chunk_header_symbol_type_;
        uint64_t const heap_vs_chunk_header_length_;
        size_units::base_10::bytes const size_;
        size_units::base_10::bytes const previous_size_{0};
        bool const is_valid_{true};
        bool const uncommitted_range_{false};
   };
}
