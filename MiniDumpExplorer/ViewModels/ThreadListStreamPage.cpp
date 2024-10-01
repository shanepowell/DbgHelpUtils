#include "pch.h"
#include "ThreadListStreamPage.h"

#include "DbgHelpUtils/thread_list_stream.h"
#include "Models/ThreadListStreamEntriesDataSource.h"
#include "Models/ThreadListStreamEntry.h"

#if __has_include("ThreadListStreamPage.g.cpp")
// ReSharper disable once CppUnusedIncludeDirective
#include "ThreadListStreamPage.g.cpp" // NOLINT(bugprone-suspicious-include)
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    // ReSharper disable once CppDefaultedSpecialMemberFunctionIsImplicitlyDeleted
    ThreadListStreamPage::ThreadListStreamPage() = default;

    void ThreadListStreamPage::InitializeComponent()
    {
        ThreadListStreamPageT::InitializeComponent();

        onRowDoubleTapped_ = ThreadsDataGrid().RowDoubleTapped({ this, &ThreadListStreamPage::OnRowDoubleTapped });
    }

    void ThreadListStreamPage::SelectThread([[maybe_unused]] Windows::Foundation::IInspectable const& sender, [[maybe_unused]] RoutedEventArgs const& e) const
    {
        SelectCurrentlySelectedEntry();
    }

    void ThreadListStreamPage::CreateNavigationViewItemEntriesForStream(Controls::NavigationViewItem const& item, MiniDumpExplorer::MiniDumpPageParameters const& parameters, dlg_help_utils::mini_dump const& mini_dump)
    {
        dlg_help_utils::thread_list_stream const thread_list{mini_dump, parameters.StreamIndex()};

        if(!thread_list.found())
        {
            logger::Log().LogMessage(log_level::error, std::format("failed to load mini dump thread list stream index:[{}]", parameters.StreamIndex()));
            return;
        }

        for (uint32_t index = 0; index < thread_list.thread_list().NumberOfThreads; ++index)
        {
            const Controls::NavigationViewItem newItem;
            newItem.Content(box_value(std::format(L"Thread: {}", index)));
            MiniDumpExplorer::MiniDumpPageParameters subParameters{parameters.MiniDump(), CreateFindNavigationTag(parameters.StreamIndex(), index), MiniDumpPage::ThreadListEntryStreamTag, parameters.StreamIndex(), 0, index};
            newItem.Tag(subParameters);
            const Controls::SymbolIcon iconSource;
            iconSource.Symbol(Controls::Symbol::Mail);
            newItem.Icon(iconSource);
            item.MenuItems().Append(newItem);
        }
    }

    void ThreadListStreamPage::OnRowDoubleTapped([[maybe_unused]] Windows::Foundation::IInspectable const& sender, [[maybe_unused]] DataGridRowDetailsEventArgs const& e) const
    {
        SelectCurrentlySelectedEntry();
    }

    void ThreadListStreamPage::MiniDumpLoaded(MiniDumpExplorer::MiniDumpPageParameters const& parameters)
    {
        miniDumpPage_ = parameters.MiniDump();
        auto const miniDumpPage = miniDumpPage_.as<MiniDumpPage>();
        auto const& miniDump = miniDumpPage->MiniDump();
        index_ = parameters.StreamIndex();

        dlg_help_utils::thread_list_stream const thread_list{miniDump, parameters.StreamIndex()};

        if(!thread_list.found())
        {
            logger::Log().LogMessage(log_level::error, std::format("failed to load mini dump thread list stream index:[{}]", parameters.StreamIndex()));
            return;
        }

        total_entries_ = thread_list.thread_list().NumberOfThreads;
        threadsStreamEntriesDataSource_.as<ThreadListStreamEntriesDataSource>()->LoadMiniDumpThreadStream(thread_list);
    }

    void ThreadListStreamPage::SelectCurrentlySelectedEntry() const
    {
        try
        {
            if (auto const selectedThread = threadsStreamEntriesDataSource_.CollectionView().CurrentItem();
                selectedThread)
            {
                if (auto const selectedThreadEntry = selectedThread.as<ThreadListStreamEntry>();
                    selectedThreadEntry)
                {
                    auto const miniDumpPage = miniDumpPage_.as<MiniDumpPage>();
                    miniDumpPage->SelectNavigationItemTag(CreateFindNavigationTag(index_, selectedThreadEntry->Index()));
                }
            }
        }
        catch (...)
        {
            logger::Log().HandleUnknownException();
        }
    }

    std::wstring ThreadListStreamPage::CreateFindNavigationTag(uint32_t index, uint32_t sub_index)
    {
        return std::format(L"ThreadListStreamPage_{}_{}", index, sub_index);
    }
}
