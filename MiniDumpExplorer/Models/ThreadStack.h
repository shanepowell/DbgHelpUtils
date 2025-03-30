#pragma once

#include "ThreadStack.g.h"

#include "GlobalOptionsNotifyPropertyChangedBase.h"
#include "Utility/mini_dump_walker_store.h"

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
        fire_and_forget LoadStack(dlg_help_utils::mini_dump const& mini_dump);

        MiniDumpExplorer::MiniDumpMemoryDescriptor Stack() const { return stack_; }
        Windows::Foundation::Collections::IObservableVector<MiniDumpExplorer::ThreadStackEntry> StackEntries() const { return stackEntries_; }

    private:
        dlg_help_utils::stream_thread thread_{};
        MiniDumpExplorer::MiniDumpMemoryDescriptor stack_;
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
