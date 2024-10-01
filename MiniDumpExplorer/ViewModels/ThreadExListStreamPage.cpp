#include "pch.h"
#include "ThreadExListStreamPage.h"

#include "DbgHelpUtils/thread_ex_list_stream.h"
#include "Models/ThreadExListStreamEntriesDataSource.h"

#if __has_include("ThreadExListStreamPage.g.cpp")
// ReSharper disable once CppUnusedIncludeDirective
#include "ThreadExListStreamPage.g.cpp" // NOLINT(bugprone-suspicious-include)
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    // ReSharper disable once CppDefaultedSpecialMemberFunctionIsImplicitlyDeleted
    ThreadExListStreamPage::ThreadExListStreamPage() = default;

    void ThreadExListStreamPage::InitializeComponent()
    {
        ThreadExListStreamPageT::InitializeComponent();

        onRowDoubleTapped_ = ThreadsExDataGrid().RowDoubleTapped({ this, &ThreadExListStreamPage::OnRowDoubleTapped });
    }

    // ReSharper disable once CppMemberFunctionMayBeStatic
    void ThreadExListStreamPage::SelectThread([[maybe_unused]] Windows::Foundation::IInspectable const& sender, [[maybe_unused]] RoutedEventArgs const& e) const
    {
    }

    // ReSharper disable once CppMemberFunctionMayBeStatic
    void ThreadExListStreamPage::OnRowDoubleTapped([[maybe_unused]] Windows::Foundation::IInspectable const& sender, [[maybe_unused]] DataGridRowDetailsEventArgs const& e) const
    {
    }

    void ThreadExListStreamPage::MiniDumpLoaded(MiniDumpExplorer::MiniDumpPageParameters const& parameters)
    {
        auto const miniDumpPage = parameters.MiniDump().as<MiniDumpPage>();
        auto const& miniDump = miniDumpPage->MiniDump();
        index_ = parameters.StreamIndex();

        dlg_help_utils::thread_ex_list_stream const thread_list{miniDump, parameters.StreamIndex()};

        if(!thread_list.found())
        {
            logger::Log().LogMessage(log_level::error, std::format("failed to load mini dump thread ex list stream index:[{}]", parameters.StreamIndex()));
            return;
        }

        total_entries_ = thread_list.thread_list().NumberOfThreads;
        threadsStreamEntriesDataSource_.as<ThreadExListStreamEntriesDataSource>()->LoadMiniDumpThreadStream(thread_list);
    }
}
