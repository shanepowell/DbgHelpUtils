#pragma once
#include <cstdint>
#include <optional>

#include "i_stack_walk_callback.h"
#include "symbol_type_info.h"

#include <DbgHelp.h>
#include <experimental/generator>

#include "mini_dump_memory_stream.h"
#include "symbol_engine.h"

namespace dlg_help_utils
{
    class stream_unloaded_module;
    class stream_module;
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

        [[nodiscard]] mini_dump_memory_stream get_process_memory_stream(DWORD64 base_address, DWORD64 size, bool enable_module_loading = true) const;
        [[nodiscard]] DWORD64 find_memory_range(DWORD64 base_address, DWORD64 element_size, DWORD64 max_elements, bool enable_module_loading = true) const;
        [[nodiscard]] DWORD64 find_memory_range_if(DWORD64 base_address, DWORD64 element_size, DWORD64 max_elements, std::function<bool(void const*)> const& pred, bool enable_module_loading = true) const;

        [[nodiscard]] std::optional<dbg_help::symbol_address_info> find_symbol_info(STACKFRAME_EX const& frame) const override;
        static [[nodiscard]] std::optional<dbg_help::symbol_address_info> find_symbol_info(
            uint64_t address, module_list_stream const& module_list,
            unloaded_module_list_stream const& unloaded_module_list, dbg_help::symbol_engine& symbol_engine);

        [[nodiscard]] std::optional<dbg_help::symbol_type_info> get_type_info(std::wstring const& type_name) const;
        [[nodiscard]] std::optional<dbg_help::symbol_type_info> get_symbol_info(std::wstring const& symbol_name) const;
        [[nodiscard]] std::experimental::generator<dbg_help::symbol_type_info> module_types(std::wstring const& module_name) const;

        [[nodiscard]] dbg_help::symbol_engine& symbol_engine() const { return symbol_engine_; }
        [[nodiscard]] module_list_stream const& module_list() const { return module_list_; }
        [[nodiscard]] unloaded_module_list_stream const& unloaded_module_list() const { return unloaded_module_list_; }
        [[nodiscard]] memory_list_stream const& memory_list() const { return memory_list_; }
        [[nodiscard]] memory64_list_stream const& memory64_list() const { return memory64_list_; }

    private:
        [[nodiscard]] void const* get_process_memory_range(DWORD64 base_address, DWORD64& size, bool enable_module_loading) const;

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
        [[nodiscard]] void const* get_stack_memory(DWORD64 base_address, DWORD64& size, bool limit_size) const;
        [[nodiscard]] void const* get_memory_list_memory(DWORD64 base_address, DWORD64& size, bool limit_size) const;
        [[nodiscard]] void const* get_memory64_list_memory(DWORD64 base_address, DWORD64& size, bool limit_size) const;
        [[nodiscard]] void const* get_memory_from_pe_file(DWORD64 base_address, DWORD64& size, bool limit_size) const;
        [[nodiscard]] bool load_pe_file(DWORD64 base_address) const;
        [[nodiscard]] bool load_loaded_module(DWORD64 base_address) const;
        [[nodiscard]] bool load_unloaded_module(DWORD64 base_address) const;
        [[nodiscard]] PVOID get_mini_dump_function_table(DWORD64 base_address) const;
        [[nodiscard]] DWORD64 get_loaded_module_base_routine(DWORD64 address) const;
        [[nodiscard]] DWORD64 get_unloaded_module_base_routine(DWORD64 address) const;

        void load_module(std::wstring const& module_name) const;
        void load_module(stream_module const& module) const;
        void load_module(stream_unloaded_module const& module) const;

        static [[nodiscard]] std::optional<dbg_help::symbol_address_info> find_unloaded_module_symbol_info(
            uint64_t address, unloaded_module_list_stream const& unloaded_module_list,
            dbg_help::symbol_engine& symbol_engine);

        static DWORD64 find_max_element_size(void const* memory, DWORD64 element_size, DWORD64 max_size, std::function<bool(void const*)> const& pred);

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
        dbg_help::callback_handle handle_;
    };
}
