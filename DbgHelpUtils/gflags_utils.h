#pragma once
#include <cstdint>
#include <string>
#include <vector>

namespace dlg_help_utils::gflags_utils
{
    // see https://docs.microsoft.com/en-us/windows-hardware/drivers/debugger/gflags-flag-table
    enum class gflags : uint32_t
    {
        // ReSharper disable IdentifierTypo
        FLG_DISABLE_DBGPRINT            = 0x08000000,
        FLG_KERNEL_STACK_TRACE_DB       = 0x2000,
        FLG_USER_STACK_TRACE_DB         = 0x1000,
        FLG_DEBUG_INITIAL_COMMAND       = 0x04,
        FLG_DEBUG_INITIAL_COMMAND_EX    = 0x04000000,
        FLG_HEAP_DISABLE_COALESCING     = 0x00200000,
        FLG_DISABLE_PAGE_KERNEL_STACKS  = 0x080000,
        FLG_DISABLE_PROTDLLS            = 0x80000000,
        FLG_DISABLE_STACK_EXTENSION     = 0x010000,
        FLG_CRITSEC_EVENT_CREATION      = 0x10000000,
        FLG_APPLICATION_VERIFIER        = 0x0100,
        FLG_ENABLE_HANDLE_EXCEPTIONS    = 0x40000000,
        FLG_ENABLE_CLOSE_EXCEPTIONS     = 0x400000,
        FLG_ENABLE_CSRDEBUG             = 0x020000,
        FLG_ENABLE_EXCEPTION_LOGGING    = 0x800000,
        FLG_HEAP_ENABLE_FREE_CHECK      = 0x20,
        FLG_HEAP_VALIDATE_PARAMETERS    = 0x40,
        FLG_HEAP_ENABLE_TAGGING         = 0x0800,
        FLG_HEAP_ENABLE_TAG_BY_DLL      = 0x8000,
        FLG_HEAP_ENABLE_TAIL_CHECK      = 0x10,
        FLG_HEAP_VALIDATE_ALL           = 0x80,
        FLG_ENABLE_KDEBUG_SYMBOL_LOAD   = 0x040000,
        FLG_ENABLE_HANDLE_TYPE_TAGGING  = 0x01000000,
        FLG_HEAP_PAGE_ALLOCS            = 0x02000000,
        FLG_POOL_ENABLE_TAGGING         = 0x0400,
        FLG_ENABLE_SYSTEM_CRIT_BREAKS   = 0x100000,
        FLG_MAINTAIN_OBJECT_TYPELIST    = 0x4000,
        FLG_MONITOR_SILENT_PROCESS_EXIT = 0x200,
        FLG_SHOW_LDR_SNAPS              = 0x02,
        FLG_STOP_ON_EXCEPTION           = 0x01,
        FLG_STOP_ON_HUNG_GUI            = 0x08,
        FLG_STOP_ON_UNHANDLED_EXCEPTION = 0x20000000,
        // ReSharper restore IdentifierTypo
    };

    std::vector<std::wstring> dump_gflags_to_strings(gflags nt_global_flags);
}
