#include "pch.h"
#include "HandleOperationStreamEntry.h"

#include "AvrfBackTraceInformation.h"
#include "DbgHelpUtils/system_info_utils.h"

#if __has_include("HandleOperationStreamEntry.g.cpp")
// ReSharper disable once CppUnusedIncludeDirective
#include "HandleOperationStreamEntry.g.cpp"  // NOLINT(bugprone-suspicious-include)
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    HandleOperationStreamEntry::HandleOperationStreamEntry()
        : GlobalOptionsNotifyPropertyChangedBase(
            {
                L"Handle",
                L"ProcessId"
                L"ThreadId"
                L"OperationType"
            }, 
            {
            },
            {
            },
            {
            })
    {
    }

    void HandleOperationStreamEntry::Set(uint32_t const index, AVRF_HANDLE_OPERATION const* operation)
    {
        index_ = index;
        operation_ = operation;
        operationTypeString_ = dlg_help_utils::system_info_utils::handle_trace_operation_to_string(operation->OperationType);
        backTraceInformation_.as<AvrfBackTraceInformation>()->Set(&operation->BackTraceInformation);
    }

}
