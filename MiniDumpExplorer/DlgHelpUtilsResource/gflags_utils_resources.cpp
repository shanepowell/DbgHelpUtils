#include "pch.h"
#include "DbgHelpUtils/gflags_utils.h"

#include <winrt/Microsoft.Windows.ApplicationModel.Resources.h>

#include "DbgHelpUtils/flags_string_utils.h"

using namespace std::string_literals;

namespace
{

    std::unordered_map<dlg_help_utils::gflags_utils::gflags, std::wstring> const& dump_gflags()
    {
        using dlg_help_utils::gflags_utils::gflags;
        winrt::Microsoft::Windows::ApplicationModel::Resources::ResourceManager const rm{};
        static std::unordered_map<gflags, std::wstring> const dump_gflags =
        {
        {gflags::FLG_DISABLE_DBGPRINT           , std::wstring{rm.MainResourceMap().GetValue(L"Resources/FLG_DISABLE_DBGPRINT").ValueAsString()}},
        {gflags::FLG_KERNEL_STACK_TRACE_DB      , std::wstring{rm.MainResourceMap().GetValue(L"Resources/FLG_KERNEL_STACK_TRACE_DB").ValueAsString()}},
        {gflags::FLG_USER_STACK_TRACE_DB        , std::wstring{rm.MainResourceMap().GetValue(L"Resources/FLG_USER_STACK_TRACE_DB").ValueAsString()}},
        {gflags::FLG_DEBUG_INITIAL_COMMAND      , std::wstring{rm.MainResourceMap().GetValue(L"Resources/FLG_DEBUG_INITIAL_COMMAND").ValueAsString()}},
        {gflags::FLG_DEBUG_INITIAL_COMMAND_EX   , std::wstring{rm.MainResourceMap().GetValue(L"Resources/FLG_DEBUG_INITIAL_COMMAND_EX").ValueAsString()}},
        {gflags::FLG_HEAP_DISABLE_COALESCING    , std::wstring{rm.MainResourceMap().GetValue(L"Resources/FLG_HEAP_DISABLE_COALESCING").ValueAsString()}},
        {gflags::FLG_DISABLE_PAGE_KERNEL_STACKS , std::wstring{rm.MainResourceMap().GetValue(L"Resources/FLG_DISABLE_PAGE_KERNEL_STACKS").ValueAsString()}},
        {gflags::FLG_DISABLE_PROTDLLS           , std::wstring{rm.MainResourceMap().GetValue(L"Resources/FLG_DISABLE_PROTDLLS").ValueAsString()}},
        {gflags::FLG_DISABLE_STACK_EXTENSION    , std::wstring{rm.MainResourceMap().GetValue(L"Resources/FLG_DISABLE_STACK_EXTENSION").ValueAsString()}},
        {gflags::FLG_CRITSEC_EVENT_CREATION     , std::wstring{rm.MainResourceMap().GetValue(L"Resources/FLG_CRITSEC_EVENT_CREATION").ValueAsString()}},
        {gflags::FLG_APPLICATION_VERIFIER       , std::wstring{rm.MainResourceMap().GetValue(L"Resources/FLG_APPLICATION_VERIFIER").ValueAsString()}},
        {gflags::FLG_ENABLE_HANDLE_EXCEPTIONS   , std::wstring{rm.MainResourceMap().GetValue(L"Resources/FLG_ENABLE_HANDLE_EXCEPTIONS").ValueAsString()}},
        {gflags::FLG_ENABLE_CLOSE_EXCEPTIONS    , std::wstring{rm.MainResourceMap().GetValue(L"Resources/FLG_ENABLE_CLOSE_EXCEPTIONS").ValueAsString()}},
        {gflags::FLG_ENABLE_CSRDEBUG            , std::wstring{rm.MainResourceMap().GetValue(L"Resources/FLG_ENABLE_CSRDEBUG").ValueAsString()}},
        {gflags::FLG_ENABLE_EXCEPTION_LOGGING   , std::wstring{rm.MainResourceMap().GetValue(L"Resources/FLG_ENABLE_EXCEPTION_LOGGING").ValueAsString()}},
        {gflags::FLG_HEAP_ENABLE_FREE_CHECK     , std::wstring{rm.MainResourceMap().GetValue(L"Resources/FLG_HEAP_ENABLE_FREE_CHECK").ValueAsString()}},
        {gflags::FLG_HEAP_VALIDATE_PARAMETERS   , std::wstring{rm.MainResourceMap().GetValue(L"Resources/FLG_HEAP_VALIDATE_PARAMETERS").ValueAsString()}},
        {gflags::FLG_HEAP_ENABLE_TAGGING        , std::wstring{rm.MainResourceMap().GetValue(L"Resources/FLG_HEAP_ENABLE_TAGGING").ValueAsString()}},
        {gflags::FLG_HEAP_ENABLE_TAG_BY_DLL     , std::wstring{rm.MainResourceMap().GetValue(L"Resources/FLG_HEAP_ENABLE_TAG_BY_DLL").ValueAsString()}},
        {gflags::FLG_HEAP_ENABLE_TAIL_CHECK     , std::wstring{rm.MainResourceMap().GetValue(L"Resources/FLG_HEAP_ENABLE_TAIL_CHECK").ValueAsString()}},
        {gflags::FLG_HEAP_VALIDATE_ALL          , std::wstring{rm.MainResourceMap().GetValue(L"Resources/FLG_HEAP_VALIDATE_ALL").ValueAsString()}},
        {gflags::FLG_ENABLE_KDEBUG_SYMBOL_LOAD  , std::wstring{rm.MainResourceMap().GetValue(L"Resources/FLG_ENABLE_KDEBUG_SYMBOL_LOAD").ValueAsString()}},
        {gflags::FLG_ENABLE_HANDLE_TYPE_TAGGING , std::wstring{rm.MainResourceMap().GetValue(L"Resources/FLG_ENABLE_HANDLE_TYPE_TAGGING").ValueAsString()}},
        {gflags::FLG_HEAP_PAGE_ALLOCS           , std::wstring{rm.MainResourceMap().GetValue(L"Resources/FLG_HEAP_PAGE_ALLOCS").ValueAsString()}},
        {gflags::FLG_POOL_ENABLE_TAGGING        , std::wstring{rm.MainResourceMap().GetValue(L"Resources/FLG_POOL_ENABLE_TAGGING").ValueAsString()}},
        {gflags::FLG_ENABLE_SYSTEM_CRIT_BREAKS  , std::wstring{rm.MainResourceMap().GetValue(L"Resources/FLG_ENABLE_SYSTEM_CRIT_BREAKS").ValueAsString()}},
        {gflags::FLG_MAINTAIN_OBJECT_TYPELIST   , std::wstring{rm.MainResourceMap().GetValue(L"Resources/FLG_MAINTAIN_OBJECT_TYPELIST").ValueAsString()}},
        {gflags::FLG_MONITOR_SILENT_PROCESS_EXIT, std::wstring{rm.MainResourceMap().GetValue(L"Resources/FLG_MONITOR_SILENT_PROCESS_EXIT").ValueAsString()}},
        {gflags::FLG_SHOW_LDR_SNAPS             , std::wstring{rm.MainResourceMap().GetValue(L"Resources/FLG_SHOW_LDR_SNAPS").ValueAsString()}},
        {gflags::FLG_STOP_ON_EXCEPTION          , std::wstring{rm.MainResourceMap().GetValue(L"Resources/FLG_STOP_ON_EXCEPTION").ValueAsString()}},
        {gflags::FLG_STOP_ON_HUNG_GUI           , std::wstring{rm.MainResourceMap().GetValue(L"Resources/FLG_STOP_ON_HUNG_GUI").ValueAsString()}},
        {gflags::FLG_STOP_ON_UNHANDLED_EXCEPTION, std::wstring{rm.MainResourceMap().GetValue(L"Resources/FLG_STOP_ON_UNHANDLED_EXCEPTION").ValueAsString()}},
        };

        return dump_gflags;
    }
}

namespace dlg_help_utils::gflags_utils
{
    std::vector<std::wstring> dump_gflags_to_strings(gflags const nt_global_flags)
    {
        return flags_string_utils::generate_flags_strings(nt_global_flags, dump_gflags());
    }
}
