#include "pch.h"
#include "TokenStreamPage.xaml.h"

#include "DbgHelpUtils/token_info_list_stream.h"
#include "Models/TokenStreamEntriesDataSource.h"

#if __has_include("TokenStreamPage.g.cpp")
// ReSharper disable once CppUnusedIncludeDirective
#include "TokenStreamPage.g.cpp"  // NOLINT(bugprone-suspicious-include)
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    // ReSharper disable once CppDefaultedSpecialMemberFunctionIsImplicitlyDeleted
    TokenStreamPage::TokenStreamPage() = default;

    void TokenStreamPage::InitializeComponent()
    {
        TokenStreamPageT::InitializeComponent();

        onRowDoubleTapped_ = TokensDataGrid().RowDoubleTapped({ this, &TokenStreamPage::OnRowDoubleTapped });
    }

    // ReSharper disable once CppMemberFunctionMayBeStatic
    void TokenStreamPage::SelectToken([[maybe_unused]] Windows::Foundation::IInspectable const& sender, [[maybe_unused]] RoutedEventArgs const& e) const
    {
    }

    // ReSharper disable once CppMemberFunctionMayBeStatic
    void TokenStreamPage::OnRowDoubleTapped([[maybe_unused]] Windows::Foundation::IInspectable const& sender, [[maybe_unused]] DataGridRowDetailsEventArgs const& e) const
    {
    }

    void TokenStreamPage::MiniDumpLoaded(MiniDumpExplorer::MiniDumpPageParameters const& parameters)
    {
        auto const miniDumpPage = parameters.MiniDump().as<MiniDumpPage>();
        auto const& miniDump = miniDumpPage->MiniDump();
        index_ = parameters.StreamIndex();

        dlg_help_utils::token_info_list_stream const token_stream{miniDump, parameters.StreamIndex()};

        if(!token_stream.found() || !token_stream.is_valid())
        {
            logger::Log().LogMessage(log_level::error, std::format("failed to load mini dump handle token stream index:[{}]", parameters.StreamIndex()));
            return;
        }

        total_entries_ = token_stream.size();
        tokenEntriesDataSource_.as<TokenStreamEntriesDataSource>()->LoadMiniDumpTokenStream(token_stream);
    }
}
