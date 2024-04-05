#include "pch.h"
#include "Memory64ListStreamPage.xaml.h"

#include "DbgHelpUtils/stream_hex_dump.h"
#include "DbgHelpUtils/memory64_list_stream.h"
#include "Models/Memory64ListStreamEntriesDataSource.h"

#if __has_include("Memory64ListStreamPage.g.cpp")
// ReSharper disable once CppUnusedIncludeDirective
#include "Memory64ListStreamPage.g.cpp"  // NOLINT(bugprone-suspicious-include)
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    Memory64ListStreamPage::Memory64ListStreamPage() = default;

    void Memory64ListStreamPage::InitializeComponent()
    {
        Memory64ListStreamPageT::InitializeComponent();

        onRowDoubleTapped_ = Memory64DataGrid().RowDoubleTapped({ this, &Memory64ListStreamPage::OnRowDoubleTapped });
    }

    void Memory64ListStreamPage::SelectMemoryRange([[maybe_unused]] Windows::Foundation::IInspectable const& sender, [[maybe_unused]] RoutedEventArgs const& e) const
    {
    }

    void Memory64ListStreamPage::OnRowDoubleTapped([[maybe_unused]] Windows::Foundation::IInspectable const& sender, [[maybe_unused]] DataGridRowDetailsEventArgs const& e) const
    {
    }

    void Memory64ListStreamPage::MiniDumpLoaded(MiniDumpExplorer::MiniDumpPageParameters const& parameters)
    {
        auto const miniDumpPage = parameters.MiniDump().as<MiniDumpPage>();
        auto const& miniDump = miniDumpPage->MiniDump();

        dlg_help_utils::memory64_list_stream const memory_list{miniDump, parameters.StreamIndex()};

        if(!memory_list.found())
        {
            logger::Log().LogMessage(log_level::error, std::format("failed to load mini dump memory64 list stream index:[{}]", parameters.StreamIndex()));
            return;
        }

        memoryStreamEntriesDataSource_.as<Memory64ListStreamEntriesDataSource>()->LoadMiniDumpMemoryStream(memory_list);
    }
}
