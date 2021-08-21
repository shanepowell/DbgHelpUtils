#pragma once
#include "function_table_stream.h"
#include "gflags_utils.h"
#include "memory64_list_stream.h"
#include "memory_list_stream.h"
#include "mini_dump_stack_walk.h"
#include "module_list_stream.h"
#include "pe_file_memory_mapping.h"
#include "process_parameters.h"
#include "symbol_type_info.h"
#include "thread_names_list_stream.h"
#include "unloaded_module_list_stream.h"

namespace dlg_help_utils
{
    namespace heap
    {
        class nt_heap;
    }

    class mini_dump;
}

namespace dlg_help_utils::process
{
    class process_environment_block
    {
    public:
        process_environment_block(mini_dump const& mini_dump, dbg_help::symbol_engine& symbol_engine);

        [[nodiscard]] mini_dump const& mini_dump() const { return mini_dump_; }
        [[nodiscard]] stream_stack_dump::mini_dump_stack_walk const& walker() const { return walker_; }

        [[nodiscard]] uint64_t peb_address() const { return peb_address_; }
        [[nodiscard]] gflags_utils::gflags nt_global_flag() const { return nt_global_flag_; }
        [[nodiscard]] uint64_t process_heap() const { return process_heap_; }
        [[nodiscard]] uint32_t number_of_heaps() const { return number_of_heaps_; }
        [[nodiscard]] uint64_t ldr_address() const { return ldr_address_; }
        [[nodiscard]] std::streamsize machine_hex_printable_length() const { return machine_hex_printable_length_; }

        [[nodiscard]] std::optional<process_parameters> process_parameters() const;

        [[nodiscard]] uint64_t heap_address(uint32_t heap_index) const;
        [[nodiscard]] uint32_t segment_signature(uint32_t heap_index) const;
        [[nodiscard]] std::optional<heap::nt_heap> nt_heap(uint32_t heap_index) const;

        [[nodiscard]] bool is_x86_target() const;
        [[nodiscard]] bool is_x64_target() const;

        [[nodiscard]] bool user_stack_db_enabled() const;
        [[nodiscard]] bool heap_page_alloc_enabled() const;

    private:
        [[nodiscard]] static uint64_t get_teb_address(dlg_help_utils::mini_dump const& mini_dump, stream_stack_dump::mini_dump_stack_walk const& walker, thread_names_list_stream const& names_list);
        [[nodiscard]] static uint64_t get_peb_address(stream_stack_dump::mini_dump_stack_walk const& walker, dbg_help::symbol_type_info const& teb_symbol_info, uint64_t teb_address);
        [[nodiscard]] static gflags_utils::gflags get_nt_global_flag(stream_stack_dump::mini_dump_stack_walk const& walker, dbg_help::symbol_type_info const& peb_symbol_info, uint64_t peb_address);
        [[nodiscard]] static uint64_t get_main_process_heap(stream_stack_dump::mini_dump_stack_walk const& walker, dbg_help::symbol_type_info const& peb_symbol_info, uint64_t peb_address);
        [[nodiscard]] static uint32_t get_number_of_heaps(stream_stack_dump::mini_dump_stack_walk const& walker, dbg_help::symbol_type_info const& peb_symbol_info, uint64_t peb_address);
        [[nodiscard]] static uint64_t get_ldr_address(stream_stack_dump::mini_dump_stack_walk const& walker, dbg_help::symbol_type_info const& peb_symbol_info, uint64_t peb_address);

    private:
        dlg_help_utils::mini_dump const& mini_dump_;
        thread_names_list_stream names_list_;
        memory_list_stream memory_list_;
        memory64_list_stream memory64_list_;
        function_table_stream function_table_;
        module_list_stream module_list_;
        unloaded_module_list_stream unloaded_module_list_;
        pe_file_memory_mapping pe_file_memory_mappings_{};
        stream_stack_dump::mini_dump_stack_walk const walker_;
        dbg_help::symbol_type_info const heap_symbol_type_;
        dbg_help::symbol_type_info const peb_symbol_info_;
        dbg_help::symbol_type_info const teb_symbol_info_;
        uint64_t const teb_address_;
        uint64_t const peb_address_;
        gflags_utils::gflags const nt_global_flag_;
        uint64_t const process_heap_;
        uint32_t const number_of_heaps_;
        uint64_t const ldr_address_;
        dbg_help::symbol_type_info process_heaps_pointer_type_;
        uint64_t process_heaps_address_{};
        std::streamsize machine_pointer_size_{};
        std::streamsize machine_hex_printable_length_{};
    };
}
