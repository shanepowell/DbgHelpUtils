#include "pch.h"
#include "MemoryInfoListStreamPage.xaml.h"

#include "DbgHelpUtils/memory_info_list_stream.h"
#include "Models/MemoryInfoListStreamEntriesDataSource.h"

#if __has_include("MemoryInfoListStreamPage.g.cpp")
// ReSharper disable once CppUnusedIncludeDirective
#include "MemoryInfoListStreamPage.g.cpp"  // NOLINT(bugprone-suspicious-include)
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    MemoryInfoListStreamPage::MemoryInfoListStreamPage() = default;

    void MemoryInfoListStreamPage::InitializeComponent()
    {
        MemoryInfoListStreamPageT::InitializeComponent();

        onRowDoubleTapped_ = MemoryInfoDataGrid().RowDoubleTapped({ this, &MemoryInfoListStreamPage::OnRowDoubleTapped });
    }

    void MemoryInfoListStreamPage::SelectMemoryRange([[maybe_unused]] Windows::Foundation::IInspectable const& sender, [[maybe_unused]] RoutedEventArgs const& e) const
    {
    }

    void MemoryInfoListStreamPage::OnRowDoubleTapped([[maybe_unused]] Windows::Foundation::IInspectable const& sender, [[maybe_unused]] DataGridRowDetailsEventArgs const& e) const
    {
    }

    void MemoryInfoListStreamPage::MiniDumpLoaded(MiniDumpExplorer::MiniDumpPageParameters const& parameters)
    {
        auto const miniDumpPage = parameters.MiniDump().as<MiniDumpPage>();
        auto const& miniDump = miniDumpPage->MiniDump();
        index_ = parameters.StreamIndex();

        dlg_help_utils::memory_info_list_stream const memory_list{miniDump, parameters.StreamIndex()};

        if(!memory_list.found())
        {
            logger::Log().LogMessage(log_level::error, std::format("failed to load mini dump memory info list stream index:[{}]", parameters.StreamIndex()));
            return;
        }

        total_entries_ = memory_list.size();
        memoryInfoStreamEntriesDataSource_.as<MemoryInfoListStreamEntriesDataSource>()->LoadMiniDumpMemoryStream(memory_list);
    }
}
