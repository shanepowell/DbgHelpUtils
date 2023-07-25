#include "pch.h"
#include "DefaultPage.xaml.h"

#include <winrt/Windows.UI.Core.h>

#include "App.xaml.h"
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
    const std::wstring settingsTag = L"Settings"s;
}

namespace winrt::MiniDumpExplorer::implementation
{
    std::unordered_map<std::wstring, Windows::UI::Xaml::Interop::TypeName> DefaultPage::pageMap_ =
        {
            { L"Recent", xaml_typename<RecentPage>() },
            { settingsTag, xaml_typename<SettingsPage>() },
        };

    DefaultPage::DefaultPage()
    {
        logger::Log().LogMessage(log_level::debug, "DefaultPage");
        InitializeComponent();
        logger::Log().LogMessage(log_level::debug, "DefaultPage post InitializeComponent");
    }

    void DefaultPage::NavigationView_SelectionChanged([[maybe_unused]] Controls::NavigationView const& sender, Controls::NavigationViewSelectionChangedEventArgs const& args)
    {
        if(const auto navItemTag = args.IsSettingsSelected() ? settingsTag : static_cast<std::wstring>(unbox_value<hstring>(args.SelectedItemContainer().Tag())); 
            !navItemTag.empty())
        {
            if(auto const it = pageMap_.find(navItemTag); it != pageMap_.end())
            {
                ContentFrame().Navigate(it->second);
            }
        }
    }
}
