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
    class heap_vs_entry;
    class segment_heap;

    class heap_vs_subsegment
    {
    public:
         heap_vs_subsegment(segment_heap const& heap, uint64_t heap_vs_subsegment_address);

        [[nodiscard]] segment_heap const& heap() const { return heap_; }
        [[nodiscard]] stream_stack_dump::mini_dump_stack_walk const& walker() const;
        [[nodiscard]] process::process_environment_block const& peb() const;

        [[nodiscard]] uint64_t heap_vs_subsegment_address() const { return heap_vs_subsegment_address_; }

        [[nodiscard]] size_units::base_16::bytes size() const;
        [[nodiscard]] uint16_t signature() const;
        [[nodiscard]] bool is_signature_valid() const;
        [[nodiscard]] bool full_commit() const;

        [[nodiscard]] std::experimental::generator<heap_vs_entry> entries() const;

        [[nodiscard]] uint64_t symbol_address() const { return heap_vs_subsegment_address(); }
        [[nodiscard]] dbg_help::symbol_type_info const& symbol_type() const { return heap_vs_subsegment_symbol_type_; }

        static std::wstring const& symbol_name;

    private:
        [[nodiscard]] uint16_t size_raw() const;
        [[nodiscard]] uint64_t get_subsegment_offset() const;

    private:
        segment_heap const& heap_;
        uint64_t const heap_vs_subsegment_address_;
        dbg_help::symbol_type_info const heap_vs_subsegment_symbol_type_;
        size_t const heap_vs_subsegment_length_;
        size_t const heap_vs_chunk_header_length_;
   };
}