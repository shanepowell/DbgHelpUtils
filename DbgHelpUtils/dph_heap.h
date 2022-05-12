#pragma once
#include "size_units.h"
#include "stream_utils.h"
#include "symbol_type_info.h"
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
    class dph_entry;

    class dph_heap
    {
    public:
        dph_heap(cache_manager& cache, process::process_environment_block const& peb, uint64_t dph_heap_address);

        [[nodiscard]] cache_manager& cache() const { return *cache_manager_; }

        [[nodiscard]] stream_stack_dump::mini_dump_memory_walker const& walker() const;
        [[nodiscard]] process::process_environment_block const& peb() const { return *peb_; }
        
        [[nodiscard]] uint64_t address() const { return dph_heap_address_; }

        [[nodiscard]] uint64_t normal_heap() const;
        [[nodiscard]] uint32_t flags() const;
        [[nodiscard]] uint32_t extra_flags() const;
        [[nodiscard]] uint32_t seed() const;

        [[nodiscard]] uint32_t busy_allocations() const;
        [[nodiscard]] size_units::base_16::bytes busy_allocations_committed() const;

        [[nodiscard]] uint32_t virtual_storage_ranges() const;
        [[nodiscard]] size_units::base_16::bytes virtual_storage_total() const;

        [[nodiscard]] uint32_t free_allocations() const;
        [[nodiscard]] size_units::base_16::bytes free_allocations_committed() const;

        [[nodiscard]] static std::experimental::generator<dph_heap> dph_heaps(cache_manager& cache, process::process_environment_block const& peb);

        [[nodiscard]] std::experimental::generator<dph_entry> busy_entries() const;
        [[nodiscard]] std::experimental::generator<dph_entry> free_entries() const;
        [[nodiscard]] std::experimental::generator<dph_entry> virtual_ranges() const;

        [[nodiscard]] ust_address_stack_trace const& stack_trace() const { return stack_trace_; }

        [[nodiscard]] uint64_t symbol_address() const { return address(); }
        [[nodiscard]] dbg_help::symbol_type_info const& symbol_type() const { return cache_data_->dph_heap_root_symbol_type; }

        static std::wstring const& symbol_name;

    private:
        struct cache_data
        {
            dbg_help::symbol_type_info dph_heap_root_symbol_type;
            stream_utils::symbol_type_and_base_type_field_offset dph_heap_root_flags_field_data;
            stream_utils::symbol_type_and_base_type_field_offset dph_heap_root_extra_flags_field_data;
            stream_utils::symbol_type_and_base_type_field_offset dph_heap_root_seed_field_data;
            stream_utils::symbol_type_and_base_type_field_offset dph_heap_root_busy_allocations_field_data;
            stream_utils::symbol_type_and_base_type_field_offset dph_heap_root_busy_allocations_bytes_committed_field_data;
            stream_utils::symbol_type_and_base_type_field_offset dph_heap_root_virtual_storage_ranges_field_data;
            stream_utils::symbol_type_and_base_type_field_offset dph_heap_root_virtual_storage_bytes_field_data;
            stream_utils::symbol_type_and_base_type_field_offset dph_heap_root_free_allocations_bytes_committed_field_data;
            stream_utils::symbol_type_and_base_type_field_offset dph_heap_root_normal_heap_field_data;
            stream_utils::symbol_type_and_base_type_field_offset dph_heap_root_free_allocations_list_head_field_data;
            stream_utils::symbol_type_and_base_type_field_offset dph_heap_root_free_allocations_list_tail_field_data;
            stream_utils::symbol_type_and_base_type_field_offset dph_heap_root_virtual_storage_list_head_field_data;
            stream_utils::symbol_type_and_base_type_field_offset dph_heap_root_virtual_storage_list_tail_field_data;

            dbg_help::symbol_type_and_field_offset dph_heap_root_busy_nodes_table_field_data;
        };

        [[nodiscard]] std::experimental::generator<dph_entry> walk_list(uint64_t head, uint64_t tail) const;
        [[nodiscard]] cache_data const& setup_globals() const;

    private:
        cache_manager* cache_manager_;
        process::process_environment_block const* peb_;
        uint64_t dph_heap_address_;
        cache_data const* cache_data_{&setup_globals()};
        ust_address_stack_trace stack_trace_{cache(), walker()};
    };
}
