#include "pch.h"
#include "ThreadNameListStreamEntry.h"

#if __has_include("ThreadNameListStreamEntry.g.cpp")
// ReSharper disable once CppUnusedIncludeDirective
#include "ThreadNameListStreamEntry.g.cpp"  // NOLINT(bugprone-suspicious-include)
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    ThreadNameListStreamEntry::ThreadNameListStreamEntry()
        : GlobalOptionsNotifyPropertyChangedBase(
            {
                L"Id",
            },
            {
            }, 
            {
            },
            {
            },
            {
            },
            {
            })
    {
    }

    void ThreadNameListStreamEntry::Set(uint32_t const index, dlg_help_utils::stream_thread_name thread)
    {
        index_ = index;
        thread_ = std::move(thread);
        name_ = thread_.name();
    }
}
