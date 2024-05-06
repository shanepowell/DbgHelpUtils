#include "pch.h"
#include "HandleOperationListStreamPage.xaml.h"

#include "DbgHelpUtils/handle_operation_list_stream.h"
#include "Models/HandleOperationListStreamEntriesDataSource.h"

#if __has_include("HandleOperationListStreamPage.g.cpp")
// ReSharper disable once CppUnusedIncludeDirective
#include "HandleOperationListStreamPage.g.cpp"  // NOLINT(bugprone-suspicious-include)
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    HandleOperationListStreamPage::HandleOperationListStreamPage() = default;

    void HandleOperationListStreamPage::InitializeComponent()
    {
        HandleOperationListStreamPageT::InitializeComponent();

        onRowDoubleTapped_ = HandleOperationDataGrid().RowDoubleTapped({ this, &HandleOperationListStreamPage::OnRowDoubleTapped });
    }

    void HandleOperationListStreamPage::SelectHandleOperation([[maybe_unused]] Windows::Foundation::IInspectable const& sender, [[maybe_unused]] RoutedEventArgs const& e) const
    {
    }

    void HandleOperationListStreamPage::OnRowDoubleTapped([[maybe_unused]] Windows::Foundation::IInspectable const& sender, [[maybe_unused]] DataGridRowDetailsEventArgs const& e) const
    {
    }

    void HandleOperationListStreamPage::MiniDumpLoaded(MiniDumpExplorer::MiniDumpPageParameters const& parameters)
    {
        auto const miniDumpPage = parameters.MiniDump().as<MiniDumpPage>();
        auto const& miniDump = miniDumpPage->MiniDump();
        index_ = parameters.StreamIndex();

        dlg_help_utils::handle_operation_list_stream const operation_list_stream{miniDump, parameters.StreamIndex()};

        if(!operation_list_stream.found())
        {
            logger::Log().LogMessage(log_level::error, std::format("failed to load mini dump handle operation list stream index:[{}]", parameters.StreamIndex()));
            return;
        }

        total_entries_ = operation_list_stream.size();
        handleOperationEntriesDataSource_.as<HandleOperationListStreamEntriesDataSource>()->LoadMiniDumpHandleOperationStream(operation_list_stream);
    }
}
