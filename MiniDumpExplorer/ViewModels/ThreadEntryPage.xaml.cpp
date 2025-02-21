#include "pch.h"
#include "ThreadEntryPage.xaml.h"

#include "DbgHelpUtils/thread_list_stream.h"
#include "Helpers/UIHelper.h"
#include "Models/ThreadListStreamEntry.h"
#include "Models/ThreadStackEntry.h"

#if __has_include("ThreadEntryPage.g.cpp")
// ReSharper disable once CppUnusedIncludeDirective
#include "ThreadEntryPage.g.cpp" // NOLINT(bugprone-suspicious-include)
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    ThreadEntryPage::ThreadEntryPage() = default;

    void ThreadEntryPage::InitializeComponent()
    {
        ThreadEntryPageT::InitializeComponent();
        SetupFlyoutMenus();
    }

    void ThreadEntryPage::OnTreeViewItemExpanding([[maybe_unused]] Controls::TreeView const& sender, Controls::TreeViewExpandingEventArgs const& args)
    {
        if (auto const item = args.Item().as<ThreadStackEntry>();
            item && !item->AreChildrenLoaded())
        {
            item->LoadChildren();
        }
    }

    void ThreadEntryPage::SetupFlyoutMenus()
    {
        UIHelper::CreateStandardHexNumberMenu(
            threadId(),
            threadPriority(),
            threadTEB(),
            stackStartOfMemoryRange(),
            stackEndOfMemoryRange()
        );

        UIHelper::CreateStandardSizeNumberMenu(
            stackDataSize()
        );
    }

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

        auto thread = thread_.as<ThreadListStreamEntry>();
        thread->Set(parameters.StreamSubIndex(), thread_list.get_thread(parameters.StreamSubIndex()));
        thread->LoadStack(miniDump);
    }
}
