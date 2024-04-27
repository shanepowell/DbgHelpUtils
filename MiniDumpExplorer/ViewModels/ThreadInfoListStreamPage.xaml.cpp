#include "pch.h"
#include "ThreadInfoListStreamPage.xaml.h"

#include "DbgHelpUtils/misc_info_stream.h"
#include "DbgHelpUtils/thread_info_list_stream.h"
#include "Models/ThreadInfoListStreamEntriesDataSource.h"

#if __has_include("ThreadInfoListStreamPage.g.cpp")
// ReSharper disable once CppUnusedIncludeDirective
#include "ThreadInfoListStreamPage.g.cpp" // NOLINT(bugprone-suspicious-include)
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    ThreadInfoListStreamPage::ThreadInfoListStreamPage() = default;

    void ThreadInfoListStreamPage::InitializeComponent()
    {
        ThreadInfoListStreamPageT::InitializeComponent();

        onRowDoubleTapped_ = ThreadsInfoDataGrid().RowDoubleTapped({ this, &ThreadInfoListStreamPage::OnRowDoubleTapped });
    }

    void ThreadInfoListStreamPage::SelectThread([[maybe_unused]] Windows::Foundation::IInspectable const& sender, [[maybe_unused]] RoutedEventArgs const& e) const
    {
    }

    void ThreadInfoListStreamPage::OnRowDoubleTapped([[maybe_unused]] Windows::Foundation::IInspectable const& sender, [[maybe_unused]] DataGridRowDetailsEventArgs const& e) const
    {
    }

    void ThreadInfoListStreamPage::MiniDumpLoaded(MiniDumpExplorer::MiniDumpPageParameters const& parameters)
    {
        auto const miniDumpPage = parameters.MiniDump().as<MiniDumpPage>();
        auto const& miniDump = miniDumpPage->MiniDump();

        auto const dump_file_timezone_info = dlg_help_utils::misc_info_stream::get_dump_file_timezone_info(miniDump);
        dlg_help_utils::thread_info_list_stream const thread_list{miniDump, parameters.StreamIndex()};

        if(!thread_list.found())
        {
            logger::Log().LogMessage(log_level::error, std::format("failed to load mini dump thread info list stream index:[{}]", parameters.StreamIndex()));
            return;
        }

        threadsInfoStreamEntriesDataSource_.as<ThreadInfoListStreamEntriesDataSource>()->LoadMiniDumpThreadInfoStream(thread_list, dump_file_timezone_info);
    }
}
