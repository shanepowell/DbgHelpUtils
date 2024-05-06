#pragma once

#include "HandleOperationStreamEntry.g.h"

#include "GlobalOptionsNotifyPropertyChangedBase.h"

// ReSharper disable once CppUnusedIncludeDirective
#include "DbgHelpUtils/windows_setup.h"
#include <avrfsdk.h>

namespace winrt::MiniDumpExplorer::implementation
{
    struct AvrfBackTraceInformation;

    struct HandleOperationStreamEntry : HandleOperationStreamEntryT<HandleOperationStreamEntry>, GlobalOptionsNotifyPropertyChangedBase<HandleOperationStreamEntry>
    {
        HandleOperationStreamEntry();

        uint32_t Index()  const { return index_; }
        uint64_t Handle()  const { return operation_->Handle; }
        uint32_t ProcessId()  const { return operation_->ProcessId; }
        uint32_t ThreadId()  const { return operation_->ThreadId; }
        uint32_t OperationType()  const { return operation_->OperationType; }
        hstring OperationTypeString()  const { return operationTypeString_; }
        MiniDumpExplorer::AvrfBackTraceInformation BackTraceInformation() const { return backTraceInformation_; }

        void Set(uint32_t index, AVRF_HANDLE_OPERATION const* operation);

    private:
        uint32_t index_{};
        AVRF_HANDLE_OPERATION const* operation_{ nullptr };
        hstring operationTypeString_;
        MiniDumpExplorer::AvrfBackTraceInformation backTraceInformation_{};
                                            };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct HandleOperationStreamEntry : HandleOperationStreamEntryT<HandleOperationStreamEntry, implementation::HandleOperationStreamEntry>
    {
    };
}
