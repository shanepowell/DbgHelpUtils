#pragma once
#include "process_environment_block.h"
#include "size_units.h"
#include "symbol_type_info.h"
#include "ust_address_stack_trace.h"

namespace dlg_help_utils::stream_stack_dump
{
    class mini_dump_stack_walk;
}

namespace dlg_help_utils::heap
{
    class dph_entry;

    class dph_heap
    {
    public:
        dph_heap(process::process_environment_block const& peb, uint64_t dph_heap_address);

        [[nodiscard]] process::process_environment_block const& peb() const { return peb_; }
        [[nodiscard]] stream_stack_dump::mini_dump_stack_walk const& walker() const { return peb_.walker(); }
        
        [[nodiscard]] uint64_t address() const { return dph_heap_address_; }

        [[nodiscard]] uint64_t normal_heap() const;
        [[nodiscard]] uint32_t flags() const;
        [[nodiscard]] uint32_t extra_flags() const;
        [[nodiscard]] uint32_t seed() const;

        [[nodiscard]] uint32_t busy_allocations() const;
        [[nodiscard]] size_units::base_10::bytes busy_allocations_committed() const;

        [[nodiscard]] uint32_t virtual_storage_ranges() const;
        [[nodiscard]] size_units::base_10::bytes virtual_storage_total() const;

        [[nodiscard]] uint32_t free_allocations() const;
        [[nodiscard]] size_units::base_10::bytes free_allocations_committed() const;

        [[nodiscard]] static std::experimental::generator<dph_heap> dph_heaps(process::process_environment_block const& peb);

        [[nodiscard]] std::experimental::generator<dph_entry> busy_entries() const;
        [[nodiscard]] std::experimental::generator<dph_entry> free_entries() const;
        [[nodiscard]] std::experimental::generator<dph_entry> virtual_ranges() const;

        [[nodiscard]] ust_address_stack_trace const& stack_trace() const { return stack_trace_; }

    private:
        [[nodiscard]] std::experimental::generator<uint64_t> walk_balanced_table() const;
        [[nodiscard]] std::experimental::generator<dph_entry> walk_list(uint64_t head, uint64_t tail) const;

        template<typename T>
        [[nodiscard]] T get_field_value(std::wstring const& field_name) const;
        [[nodiscard]] uint64_t get_machine_size_field_value(std::wstring const& field_name) const;

        [[noreturn]] static void throw_cant_get_field_data(std::wstring const& field_name);
    
    private:
        process::process_environment_block const& peb_;
        uint64_t const dph_heap_address_;
        dbg_help::symbol_type_info const dph_heap_root_symbol_type_;
        dbg_help::symbol_type_info const rtl_balanced_links_symbol_type_;
        uint64_t const busy_nodes_table_offset_;
        ust_address_stack_trace stack_trace_;
    };
}
