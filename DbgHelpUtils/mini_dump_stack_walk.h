#pragma once
#include <cstdint>
#include <optional>

#include "i_stack_walk_callback.h"
#include <DbgHelp.h>

namespace dlg_help_utils
{
    namespace dbg_help
    {
        class i_symbol_load_callback;
        class symbol_engine;
    }

    class pe_file_memory_mapping;
    class unloaded_module_list_stream;
    class module_list_stream;
    class function_table_stream;
    class memory64_list_stream;
    class memory_list_stream;
}

namespace dlg_help_utils::stream_stack_dump
{
    class mini_dump_stack_walk : public dbg_help::i_stack_walk_callback
    {
    public:
        mini_dump_stack_walk(DWORD64 stack_start_range, void const* stack, size_t stack_size,
                             memory_list_stream const& memory_list, memory64_list_stream const& memory64_list,
                             function_table_stream const& function_table, module_list_stream const& module_list,
                             unloaded_module_list_stream const& unloaded_module_list,
                             pe_file_memory_mapping& pe_file_memory_mappings, dbg_help::symbol_engine& symbol_engine);

        [[nodiscard]] bool read_process_memory(DWORD64 base_address, PVOID buffer, DWORD size,
                                               LPDWORD number_of_bytes_read, bool enable_module_loading) override;
        [[nodiscard]] PVOID function_table_access(DWORD64 base_address) override;
        [[nodiscard]] DWORD64 get_module_base_routine(DWORD64 address) override;
        [[nodiscard]] DWORD64 translate_address(HANDLE h_thread, LPADDRESS64 lp_address) override;

        [[nodiscard]] std::optional<dbg_help::symbol_address_info>
        find_symbol_info(STACKFRAME_EX const& frame) const override;
        static [[nodiscard]] std::optional<dbg_help::symbol_address_info> find_symbol_info(
            uint64_t address, module_list_stream const& module_list,
            unloaded_module_list_stream const& unloaded_module_list, dbg_help::symbol_engine& symbol_engine);

    private:
        [[nodiscard]] bool do_read_process_memory(DWORD64 base_address, PVOID buffer, DWORD size,
                                                  LPDWORD number_of_bytes_read, bool enable_module_loading) const;
        [[nodiscard]] bool read_stack_memory(DWORD64 base_address, PVOID buffer, DWORD size,
                                             LPDWORD number_of_bytes_read) const;
        [[nodiscard]] bool read_memory_list_memory(DWORD64 base_address, PVOID buffer, DWORD size,
                                                   LPDWORD number_of_bytes_read) const;
        [[nodiscard]] bool read_memory64_list_memory(DWORD64 base_address, PVOID buffer, DWORD size,
                                                     LPDWORD number_of_bytes_read) const;
        [[nodiscard]] bool read_memory_from_pe_file(DWORD64 base_address, PVOID buffer, DWORD size,
                                                    LPDWORD number_of_bytes_read) const;
        [[nodiscard]] bool load_pe_file(DWORD64 base_address) const;
        [[nodiscard]] bool load_loaded_module(DWORD64 base_address) const;
        [[nodiscard]] bool load_unloaded_module(DWORD64 base_address) const;
        [[nodiscard]] PVOID get_mini_dump_function_table(DWORD64 base_address) const;
        [[nodiscard]] DWORD64 get_loaded_module_base_routine(DWORD64 address) const;
        [[nodiscard]] DWORD64 get_unloaded_module_base_routine(DWORD64 address) const;

        static [[nodiscard]] std::optional<dbg_help::symbol_address_info> find_unloaded_module_symbol_info(
            uint64_t address, unloaded_module_list_stream const& unloaded_module_list,
            dbg_help::symbol_engine& symbol_engine);

    private:
        DWORD64 stack_start_range_;
        uint8_t const* stack_;
        size_t stack_size_;
        memory_list_stream const& memory_list_;
        memory64_list_stream const& memory64_list_;
        function_table_stream const& function_table_;
        module_list_stream const& module_list_;
        unloaded_module_list_stream const& unloaded_module_list_;
        pe_file_memory_mapping& pe_file_memory_mappings_;
        dbg_help::symbol_engine& symbol_engine_;
        dbg_help::i_symbol_load_callback& callback_;
    };
}
