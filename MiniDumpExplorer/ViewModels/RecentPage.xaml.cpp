#include "pch.h"
#include "RecentPage.xaml.h"

#include "Helpers/GlobalOptions.h"
#include "Utility/logger.h"

#if __has_include("RecentPage.g.cpp")
// ReSharper disable once CppUnusedIncludeDirective
#include "RecentPage.g.cpp"  // NOLINT(bugprone-suspicious-include)
#endif


using namespace winrt;
using namespace Microsoft::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    RecentPage::RecentPage()
        : recentFileItems_{single_threaded_observable_vector<MiniDumpExplorer::RecentFileItem>()}
    {
        logger::Log().LogMessage(log_level::debug, "RecentPage");
        InitializeComponent();
        logger::Log().LogMessage(log_level::debug, "RecentPage post InitializeComponent");
    }

    void RecentPage::ClearAllRecentFiles([[maybe_unused]] Windows::Foundation::IInspectable const& sender, [[maybe_unused]] RoutedEventArgs const& e) const
    {
        recentFileItems_.Clear();
        GlobalOptions::Options().RecentFiles({});
    }
}
