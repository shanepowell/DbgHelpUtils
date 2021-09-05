#pragma once
#include <cstdint>

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
    class heap_lfh_context;
    class heap_lfh_subsegment;

    class heap_lfh_affinity_slot
    {
    public:
        heap_lfh_affinity_slot(heap_lfh_context const& heap, uint64_t heap_lfh_affinity_slot_address);

        [[nodiscard]] heap_lfh_context const& heap() const { return heap_; }
        [[nodiscard]] stream_stack_dump::mini_dump_stack_walk const& walker() const;
        [[nodiscard]] process::process_environment_block const& peb() const;

        [[nodiscard]] uint64_t heap_lfh_affinity_slot_address() const { return heap_lfh_affinity_slot_address_; }

        [[nodiscard]] size_t bucket_index() const;
        [[nodiscard]] size_t slot_index() const;
        [[nodiscard]] uint64_t available_subsegment_count() const;

        [[nodiscard]] std::experimental::generator<heap_lfh_subsegment> subsegments() const;

        [[nodiscard]] uint64_t symbol_address() const { return heap_lfh_affinity_slot_address(); }
        [[nodiscard]] dbg_help::symbol_type_info const& symbol_type() const { return heap_lfh_affinity_slot_symbol_type_; }

        static std::wstring const& symbol_name;

    private:
        heap_lfh_context const& heap_;
        uint64_t const heap_lfh_affinity_slot_address_;
        dbg_help::symbol_type_info const heap_lfh_affinity_slot_symbol_type_;
    };
}

