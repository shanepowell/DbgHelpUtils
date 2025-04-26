#pragma once

#include "ThreadStack.g.h"

#include "GlobalOptionsNotifyPropertyChangedBase.h"
#include "DbgHelpUtils/stream_thread_ex.h"
#include "Utility/mini_dump_walker_store.h"

class SymbolEngineHelper;

namespace dlg_help_utils::dbg_help
{
    class symbol_engine;
}

namespace winrt::MiniDumpExplorer::implementation
{
    struct MiniDumpMemoryDescriptor;
    struct ThreadStackEntry;

    struct ThreadStack : ThreadStackT<ThreadStack>, GlobalOptionsNotifyPropertyChangedBase<ThreadStack>
    {
        ThreadStack();

        void Set(dlg_help_utils::stream_thread thread);
        void Set(dlg_help_utils::stream_thread_ex thread);
        void LoadStack(dlg_help_utils::mini_dump const& mini_dump, SymbolEngineHelper& symbolEngineHelper);

        bool HasStackMemoryRange() const { return stackMemoryRange_ != nullptr; }
        MiniDumpExplorer::MiniDumpMemoryDescriptor StackMemoryRange() const { return stackMemoryRange_; }
        Windows::Foundation::Collections::IObservableVector<MiniDumpExplorer::ThreadStackEntry> StackEntries() const { return stackEntries_; }

    private:
        fire_and_forget LoadThreadStack(dlg_help_utils::mini_dump const& mini_dump, SymbolEngineHelper& symbolEngineHelper);
        fire_and_forget LoadThreadExStack(dlg_help_utils::mini_dump const& mini_dump, SymbolEngineHelper& symbolEngineHelper);
        static bool IsX86Process(std::shared_ptr<Utility::mini_dump_walker_store> const& walker_store, dlg_help_utils::mini_dump const& mini_dump);

    private:
        dlg_help_utils::stream_thread thread_{};
        dlg_help_utils::stream_thread_ex threadEx_{};
        MiniDumpExplorer::MiniDumpMemoryDescriptor stackMemoryRange_;
        Windows::Foundation::Collections::IObservableVector<MiniDumpExplorer::ThreadStackEntry> stackEntries_{single_threaded_observable_vector<MiniDumpExplorer::ThreadStackEntry>()};
        bool stackLoading_{false};
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct ThreadStack : ThreadStackT<ThreadStack, implementation::ThreadStack>
    {
    };
}
