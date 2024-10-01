#include "pch.h"

#include "DbgHelpUtils/thread_names_list_stream.h"
#include "Models/ThreadNamesStreamEntriesDataSource.h"

#include "ThreadNamesStreamPage.xaml.h"
#if __has_include("ThreadNamesStreamPage.g.cpp")
// ReSharper disable once CppUnusedIncludeDirective
#include "ThreadNamesStreamPage.g.cpp" // NOLINT(bugprone-suspicious-include)
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    // ReSharper disable once CppDefaultedSpecialMemberFunctionIsImplicitlyDeleted
    ThreadNamesStreamPage::ThreadNamesStreamPage() = default;

    void ThreadNamesStreamPage::MiniDumpLoaded(MiniDumpExplorer::MiniDumpPageParameters const& parameters)
    {
        auto const miniDumpPage = parameters.MiniDump().as<MiniDumpPage>();
        auto const& miniDump = miniDumpPage->MiniDump();
        index_ = parameters.StreamIndex();

        dlg_help_utils::thread_names_list_stream const thread_list{miniDump, parameters.StreamIndex()};

        if(!thread_list.found())
        {
            logger::Log().LogMessage(log_level::error, std::format("failed to load mini dump thread names list stream index:[{}]", parameters.StreamIndex()));
            return;
        }

        total_entries_ = thread_list.thread_names_list().NumberOfThreadNames;
        threadsStreamEntriesDataSource_.as<ThreadNamesStreamEntriesDataSource>()->LoadMiniDumpThreadStream(thread_list);
    }
}
