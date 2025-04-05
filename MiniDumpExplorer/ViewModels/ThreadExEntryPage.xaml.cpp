#include "pch.h"
#include "ThreadExEntryPage.xaml.h"

#include "DbgHelpUtils/thread_ex_list_stream.h"
#include "Helpers/UIHelper.h"
#include "Models/ThreadExListStreamEntry.h"

#if __has_include("ThreadExEntryPage.g.cpp")
// ReSharper disable once CppUnusedIncludeDirective
#include "ThreadExEntryPage.g.cpp" // NOLINT(bugprone-suspicious-include)
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    // ReSharper disable once CppDefaultedSpecialMemberFunctionIsImplicitlyDeleted
    ThreadExEntryPage::ThreadExEntryPage() = default;

    void ThreadExEntryPage::InitializeComponent()
    {
        ThreadExEntryPageT::InitializeComponent();
        SetupFlyoutMenus();
    }

    void ThreadExEntryPage::SetupFlyoutMenus()
    {
        UIHelper::CreateStandardHexNumberMenu(
            threadId(),
            threadPriority(),
            threadTEB()
        );
    }

    void ThreadExEntryPage::MiniDumpLoaded(MiniDumpExplorer::MiniDumpPageParameters const& parameters)
    {
        auto const miniDumpPage = parameters.MiniDump().as<MiniDumpPage>();
        auto const& miniDump = miniDumpPage->MiniDump();

        dlg_help_utils::thread_ex_list_stream const thread_list{miniDump, parameters.StreamIndex()};

        if(!thread_list.found())
        {
            logger::Log().LogMessage(log_level::error, std::format("failed to load mini dump thread ex list stream index:[{}]", parameters.StreamIndex()));
            return;
        }

        if(parameters.StreamSubType() != 0 || parameters.StreamSubIndex() >= thread_list.thread_list().NumberOfThreads)
        {
            logger::Log().LogMessage(log_level::error, std::format("failed to load mini dump thread ex list stream entry index:[{}], sub type:[{}], sub index:[{}]", parameters.StreamIndex(), parameters.StreamSubType(), parameters.StreamSubIndex()));
            return;
        }

        auto thread = thread_.as<ThreadExListStreamEntry>();
        thread->Set(parameters.StreamSubIndex(), thread_list.get_thread(parameters.StreamSubIndex()));
        thread->LoadStack(miniDump);
    }
}
