#pragma once
#include "enable_module_loading.h"
#include "symbol_address_info.h"
#include <DbgHelp.h>
#include <optional>

namespace dlg_help_utils::dbg_help
{
    enum class thread_context_type : uint8_t
    {
        x86,
        wow64,
        x64
    };

    class i_stack_walk_callback
    {
    public:
        i_stack_walk_callback() = default;
        virtual ~i_stack_walk_callback() = default;

        i_stack_walk_callback(i_stack_walk_callback const&) = delete;
        i_stack_walk_callback(i_stack_walk_callback&&) = default;

        i_stack_walk_callback& operator=(i_stack_walk_callback const&) = delete;
        i_stack_walk_callback& operator=(i_stack_walk_callback&&) = default;

        [[nodiscard]] virtual bool read_process_memory(DWORD64 base_address
            , PVOID buffer
            , DWORD size
            , LPDWORD number_of_bytes_read
            , enable_module_loading_t enable_module_loading = enable_module_loading_t{true}) = 0;
        [[nodiscard]] virtual PVOID function_table_access(DWORD64 base_address) = 0;
        [[nodiscard]] virtual DWORD64 get_module_base_routine(DWORD64 address) = 0;
        [[nodiscard]] virtual DWORD64 translate_address(HANDLE h_thread, LPADDRESS64 lp_address) = 0;

        [[nodiscard]] virtual std::optional<symbol_address_info> find_symbol_info(thread_context_type type, STACKFRAME_EX const& frame, void const* thread_context) const = 0;
    };
}
