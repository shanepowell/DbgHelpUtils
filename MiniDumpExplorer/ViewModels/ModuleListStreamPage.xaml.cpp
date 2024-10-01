#include "pch.h"
#include "ModuleListStreamPage.xaml.h"

#include "DbgHelpUtils/misc_info_stream.h"
#include "DbgHelpUtils/module_list_stream.h"
#include "Models/ModuleListStreamEntriesDataSource.h"

#if __has_include("ModuleListStreamPage.g.cpp")
// ReSharper disable once CppUnusedIncludeDirective
#include "ModuleListStreamPage.g.cpp"  // NOLINT(bugprone-suspicious-include)
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    // ReSharper disable once CppDefaultedSpecialMemberFunctionIsImplicitlyDeleted
    ModuleListStreamPage::ModuleListStreamPage() = default;

    void ModuleListStreamPage::InitializeComponent()
    {
        ModuleListStreamPageT::InitializeComponent();

        onRowDoubleTapped_ = ModulesDataGrid().RowDoubleTapped({ this, &ModuleListStreamPage::OnRowDoubleTapped });
    }

    // ReSharper disable once CppMemberFunctionMayBeStatic
    void ModuleListStreamPage::SelectModule([[maybe_unused]] Windows::Foundation::IInspectable const& sender, [[maybe_unused]] RoutedEventArgs const& e) const
    {
    }

    // ReSharper disable once CppMemberFunctionMayBeStatic
    void ModuleListStreamPage::OnRowDoubleTapped([[maybe_unused]] Windows::Foundation::IInspectable const& sender, [[maybe_unused]] DataGridRowDetailsEventArgs const& e) const
    {
    }

    void ModuleListStreamPage::MiniDumpLoaded(MiniDumpExplorer::MiniDumpPageParameters const& parameters)
    {
        auto const miniDumpPage = parameters.MiniDump().as<MiniDumpPage>();
        auto const& miniDump = miniDumpPage->MiniDump();
        index_ = parameters.StreamIndex();

        auto const dump_file_timezone_info = dlg_help_utils::misc_info_stream::get_dump_file_timezone_info(miniDump);
        dlg_help_utils::module_list_stream const module_list{miniDump, parameters.StreamIndex()};

        if(!module_list.found())
        {
            logger::Log().LogMessage(log_level::error, std::format("failed to load mini dump module list stream index:[{}]", parameters.StreamIndex()));
            return;
        }

        total_entries_ = module_list.module_list().NumberOfModules;
        modulesStreamEntriesDataSource_.as<ModuleListStreamEntriesDataSource>()->LoadMiniDumpModuleStream(module_list, dump_file_timezone_info);
    }
}
