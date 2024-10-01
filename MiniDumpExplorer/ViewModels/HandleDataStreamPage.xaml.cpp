#include "pch.h"
#include "HandleDataStreamPage.xaml.h"

#include "Models/HandleDataStreamEntriesDataSource.h"

#if __has_include("HandleDataStreamPage.g.cpp")
// ReSharper disable once CppUnusedIncludeDirective
#include "HandleDataStreamPage.g.cpp"  // NOLINT(bugprone-suspicious-include)
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    // ReSharper disable once CppDefaultedSpecialMemberFunctionIsImplicitlyDeleted
    HandleDataStreamPage::HandleDataStreamPage() = default;

    void HandleDataStreamPage::InitializeComponent()
    {
        HandleDataStreamPageT::InitializeComponent();

        onRowDoubleTapped_ = HandleTypesDataGrid().RowDoubleTapped({ this, &HandleDataStreamPage::OnRowDoubleTapped });
    }

    // ReSharper disable once CppMemberFunctionMayBeStatic
    void HandleDataStreamPage::SelectHandleType([[maybe_unused]] Windows::Foundation::IInspectable const& sender, [[maybe_unused]] RoutedEventArgs const& e) const
    {
    }

    // ReSharper disable once CppMemberFunctionMayBeStatic
    void HandleDataStreamPage::OnRowDoubleTapped([[maybe_unused]] Windows::Foundation::IInspectable const& sender, [[maybe_unused]] DataGridRowDetailsEventArgs const& e) const
    {
    }

    void HandleDataStreamPage::MiniDumpLoaded(MiniDumpExplorer::MiniDumpPageParameters const& parameters)
    {
        auto const miniDumpPage = parameters.MiniDump().as<MiniDumpPage>();
        auto const& miniDump = miniDumpPage->MiniDump();

        index_ = parameters.StreamIndex();
        handle_data_ = dlg_help_utils::handle_data_stream{miniDump, parameters.StreamIndex()};

        if(!handle_data_.found() || !handle_data_.is_valid())
        {
            logger::Log().LogMessage(log_level::error, std::format("failed to load mini dump handle data stream index:[{}]", parameters.StreamIndex()));
            return;
        }

        handleDataStreamEntriesDataSource_.as<HandleDataStreamEntriesDataSource>()->LoadMiniDumpHandleDataStream(handle_data_);
    }
}
