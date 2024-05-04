#pragma once

#include "FunctionStreamEntry.g.h"

#include "GlobalOptionsNotifyPropertyChangedBase.h"
#include "DbgHelpUtils/stream_function_descriptor.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct FunctionStreamEntry : FunctionStreamEntryT<FunctionStreamEntry>, GlobalOptionsNotifyPropertyChangedBase<FunctionStreamEntry>
    {
        FunctionStreamEntry();

        uint32_t Index() const { return index_; }
        uint64_t MinimumAddress() const { return stream_function_->MinimumAddress; }
        uint64_t MaximumAddress() const { return stream_function_->MaximumAddress; }
        uint64_t BaseAddress() const { return stream_function_->BaseAddress; }
        uint32_t EntryCount() const { return stream_function_->EntryCount; }

        void Set(uint32_t index, dlg_help_utils::stream_function_descriptor const& stream_function);

    private:
        uint32_t index_{};
        dlg_help_utils::stream_function_descriptor stream_function_{};
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct FunctionStreamEntry : FunctionStreamEntryT<FunctionStreamEntry, implementation::FunctionStreamEntry>
    {
    };
}
