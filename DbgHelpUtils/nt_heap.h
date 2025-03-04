#pragma once
#include <cstdint>
#include "generator.h"

#include "size_units.h"
#include "stream_utils.h"
#include "symbol_type_info.h"
#include "ust_address_stack_trace.h"

namespace dlg_help_utils
{
    class cache_manager;
}

namespace dlg_help_utils::stream_stack_dump
{
    class mini_dump_memory_walker;
}

namespace dlg_help_utils::heap
{
    class dph_heap;
    class lfh_heap;
    class heap_ucr_descriptor;
    class heap_segment;
    class heap_virtual_block;
    class heap_entry;

    constexpr uint32_t HeapEncodeFlagMakeEncodingEnabled = 0x100000;
    constexpr uint8_t FrontEndHeapTypeLowFragmentationHeap = 0x02;

    class nt_heap
    {
    public:

        nt_heap(cache_manager& cache, process::process_environment_block const& peb, uint64_t nt_heap_address);

        [[nodiscard]] cache_manager& cache() const { return *cache_manager_; }

        [[nodiscard]] process::process_environment_block const& peb() const { return *peb_; }
        [[nodiscard]] stream_stack_dump::mini_dump_memory_walker const& walker() const;

        [[nodiscard]] uint64_t nt_heap_address() const { return nt_heap_address_; }
        [[nodiscard]] uint32_t segment_signature() const;
        [[nodiscard]] uint32_t flags() const;
        [[nodiscard]] size_units::base_16::bytes reserved() const;
        [[nodiscard]] size_units::base_16::bytes committed() const;
        [[nodiscard]] size_units::base_16::bytes uncommitted() const;
        [[nodiscard]] size_units::base_16::bytes virtual_blocks() const;
        [[nodiscard]] uint32_t total_segments() const;
        [[nodiscard]] uint32_t total_ucrs() const;
        [[nodiscard]] size_units::base_16::bytes total_free_size() const;
        [[nodiscard]] uint16_t allocator_back_trace_index() const;
        [[nodiscard]] uint8_t front_end_heap_type() const;
        [[nodiscard]] bool is_low_fragment_heap_enabled() const { return front_end_heap_type() == FrontEndHeapTypeLowFragmentationHeap; }

        [[nodiscard]] bool is_encoded() const { return encoding_ != nullptr; }
        [[nodiscard]] uint32_t granularity() const { return cache_data_->granularity; }

        [[nodiscard]] generator<heap_segment> segments() const;
        [[nodiscard]] generator<heap_ucr_descriptor> uncommitted_ranges() const;
        [[nodiscard]] generator<heap_virtual_block> heap_virtual_blocks() const;
        [[nodiscard]] generator<heap_entry> free_entries() const;
        [[nodiscard]] std::optional<dph_heap> debug_page_heap() const;
        [[nodiscard]] bool debug_page_heap_available() const;

        [[nodiscard]] std::optional<lfh_heap> lfh_heap() const;

        [[nodiscard]] bool is_process_heap(uint64_t process_heap_address) const;

        void decode_heap_entry(void* buffer) const;

        [[nodiscard]] ust_address_stack_trace const& stack_trace() const { return stack_trace_; }

        [[nodiscard]] uint64_t symbol_address() const { return nt_heap_address(); }
        [[nodiscard]] dbg_help::symbol_type_info const& symbol_type() const { return cache_data_->heap_symbol_type; }

        static std::wstring const& symbol_name;

    private:
        struct cache_data
        {
            dbg_help::symbol_type_info heap_symbol_type;
            uint32_t granularity{};

            stream_utils::symbol_type_and_base_type_field_offset heap_encode_flag_mask_field_data;
            stream_utils::symbol_type_and_base_type_field_offset heap_segment_signature_field_data;
            stream_utils::symbol_type_and_base_type_field_offset heap_flags_field_data;
            stream_utils::symbol_type_and_base_type_field_offset heap_total_memory_reserved_field_data;
            stream_utils::symbol_type_and_base_type_field_offset heap_total_memory_committed_field_data;
            stream_utils::symbol_type_and_base_type_field_offset heap_total_memory_large_ucr_field_data;
            stream_utils::symbol_type_and_base_type_field_offset heap_total_size_in_virtual_blocks_field_data;
            stream_utils::symbol_type_and_base_type_field_offset heap_total_segments_field_data;
            stream_utils::symbol_type_and_base_type_field_offset heap_total_ucrs_field_data;
            stream_utils::symbol_type_and_base_type_field_offset heap_total_free_size_field_data;
            stream_utils::symbol_type_and_base_type_field_offset heap_allocator_back_trace_index_field_data;
            stream_utils::symbol_type_and_base_type_field_offset heap_front_end_heap_type_field_data;
            stream_utils::symbol_type_and_base_type_field_offset heap_front_end_heap_field_data;
            stream_utils::symbol_type_and_base_type_field_offset heap_encoding_field_data;

            dbg_help::symbol_type_and_field_offset heap_segment_list_field_data;
            dbg_help::symbol_type_and_field_offset heap_ucr_list_field_data;
            dbg_help::symbol_type_and_field_offset heap_virtual_allocated_blocks_field_data;
            dbg_help::symbol_type_and_field_offset heap_free_lists_field_data;
        };
        [[nodiscard]] cache_data const& setup_globals() const;

    private:
        cache_manager* cache_manager_;
        uint64_t nt_heap_address_;
        process::process_environment_block const* peb_;
        cache_data const* cache_data_{&setup_globals()};
        std::unique_ptr<uint8_t[]> encoding_;
        ust_address_stack_trace stack_trace_{cache(), walker()};
    };
}
