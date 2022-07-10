#pragma once
#include "function_table_stream.h"
#include "gflags_utils.h"
#include "memory64_list_stream.h"
#include "memory_list_stream.h"
#include "mini_dump_memory_walker.h"
#include "module_list_stream.h"
#include "pe_file_memory_mapping.h"
#include "process_environment_variables.h"
#include "stream_utils.h"
#include "symbol_type_info.h"
#include "system_memory_info_stream.h"
#include "thread_names_list_stream.h"
#include "unloaded_module_list_stream.h"

namespace dlg_help_utils
{
    class cache_manager;

    namespace heap
    {
        class segment_heap;
        class nt_heap;
    }

    class mini_dump;
}

namespace dlg_help_utils::process
{
    class process_environment_block
    {
    public:
        process_environment_block(mini_dump const& mini_dump, cache_manager& cache, dbg_help::symbol_engine& symbol_engine);

        [[nodiscard]] cache_manager& cache() const { return *cache_manager_; }
        [[nodiscard]] mini_dump const& mini_dump() const { return *mini_dump_; }
        [[nodiscard]] stream_stack_dump::mini_dump_memory_walker const& walker() const { return walker_; }

        [[nodiscard]] uint64_t peb_address() const { return peb_address_; }
        [[nodiscard]] gflags_utils::gflags nt_global_flag() const { return nt_global_flag_; }
        [[nodiscard]] uint64_t process_heap() const { return process_heap_; }
        [[nodiscard]] uint32_t number_of_heaps() const { return number_of_heaps_; }
        [[nodiscard]] uint64_t ldr_address() const { return ldr_address_; }
        [[nodiscard]] std::streamsize machine_hex_printable_length() const { return machine_hex_printable_length_; }

        [[nodiscard]] std::optional<process_environment_variables> process_environment_variables() const;

        [[nodiscard]] uint64_t heap_address(uint32_t heap_index) const;
        [[nodiscard]] uint32_t segment_signature(uint32_t heap_index) const;
        [[nodiscard]] std::optional<heap::nt_heap> nt_heap(uint32_t heap_index) const;
        [[nodiscard]] std::optional<heap::segment_heap> segment_heap(uint32_t heap_index) const;

        [[nodiscard]] bool is_x86_target() const;
        [[nodiscard]] bool is_x64_target() const;

        [[nodiscard]] bool user_stack_db_enabled() const;
        [[nodiscard]] bool heap_page_alloc_enabled() const;

        [[nodiscard]] std::streamsize machine_pointer_size() const { return machine_pointer_size_; }

        [[nodiscard]] uint64_t page_size() const { return system_memory_info_.system_memory_misc_info().BasicInfo.PageSize; }

        void gather_system_area_addresses(std::set<uint64_t>& system_area_addresses) const;

        [[nodiscard]] thread_names_list_stream const& names_list() const { return names_list_; }
        [[nodiscard]] memory_list_stream const& memory_list() const { return memory_list_; }
        [[nodiscard]] memory64_list_stream const& memory64_list() const { return memory64_list_; }
        [[nodiscard]] function_table_stream const& function_table() const { return function_table_; }
        [[nodiscard]] module_list_stream const& module_list() const { return module_list_; }
        [[nodiscard]] unloaded_module_list_stream const& unloaded_module_list() const { return unloaded_module_list_; }
        [[nodiscard]] system_memory_info_stream const& system_memory_info() const { return system_memory_info_; }

        [[nodiscard]] dbg_help::symbol_type_info const& peb_symbol() const;
        [[nodiscard]] dbg_help::symbol_type_info const& teb_symbol() const;

    private:
        [[nodiscard]] uint64_t get_teb_address() const;
        [[nodiscard]] uint64_t get_peb_address() const;
        [[nodiscard]] gflags_utils::gflags get_nt_global_flag() const;
        [[nodiscard]] uint64_t get_main_process_heap() const;
        [[nodiscard]] uint32_t get_number_of_heaps() const;
        [[nodiscard]] uint64_t get_ldr_address() const;

        struct cache_data
        {
            dbg_help::symbol_type_info heap_symbol_type;
            dbg_help::symbol_type_info peb_symbol_info;
            dbg_help::symbol_type_info teb_symbol_info;

            stream_utils::symbol_type_and_base_type_field_offset peb_structure_process_heaps_field_data;
            stream_utils::symbol_type_and_base_type_field_offset peb_structure_process_parameters_field_data;
            stream_utils::symbol_type_and_base_type_field_offset peb_structure_nt_global_flag_field_data;
            stream_utils::symbol_type_and_base_type_field_offset peb_structure_process_heap_field_data;
            stream_utils::symbol_type_and_base_type_field_offset peb_structure_number_of_heaps_field_data;
            stream_utils::symbol_type_and_base_type_field_offset peb_structure_ldr_field_data;

            stream_utils::symbol_type_and_base_type_field_offset teb_structure_process_environment_block_field_data;
        };
        [[nodiscard]] cache_data const& setup_globals() const;

    private:
        cache_manager* cache_manager_;
        dlg_help_utils::mini_dump const* mini_dump_;
        thread_names_list_stream names_list_;
        memory_list_stream memory_list_;
        memory64_list_stream memory64_list_;
        function_table_stream function_table_;
        module_list_stream module_list_;
        unloaded_module_list_stream unloaded_module_list_;
        system_memory_info_stream system_memory_info_;
        pe_file_memory_mapping pe_file_memory_mappings_{};
        stream_stack_dump::mini_dump_memory_walker walker_;
        cache_data const* cache_data_{&setup_globals()};
        uint64_t teb_address_{get_teb_address()};
        uint64_t peb_address_{get_peb_address()};
        gflags_utils::gflags nt_global_flag_{get_nt_global_flag()};
        uint64_t process_heap_{get_main_process_heap()};
        uint32_t number_of_heaps_{get_number_of_heaps()};
        uint64_t ldr_address_{get_ldr_address()};
        dbg_help::symbol_type_info process_heaps_pointer_type_{nullptr, 0, 0};
        uint64_t process_heaps_address_{};
        std::streamsize machine_pointer_size_{};
        std::streamsize machine_hex_printable_length_{};
    };
}
