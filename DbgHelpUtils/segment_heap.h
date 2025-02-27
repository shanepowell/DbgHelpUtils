#pragma once
#include <cstdint>

#include "process_environment_block.h"
#include "segment_heap_options.h"
#include "size_units.h"
#include "stream_utils.h"
#include "ust_address_stack_trace.h"

namespace dlg_help_utils
{
    class cache_manager;
}

namespace dlg_help_utils::process
{
    class process_environment_block;
}

namespace dlg_help_utils::stream_stack_dump
{
    class mini_dump_memory_walker;
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
        segment_heap(cache_manager& cache, process::process_environment_block const& peb, uint64_t segment_heap_address, segment_heap_options options);

        [[nodiscard]] cache_manager& cache() const { return *cache_manager_; }

        [[nodiscard]] process::process_environment_block const& peb() const { return *peb_; }
        [[nodiscard]] stream_stack_dump::mini_dump_memory_walker const& walker() const { return peb().walker(); }
        [[nodiscard]] segment_heap_options const& options() const { return options_; }

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

        [[nodiscard]] dlg_help_utils::generator<heap_segment_context> segment_contexts() const;
        [[nodiscard]] heap_vs_context vs_context() const;
        [[nodiscard]] heap_lfh_context lfh_context() const;

        [[nodiscard]] std::optional<dph_heap> debug_page_heap() const;

        [[nodiscard]] dlg_help_utils::generator<large_alloc_entry> large_entries() const;

        [[nodiscard]] uint64_t unit_shift_amount() const { return unit_shift_amount_; }

        void decode_vs_check_header(uint64_t header_address, void* header) const;

        [[nodiscard]] ust_address_stack_trace const& stack_trace() const { return stack_trace_; }

        [[nodiscard]] uint64_t symbol_address() const { return segment_heap_address(); }
        [[nodiscard]] dbg_help::symbol_type_info const& symbol_type() const { return cache_data_->segment_heap_symbol_type; }

        static std::wstring const& symbol_name;
            
    private:
        template<typename T>
        [[nodiscard]] T get_heap_key_value(std::pair<uint64_t, uint64_t> const& address_and_size, uint64_t data_offset) const;
        [[nodiscard]] uint64_t get_heap_key() const;
        [[nodiscard]] uint32_t get_lfh_heap_key() const;

        template<typename T>
        void decode_vs_check_header_type(uint64_t header_address, void* header) const;

        [[nodiscard]] uint64_t get_unit_shift_amount() const;

        struct cache_data
        {
            dbg_help::symbol_type_info segment_heap_symbol_type;
            dbg_help::symbol_type_info heap_seg_context_symbol_type;
            size_t heap_seg_context_symbol_length{};
            dbg_help::symbol_type_info heap_seg_context_array_field_symbol_type;
            uint64_t heap_seg_context_array_field_offset{};
            uint64_t heap_vs_context_offset{};
            uint64_t heap_lfh_context_offset{};

            stream_utils::symbol_type_and_base_type_field_offset segment_heap_signature_field_data;
            stream_utils::symbol_type_and_base_type_field_offset segment_heap_global_flags_field_data;
            stream_utils::symbol_type_and_base_type_field_offset segment_heap_mem_stats_total_reserved_pages_field_data;
            stream_utils::symbol_type_and_base_type_field_offset segment_heap_mem_stats_total_committed_pages_field_data;
            stream_utils::symbol_type_and_base_type_field_offset segment_heap_mem_stats_free_committed_pages_field_data;
            stream_utils::symbol_type_and_base_type_field_offset segment_heap_mem_stats_lfh_free_committed_pages_field_data;
            stream_utils::symbol_type_and_base_type_field_offset segment_heap_large_reserved_pages_field_data;
            stream_utils::symbol_type_and_base_type_field_offset segment_heap_large_committed_pages_field_data;

            dbg_help::symbol_type_and_field_offset segment_heap_large_alloc_metadata_field_data;
        };
        [[nodiscard]] static stream_utils::symbol_type_and_base_type_field_offset get_heap_seg_context_array_data(cache_data const& data);
        [[nodiscard]] cache_data const& setup_globals() const;

    private:
        cache_manager* cache_manager_;
        uint64_t segment_heap_address_;
        segment_heap_options options_;
        process::process_environment_block const* peb_;
        cache_data const* cache_data_{&setup_globals()};
        uint64_t heap_key_;
        uint32_t lfh_heap_key_;
        uint64_t unit_shift_amount_;
        ust_address_stack_trace stack_trace_{cache(), walker()};
    };
}
