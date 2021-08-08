#include "gflags_utils.h"

#include <map>

#include "flags_string_utils.h"

using namespace std::string_view_literals;

namespace
{

    using dlg_help_utils::gflags_utils::gflags;

   std::map<uint32_t, std::wstring_view> const dump_gflags =
    {
        {static_cast<uint32_t>(gflags::FLG_DISABLE_DBGPRINT           ), L"FLG_DISABLE_DBGPRINT"sv},
        {static_cast<uint32_t>(gflags::FLG_KERNEL_STACK_TRACE_DB      ), L"FLG_KERNEL_STACK_TRACE_DB"sv},
        {static_cast<uint32_t>(gflags::FLG_USER_STACK_TRACE_DB        ), L"FLG_USER_STACK_TRACE_DB"sv},
        {static_cast<uint32_t>(gflags::FLG_DEBUG_INITIAL_COMMAND      ), L"FLG_DEBUG_INITIAL_COMMAND"sv},
        {static_cast<uint32_t>(gflags::FLG_DEBUG_INITIAL_COMMAND_EX   ), L"FLG_DEBUG_INITIAL_COMMAND_EX"sv},
        {static_cast<uint32_t>(gflags::FLG_HEAP_DISABLE_COALESCING    ), L"FLG_HEAP_DISABLE_COALESCING"sv},
        {static_cast<uint32_t>(gflags::FLG_DISABLE_PAGE_KERNEL_STACKS ), L"FLG_DISABLE_PAGE_KERNEL_STACKS"sv},
        {static_cast<uint32_t>(gflags::FLG_DISABLE_PROTDLLS           ), L"FLG_DISABLE_PROTDLLS"sv},
        {static_cast<uint32_t>(gflags::FLG_DISABLE_STACK_EXTENSION    ), L"FLG_DISABLE_STACK_EXTENSION"sv},
        {static_cast<uint32_t>(gflags::FLG_CRITSEC_EVENT_CREATION     ), L"FLG_CRITSEC_EVENT_CREATION"sv},
        {static_cast<uint32_t>(gflags::FLG_APPLICATION_VERIFIER       ), L"FLG_APPLICATION_VERIFIER"sv},
        {static_cast<uint32_t>(gflags::FLG_ENABLE_HANDLE_EXCEPTIONS   ), L"FLG_ENABLE_HANDLE_EXCEPTIONS"sv},
        {static_cast<uint32_t>(gflags::FLG_ENABLE_CLOSE_EXCEPTIONS    ), L"FLG_ENABLE_CLOSE_EXCEPTIONS"sv},
        {static_cast<uint32_t>(gflags::FLG_ENABLE_CSRDEBUG            ), L"FLG_ENABLE_CSRDEBUG"sv},
        {static_cast<uint32_t>(gflags::FLG_ENABLE_EXCEPTION_LOGGING   ), L"FLG_ENABLE_EXCEPTION_LOGGING"sv},
        {static_cast<uint32_t>(gflags::FLG_HEAP_ENABLE_FREE_CHECK     ), L"FLG_HEAP_ENABLE_FREE_CHECK"sv},
        {static_cast<uint32_t>(gflags::FLG_HEAP_VALIDATE_PARAMETERS   ), L"FLG_HEAP_VALIDATE_PARAMETERS"sv},
        {static_cast<uint32_t>(gflags::FLG_HEAP_ENABLE_TAGGING        ), L"FLG_HEAP_ENABLE_TAGGING"sv},
        {static_cast<uint32_t>(gflags::FLG_HEAP_ENABLE_TAG_BY_DLL     ), L"FLG_HEAP_ENABLE_TAG_BY_DLL"sv},
        {static_cast<uint32_t>(gflags::FLG_HEAP_ENABLE_TAIL_CHECK     ), L"FLG_HEAP_ENABLE_TAIL_CHECK"sv},
        {static_cast<uint32_t>(gflags::FLG_HEAP_VALIDATE_ALL          ), L"FLG_HEAP_VALIDATE_ALL"sv},
        {static_cast<uint32_t>(gflags::FLG_ENABLE_KDEBUG_SYMBOL_LOAD  ), L"FLG_ENABLE_KDEBUG_SYMBOL_LOAD"sv},
        {static_cast<uint32_t>(gflags::FLG_ENABLE_HANDLE_TYPE_TAGGING ), L"FLG_ENABLE_HANDLE_TYPE_TAGGING"sv},
        {static_cast<uint32_t>(gflags::FLG_HEAP_PAGE_ALLOCS           ), L"FLG_HEAP_PAGE_ALLOCS"sv},
        {static_cast<uint32_t>(gflags::FLG_POOL_ENABLE_TAGGING        ), L"FLG_POOL_ENABLE_TAGGING"sv},
        {static_cast<uint32_t>(gflags::FLG_ENABLE_SYSTEM_CRIT_BREAKS  ), L"FLG_ENABLE_SYSTEM_CRIT_BREAKS"sv},
        {static_cast<uint32_t>(gflags::FLG_MAINTAIN_OBJECT_TYPELIST   ), L"FLG_MAINTAIN_OBJECT_TYPELIST"sv},
        {static_cast<uint32_t>(gflags::FLG_MONITOR_SILENT_PROCESS_EXIT), L"FLG_MONITOR_SILENT_PROCESS_EXIT"sv},
        {static_cast<uint32_t>(gflags::FLG_SHOW_LDR_SNAPS             ), L"FLG_SHOW_LDR_SNAPS"sv},
        {static_cast<uint32_t>(gflags::FLG_STOP_ON_EXCEPTION          ), L"FLG_STOP_ON_EXCEPTION"sv},
        {static_cast<uint32_t>(gflags::FLG_STOP_ON_HUNG_GUI           ), L"FLG_STOP_ON_HUNG_GUI"sv},
        {static_cast<uint32_t>(gflags::FLG_STOP_ON_UNHANDLED_EXCEPTION), L"FLG_STOP_ON_UNHANDLED_EXCEPTION"sv},
    };
}

namespace dlg_help_utils::gflags_utils
{
    std::vector<std::wstring_view> dump_gflags_to_strings(gflags const nt_global_flags)
    {
        return flags_string_utils::generate_flags_strings(static_cast<uint32_t>(nt_global_flags), dump_gflags);
    }
}
