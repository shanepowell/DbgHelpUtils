#include "pch.h"
#include "UnloadedModuleListStreamPage.xaml.h"

#include "DbgHelpUtils/misc_info_stream.h"
#include "DbgHelpUtils/unloaded_module_list_stream.h"
#include "Models/UnloadedModuleListStreamEntriesDataSource.h"

#if __has_include("UnloadedModuleListStreamPage.g.cpp")
// ReSharper disable once CppUnusedIncludeDirective
#include "UnloadedModuleListStreamPage.g.cpp" // NOLINT(bugprone-suspicious-include)
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    UnloadedModuleListStreamPage::UnloadedModuleListStreamPage() = default;

    void UnloadedModuleListStreamPage::InitializeComponent()
    {
        UnloadedModuleListStreamPageT::InitializeComponent();

        onRowDoubleTapped_ = UnloadedModulesDataGrid().RowDoubleTapped({ this, &UnloadedModuleListStreamPage::OnRowDoubleTapped });
    }

    void UnloadedModuleListStreamPage::SelectModule([[maybe_unused]] Windows::Foundation::IInspectable const& sender, [[maybe_unused]] RoutedEventArgs const& e) const
    {
    }

    void UnloadedModuleListStreamPage::OnRowDoubleTapped([[maybe_unused]] Windows::Foundation::IInspectable const& sender, [[maybe_unused]] DataGridRowDetailsEventArgs const& e) const
    {
    }

    void UnloadedModuleListStreamPage::MiniDumpLoaded(MiniDumpExplorer::MiniDumpPageParameters const& parameters)
    {
        auto const miniDumpPage = parameters.MiniDump().as<MiniDumpPage>();
        auto const& miniDump = miniDumpPage->MiniDump();
        index_ = parameters.StreamIndex();

        dlg_help_utils::unloaded_module_list_stream const module_list{miniDump, parameters.StreamIndex()};

        if(!module_list.found())
        {
            logger::Log().LogMessage(log_level::error, std::format("failed to load mini dump unloaded module list stream index:[{}]", parameters.StreamIndex()));
            return;
        }

        total_entries_ = module_list.size();
        unloadedModulesStreamEntriesDataSource_.as<UnloadedModuleListStreamEntriesDataSource>()->LoadMiniDumpUnloadedModuleStream(module_list);
    }
}
