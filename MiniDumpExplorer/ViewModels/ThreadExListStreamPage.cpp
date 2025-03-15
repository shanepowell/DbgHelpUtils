#include "pch.h"
#include "ThreadExListStreamPage.h"

#include "DbgHelpUtils/thread_ex_list_stream.h"
#include "Models/ThreadExListStreamEntriesDataSource.h"
#include "Models/ThreadExListStreamEntry.h"

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

    void ThreadExListStreamPage::SelectThread([[maybe_unused]] Windows::Foundation::IInspectable const& sender, [[maybe_unused]] RoutedEventArgs const& e) const
    {
        SelectCurrentlySelectedEntry();
    }

    void ThreadExListStreamPage::OnRowDoubleTapped([[maybe_unused]] Windows::Foundation::IInspectable const& sender, [[maybe_unused]] DataGridRowDetailsEventArgs const& e) const
    {
        SelectCurrentlySelectedEntry();
    }

    void ThreadExListStreamPage::MiniDumpLoaded(MiniDumpExplorer::MiniDumpPageParameters const& parameters)
    {
        miniDumpPage_ = parameters.MiniDump();
        auto const miniDumpPage = miniDumpPage_.as<MiniDumpPage>();
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

    void ThreadExListStreamPage::SelectCurrentlySelectedEntry() const
    {
        try
        {
            if (auto const selectedThread = threadsStreamEntriesDataSource_.CollectionView().CurrentItem();
                selectedThread)
            {
                if (auto const selectedThreadEntry = selectedThread.as<ThreadExListStreamEntry>();
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

    void ThreadExListStreamPage::CreateNavigationViewItemEntriesForStream(Controls::NavigationViewItem const& item, MiniDumpExplorer::MiniDumpPageParameters const& parameters, dlg_help_utils::mini_dump const& miniDump)
    {
        dlg_help_utils::thread_ex_list_stream const thread_list{miniDump, parameters.StreamIndex()};

        if(!thread_list.found())
        {
            logger::Log().LogMessage(log_level::error, std::format("failed to load mini dump thread ex list stream index:[{}]", parameters.StreamIndex()));
            return;
        }

        for (uint32_t index = 0; index < thread_list.thread_list().NumberOfThreads; ++index)
        {
            const Controls::NavigationViewItem newItem;
            newItem.Content(box_value(std::format(L"Thread: {}", index)));
            MiniDumpExplorer::MiniDumpPageParameters subParameters{parameters.MiniDump(), CreateFindNavigationTag(parameters.StreamIndex(), index), MiniDumpPage::ThreadExListEntryStreamTag, parameters.StreamIndex(), 0, index};
            newItem.Tag(subParameters);
            const Controls::SymbolIcon iconSource;
            iconSource.Symbol(Controls::Symbol::Mail);
            newItem.Icon(iconSource);
            item.MenuItems().Append(newItem);
        }
    }

    std::wstring ThreadExListStreamPage::CreateFindNavigationTag(size_t const index, size_t const sub_index)
    {
        return std::format(L"ThreadExListStreamPage_{}_{}", index, sub_index);
    }
}
