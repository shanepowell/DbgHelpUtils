#include "pch.h"
#include "ThreadEntryPage.xaml.h"

#include "DbgHelpUtils/thread_list_stream.h"
#include "Models/ThreadListStreamEntry.h"

#if __has_include("ThreadEntryPage.g.cpp")
// ReSharper disable once CppUnusedIncludeDirective
#include "ThreadEntryPage.g.cpp" // NOLINT(bugprone-suspicious-include)
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    ThreadEntryPage::ThreadEntryPage() = default;

    void ThreadEntryPage::MiniDumpLoaded(MiniDumpExplorer::MiniDumpPageParameters const& parameters)
    {
        auto const miniDumpPage = parameters.MiniDump().as<MiniDumpPage>();
        auto const& miniDump = miniDumpPage->MiniDump();

        dlg_help_utils::thread_list_stream const thread_list{miniDump, parameters.StreamIndex()};

        if(!thread_list.found())
        {
            logger::Log().LogMessage(log_level::error, std::format("failed to load mini dump thread list stream index:[{}]", parameters.StreamIndex()));
            return;
        }

        if(parameters.StreamSubType() != 0 || parameters.StreamSubIndex() >= thread_list.thread_list().NumberOfThreads)
        {
            logger::Log().LogMessage(log_level::error, std::format("failed to load mini dump thread list stream entry index:[{}], sub type:[{}], sub index:[{}]", parameters.StreamIndex(), parameters.StreamSubType(), parameters.StreamSubIndex()));
            return;
        }

        thread_.as<ThreadListStreamEntry>()->Set(parameters.StreamSubIndex(), thread_list.get_thread(parameters.StreamSubIndex()));
    }
}
