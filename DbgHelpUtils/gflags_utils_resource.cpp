#include "gflags_utils.h"

#include "flags_string_utils.h"

using namespace std::string_literals;

namespace
{
    using dlg_help_utils::gflags_utils::gflags;

    std::unordered_map<gflags, std::wstring> const dump_gflags =
    {
        {gflags::FLG_DISABLE_DBGPRINT           , L"FLG_DISABLE_DBGPRINT"s},
        {gflags::FLG_KERNEL_STACK_TRACE_DB      , L"FLG_KERNEL_STACK_TRACE_DB"s},
        {gflags::FLG_USER_STACK_TRACE_DB        , L"FLG_USER_STACK_TRACE_DB"s},
        {gflags::FLG_DEBUG_INITIAL_COMMAND      , L"FLG_DEBUG_INITIAL_COMMAND"s},
        {gflags::FLG_DEBUG_INITIAL_COMMAND_EX   , L"FLG_DEBUG_INITIAL_COMMAND_EX"s},
        {gflags::FLG_HEAP_DISABLE_COALESCING    , L"FLG_HEAP_DISABLE_COALESCING"s},
        {gflags::FLG_DISABLE_PAGE_KERNEL_STACKS , L"FLG_DISABLE_PAGE_KERNEL_STACKS"s},
        {gflags::FLG_DISABLE_PROTDLLS           , L"FLG_DISABLE_PROTDLLS"s},
        {gflags::FLG_DISABLE_STACK_EXTENSION    , L"FLG_DISABLE_STACK_EXTENSION"s},
        {gflags::FLG_CRITSEC_EVENT_CREATION     , L"FLG_CRITSEC_EVENT_CREATION"s},
        {gflags::FLG_APPLICATION_VERIFIER       , L"FLG_APPLICATION_VERIFIER"s},
        {gflags::FLG_ENABLE_HANDLE_EXCEPTIONS   , L"FLG_ENABLE_HANDLE_EXCEPTIONS"s},
        {gflags::FLG_ENABLE_CLOSE_EXCEPTIONS    , L"FLG_ENABLE_CLOSE_EXCEPTIONS"s},
        {gflags::FLG_ENABLE_CSRDEBUG            , L"FLG_ENABLE_CSRDEBUG"s},
        {gflags::FLG_ENABLE_EXCEPTION_LOGGING   , L"FLG_ENABLE_EXCEPTION_LOGGING"s},
        {gflags::FLG_HEAP_ENABLE_FREE_CHECK     , L"FLG_HEAP_ENABLE_FREE_CHECK"s},
        {gflags::FLG_HEAP_VALIDATE_PARAMETERS   , L"FLG_HEAP_VALIDATE_PARAMETERS"s},
        {gflags::FLG_HEAP_ENABLE_TAGGING        , L"FLG_HEAP_ENABLE_TAGGING"s},
        {gflags::FLG_HEAP_ENABLE_TAG_BY_DLL     , L"FLG_HEAP_ENABLE_TAG_BY_DLL"s},
        {gflags::FLG_HEAP_ENABLE_TAIL_CHECK     , L"FLG_HEAP_ENABLE_TAIL_CHECK"s},
        {gflags::FLG_HEAP_VALIDATE_ALL          , L"FLG_HEAP_VALIDATE_ALL"s},
        {gflags::FLG_ENABLE_KDEBUG_SYMBOL_LOAD  , L"FLG_ENABLE_KDEBUG_SYMBOL_LOAD"s},
        {gflags::FLG_ENABLE_HANDLE_TYPE_TAGGING , L"FLG_ENABLE_HANDLE_TYPE_TAGGING"s},
        {gflags::FLG_HEAP_PAGE_ALLOCS           , L"FLG_HEAP_PAGE_ALLOCS"s},
        {gflags::FLG_POOL_ENABLE_TAGGING        , L"FLG_POOL_ENABLE_TAGGING"s},
        {gflags::FLG_ENABLE_SYSTEM_CRIT_BREAKS  , L"FLG_ENABLE_SYSTEM_CRIT_BREAKS"s},
        {gflags::FLG_MAINTAIN_OBJECT_TYPELIST   , L"FLG_MAINTAIN_OBJECT_TYPELIST"s},
        {gflags::FLG_MONITOR_SILENT_PROCESS_EXIT, L"FLG_MONITOR_SILENT_PROCESS_EXIT"s},
        {gflags::FLG_SHOW_LDR_SNAPS             , L"FLG_SHOW_LDR_SNAPS"s},
        {gflags::FLG_STOP_ON_EXCEPTION          , L"FLG_STOP_ON_EXCEPTION"s},
        {gflags::FLG_STOP_ON_HUNG_GUI           , L"FLG_STOP_ON_HUNG_GUI"s},
        {gflags::FLG_STOP_ON_UNHANDLED_EXCEPTION, L"FLG_STOP_ON_UNHANDLED_EXCEPTION"s},
    };
}

namespace dlg_help_utils::gflags_utils
{
    std::vector<std::wstring_view> dump_gflags_to_strings(gflags const nt_global_flags)
    {
        return flags_string_utils::generate_flags_strings(nt_global_flags, dump_gflags);
    }
}
