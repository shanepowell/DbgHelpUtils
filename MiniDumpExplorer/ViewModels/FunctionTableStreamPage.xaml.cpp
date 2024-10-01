#include "pch.h"
#include "FunctionTableStreamPage.xaml.h"

#include "DbgHelpUtils/function_info_utils.h"
#include "Models/FunctionTableStreamEntriesDataSource.h"

#if __has_include("FunctionTableStreamPage.g.cpp")
// ReSharper disable once CppUnusedIncludeDirective
#include "FunctionTableStreamPage.g.cpp"  // NOLINT(bugprone-suspicious-include)
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    // ReSharper disable once CppDefaultedSpecialMemberFunctionIsImplicitlyDeleted
    FunctionTableStreamPage::FunctionTableStreamPage() = default;

    void FunctionTableStreamPage::InitializeComponent()
    {
        FunctionTableStreamPageT::InitializeComponent();

        onRowDoubleTapped_ = FunctionTableDataGrid().RowDoubleTapped({ this, &FunctionTableStreamPage::OnRowDoubleTapped });
    }

    // ReSharper disable once CppMemberFunctionMayBeStatic
    void FunctionTableStreamPage::SelectFunction([[maybe_unused]] Windows::Foundation::IInspectable const& sender, [[maybe_unused]] RoutedEventArgs const& e) const
    {
    }

    // ReSharper disable once CppMemberFunctionMayBeStatic
    void FunctionTableStreamPage::OnRowDoubleTapped([[maybe_unused]] Windows::Foundation::IInspectable const& sender, [[maybe_unused]] DataGridRowDetailsEventArgs const& e) const
    {
    }

    void FunctionTableStreamPage::MiniDumpLoaded(MiniDumpExplorer::MiniDumpPageParameters const& parameters)
    {
        auto const miniDumpPage = parameters.MiniDump().as<MiniDumpPage>();
        auto const& miniDump = miniDumpPage->MiniDump();

        function_table_ = dlg_help_utils::function_table_stream{miniDump, parameters.StreamIndex()};

        if(!function_table_.found() || !function_table_.is_valid())
        {
            logger::Log().LogMessage(log_level::error, std::format("failed to load mini dump function table stream index:[{}]", parameters.StreamIndex()));
            return;
        }

        entry_type_ = dlg_help_utils::function_info_utils::function_entry_type_to_string(function_table_.entry_type());
        functionTableStreamEntriesDataSource_.as<FunctionTableStreamEntriesDataSource>()->LoadMiniDumpFunctionTableStream(function_table_);
    }
}
