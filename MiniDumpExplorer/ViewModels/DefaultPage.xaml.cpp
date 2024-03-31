#include "pch.h"
#include "DefaultPage.xaml.h"

#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Interop.h>

#include "Utility/logger.h"

#if __has_include("DefaultPage.g.cpp")
// ReSharper disable once CppUnusedIncludeDirective
#include "DefaultPage.g.cpp"  // NOLINT(bugprone-suspicious-include)

#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;
using namespace std::string_literals;

namespace
{
    const std::wstring SettingsTag = L"Settings"s;
    const std::wstring RecentTag = L"Recent"s;
}

namespace winrt::MiniDumpExplorer::implementation
{
    std::unordered_map<std::wstring, Windows::UI::Xaml::Interop::TypeName> DefaultPage::pageMap_ =
        {
            { RecentTag, xaml_typename<RecentPage>() },
            { SettingsTag, xaml_typename<SettingsPage>() },
        };

    void DefaultPage::InitializeComponent()
    {
        DefaultPageT::InitializeComponent();
        logger::Log().LogMessage(log_level::debug, "Navigate to default pages recent page");
        ContentFrame().Navigate(xaml_typename<RecentPage>());
    }

    void DefaultPage::NavigationView_SelectionChanged([[maybe_unused]] Controls::NavigationView const& sender, Controls::NavigationViewSelectionChangedEventArgs const& args)
    {
        if(const auto navItemTag = args.IsSettingsSelected() ? SettingsTag : static_cast<std::wstring>(unbox_value<hstring>(args.SelectedItemContainer().Tag())); 
            !navItemTag.empty())
        {
            if(auto const it = pageMap_.find(navItemTag); it != pageMap_.end())
            {
                ContentFrame().Navigate(it->second);
            }
        }
    }
}
