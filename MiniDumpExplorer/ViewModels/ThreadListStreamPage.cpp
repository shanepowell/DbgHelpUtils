#include "pch.h"
#include "ThreadListStreamPage.h"

#include "DbgHelpUtils/thread_list_stream.h"
#include "Models/ThreadListStreamEntriesDataSource.h"

#if __has_include("ThreadListStreamPage.g.cpp")
// ReSharper disable once CppUnusedIncludeDirective
#include "ThreadListStreamPage.g.cpp" // NOLINT(bugprone-suspicious-include)
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    ThreadListStreamPage::ThreadListStreamPage() = default;

    void ThreadListStreamPage::InitializeComponent()
    {
        ThreadListStreamPageT::InitializeComponent();

        onRowDoubleTapped_ = ThreadsDataGrid().RowDoubleTapped({ this, &ThreadListStreamPage::OnRowDoubleTapped });
    }

    void ThreadListStreamPage::SelectThread([[maybe_unused]] Windows::Foundation::IInspectable const& sender, [[maybe_unused]] RoutedEventArgs const& e) const
    {
    }

    void ThreadListStreamPage::OnRowDoubleTapped([[maybe_unused]] Windows::Foundation::IInspectable const& sender, [[maybe_unused]] DataGridRowDetailsEventArgs const& e) const
    {
    }

    void ThreadListStreamPage::MiniDumpLoaded(MiniDumpExplorer::MiniDumpPageParameters const& parameters)
    {
        auto const miniDumpPage = parameters.MiniDump().as<MiniDumpPage>();
        auto const& miniDump = miniDumpPage->MiniDump();

        dlg_help_utils::thread_list_stream const thread_list{miniDump, parameters.StreamIndex()};

        if(!thread_list.found())
        {
            logger::Log().LogMessage(log_level::error, std::format("failed to load mini dump thread list stream index:[{}]", parameters.StreamIndex()));
            return;
        }

        threadsStreamEntriesDataSource_.as<ThreadListStreamEntriesDataSource>()->LoadMiniDumpThreadStream(thread_list);
    }
}
