#pragma once
#include <cstdint>

#include "process_environment_block.h"
#include "size_units.h"
#include "ust_address_stack_trace.h"

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
    class dph_heap;
    class heap_lfh_context;
    class heap_segment_context;
    class heap_vs_context;
    class large_alloc_entry;

    class segment_heap
    {
    public:
        segment_heap(process::process_environment_block const& peb, uint64_t segment_heap_address);

        [[nodiscard]] process::process_environment_block const& peb() const { return peb_; }
        [[nodiscard]] stream_stack_dump::mini_dump_stack_walk const& walker() const { return peb_.walker(); }

        [[nodiscard]] uint64_t segment_heap_address() const { return segment_heap_address_; }
        [[nodiscard]] uint32_t segment_signature() const;
        [[nodiscard]] uint32_t global_flags() const;
        [[nodiscard]] uint64_t total_reserved_pages() const;
        [[nodiscard]] uint64_t total_committed_pages() const;
        [[nodiscard]] uint64_t free_committed_pages() const;
        [[nodiscard]] uint64_t lfh_free_committed_pages() const;
        [[nodiscard]] uint64_t large_reserved_pages() const;
        [[nodiscard]] uint64_t large_committed_pages() const;
        [[nodiscard]] size_units::base_16::bytes reserved() const;
        [[nodiscard]] size_units::base_16::bytes committed() const;
        [[nodiscard]] size_units::base_16::bytes uncommitted() const;
        [[nodiscard]] size_units::base_16::bytes large_reserved() const;
        [[nodiscard]] size_units::base_16::bytes large_committed() const;
        [[nodiscard]] size_units::base_16::bytes large_uncommitted() const;

        [[nodiscard]] uint64_t heap_key() const { return heap_key_; }
        [[nodiscard]] uint32_t lfh_heap_key() const { return lfh_heap_key_; }

        [[nodiscard]] std::experimental::generator<heap_segment_context> segment_contexts() const;
        [[nodiscard]] heap_vs_context vs_context() const;
        [[nodiscard]] heap_lfh_context lfh_context() const;

        [[nodiscard]] std::optional<dph_heap> debug_page_heap() const;

        [[nodiscard]] std::experimental::generator<large_alloc_entry> large_entries() const;

        [[nodiscard]] uint64_t unit_shift_amount() const { return unit_shift_amount_; }

        void decode_vs_check_header(uint64_t header_address, void* header) const;

        [[nodiscard]] ust_address_stack_trace const& stack_trace() const { return stack_trace_; }

        [[nodiscard]] uint64_t symbol_address() const { return segment_heap_address(); }
        [[nodiscard]] dbg_help::symbol_type_info const& symbol_type() const { return segment_heap_symbol_type_; }

        static std::wstring const& symbol_name;
            
    private:
        [[nodiscard]] std::pair<dbg_help::symbol_type_info, uint64_t> get_heap_seg_context_array_data() const;
        [[nodiscard]] dbg_help::symbol_type_info get_heap_seg_context_array_field_symbol_type() const;
        [[nodiscard]] uint64_t get_heap_seg_context_array_field_offset() const;

        template<typename T>
        [[nodiscard]] T get_heap_key_value(std::pair<uint64_t, uint64_t> const& address_and_size, uint64_t data_offset) const;
        [[nodiscard]] uint64_t get_heap_key() const;
        [[nodiscard]] uint32_t get_lfh_heap_key() const;

        template<typename T>
        void decode_vs_check_header_type(uint64_t header_address, void* header) const;

        [[nodiscard]] uint64_t get_unit_shift_amount() const;

    private:
        uint64_t const segment_heap_address_;
        process::process_environment_block const& peb_;
        dbg_help::symbol_type_info const segment_heap_symbol_type_;
        dbg_help::symbol_type_info const heap_seg_context_symbol_type_;
        size_t const heap_seg_context_symbol_length_;
        dbg_help::symbol_type_info const heap_seg_context_array_field_symbol_type_;
        uint64_t const heap_seg_context_array_field_offset_;
        uint64_t heap_key_;
        uint32_t lfh_heap_key_;
        uint64_t const heap_vs_context_offset_;
        uint64_t const heap_lfh_context_offset_;
        uint64_t const unit_shift_amount_;
        ust_address_stack_trace stack_trace_{walker()};
    };
}
