#include "pch.h"
#include "MemoryListStreamPage.xaml.h"

#include "DbgHelpUtils/memory_list_stream.h"
#include "Models/MemoryListStreamEntriesDataSource.h"

#if __has_include("MemoryListStreamPage.g.cpp")
// ReSharper disable once CppUnusedIncludeDirective
#include "MemoryListStreamPage.g.cpp"  // NOLINT(bugprone-suspicious-include)
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    MemoryListStreamPage::MemoryListStreamPage() = default;

    void MemoryListStreamPage::InitializeComponent()
    {
        MemoryListStreamPageT::InitializeComponent();

        onRowDoubleTapped_ = MemoryDataGrid().RowDoubleTapped({ this, &MemoryListStreamPage::OnRowDoubleTapped });
    }

    void MemoryListStreamPage::SelectMemoryRange([[maybe_unused]] Windows::Foundation::IInspectable const& sender, [[maybe_unused]] RoutedEventArgs const& e) const
    {
    }

    void MemoryListStreamPage::OnRowDoubleTapped([[maybe_unused]] Windows::Foundation::IInspectable const& sender, [[maybe_unused]] DataGridRowDetailsEventArgs const& e) const
    {
    }

    void MemoryListStreamPage::MiniDumpLoaded(MiniDumpExplorer::MiniDumpPageParameters const& parameters)
    {
        auto const miniDumpPage = parameters.MiniDump().as<MiniDumpPage>();
        auto const& miniDump = miniDumpPage->MiniDump();
        index_ = parameters.StreamIndex();

        dlg_help_utils::memory_list_stream const memory_list{miniDump, parameters.StreamIndex()};

        if(!memory_list.found())
        {
            logger::Log().LogMessage(log_level::error, std::format("failed to load mini dump memory list stream index:[{}]", parameters.StreamIndex()));
            return;
        }

        total_entries_ = memory_list.memory_list().NumberOfMemoryRanges;
        memoryStreamEntriesDataSource_.as<MemoryListStreamEntriesDataSource>()->LoadMiniDumpMemoryStream(memory_list);
    }
}
