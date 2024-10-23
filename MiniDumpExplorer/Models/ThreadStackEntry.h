#pragma once

#include "ThreadStackEntry.g.h"

#include <functional>

#include "GlobalOptionsNotifyPropertyChangedBase.h"
#include "DbgHelpUtils/stream_stack_dump.h"

namespace dlg_help_utils
{
    class mini_dump_memory_stream;
}

namespace dlg_help_utils::symbol_type_utils
{
    struct dump_variable_symbol_data;
}

namespace Utility
{
    struct mini_dump_walker_store;
}

namespace winrt::MiniDumpExplorer::implementation
{
    struct ThreadStackEntry : ThreadStackEntryT<ThreadStackEntry>, GlobalOptionsNotifyPropertyChangedBase<ThreadStackEntry>
    {
        ThreadStackEntry();

        enum class variable_type : uint8_t
        {
            parameters,
            local_variables
        };

        void Set(std::shared_ptr<Utility::mini_dump_walker_store> const& walker_store, dlg_help_utils::stream_stack_dump::stack_function_call_entry entry);
        void Set(std::shared_ptr<Utility::mini_dump_walker_store> const& walker_store, dlg_help_utils::stream_stack_dump::stack_function_call_entry entry, std::vector<dlg_help_utils::dbg_help::variable> const& variables, variable_type variable_type);
        void Set(std::shared_ptr<Utility::mini_dump_walker_store> const& walker_store, size_t index, dlg_help_utils::stream_stack_dump::stack_function_call_entry entry, dlg_help_utils::dbg_help::variable variable, bool parameter);
        void Set(std::shared_ptr<Utility::mini_dump_walker_store> const& walker_store, dlg_help_utils::symbol_type_utils::dump_variable_symbol_data data);

        uint64_t Index() const { return index_; }
        hstring Line() const { return line_; }
        bool IsInLine() const { return isInLine_; }
        bool IsStackFrame() const { return isStackFrame_; }
        bool IsParameter() const { return isParameter_; }
        bool IsLocalVariable() const { return isLocalVariable_; }
        Windows::Foundation::Collections::IObservableVector<MiniDumpExplorer::ThreadStackEntry> Children() const { return children_; }
        bool AreChildrenLoaded() const { return areChildrenLoaded_; }
        bool LoadingChildren() const { return loadingChildren_; }

        void LoadChildren();
        void SetDummy();

    private:
        fire_and_forget LoadSubLinesChildren(std::shared_ptr<Utility::mini_dump_walker_store> walker_store, dlg_help_utils::symbol_type_utils::dump_variable_symbol_data data);
        static param::hstring GetVariableTypeTitle(variable_type type);

    private:
        dlg_help_utils::stream_stack_dump::stack_function_call_entry entry_{};
        dlg_help_utils::dbg_help::variable variable_{};
        size_t index_{};
        bool isInLine_{};
        bool isStackFrame_{};
        bool isParameter_{};
        bool isLocalVariable_{};
        hstring line_{};
        std::atomic_bool areChildrenLoaded_{};
        bool loadingChildren_{};
        std::function<void ()> loadChildren_{};
        Windows::Foundation::Collections::IObservableVector<MiniDumpExplorer::ThreadStackEntry> children_{single_threaded_observable_vector<MiniDumpExplorer::ThreadStackEntry>()};
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct ThreadStackEntry : ThreadStackEntryT<ThreadStackEntry, implementation::ThreadStackEntry>
    {
    };
}
