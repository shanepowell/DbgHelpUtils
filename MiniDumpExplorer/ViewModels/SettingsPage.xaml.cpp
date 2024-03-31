#include "pch.h"
#include "SettingsPage.xaml.h"

#include "Helpers/GlobalOptions.h"
#include "Helpers/ThemeHelper.h"
#include "Helpers/UIHelper.h"
#include "Helpers/WindowHelper.h"
#include "Utility/logger.h"

#include "DbgHelpUtils/file_version_info.h"
#include "DbgHelpUtils/stream_hex_dump.h"
#include "DbgHelpUtils/system_info_utils.h"
#include "DbgHelpUtils/windows_error.h"

#include <winrt/Microsoft.Windows.ApplicationModel.Resources.h>

#if __has_include("SettingsPage.g.cpp")
// ReSharper disable once CppUnusedIncludeDirective
#include "SettingsPage.g.cpp"  // NOLINT(bugprone-suspicious-include)
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;
using namespace dlg_help_utils::stream_hex_dump;
using namespace dlg_help_utils;

namespace winrt::MiniDumpExplorer::implementation
{
    namespace
    {
        auto const ThemeDarkTag = L"Dark";
        auto const ThemeLightTag = L"Light";
    }

    SettingsPage::SettingsPage()
        : GlobalOptionsNotifyPropertyChangedBase({ L"SuiteMask" },{ L"ExampleSize" })
    {
    }

    void SettingsPage::InitializeComponent()
    {
        SettingsPageT::InitializeComponent();

        LoadVersionInformation();
        onSettingsPageLoadedEvent_ = Loaded(RoutedEventHandler{this, &SettingsPage::OnSettingsPageLoaded});
        SetupFlyoutMenus();
    }

    void SettingsPage::OnSettingsPageLoaded([[maybe_unused]] Windows::Foundation::IInspectable const& sender, [[maybe_unused]] RoutedEventArgs const& e)
    {
        switch (ThemeHelper::RootTheme())
        {
        case ElementTheme::Light:
            themeMode().SelectedIndex(0);
            break;
        case ElementTheme::Dark:
            themeMode().SelectedIndex(1);
            break;
        case ElementTheme::Default:
            themeMode().SelectedIndex(2);
            break;
        default:
            break;
        }

        logLevelMode().SelectedIndex(static_cast<int>(logger::Log().LogLevel()));

        auto const& options = GlobalOptions::Options();
        numberFormatMode().SelectedIndex(static_cast<int>(options.NumberDisplayFormat()));

        unitSizeFormatMode().SelectedIndex(static_cast<int>(options.SizeNumberDisplayFormat()));
        unitFormatMode().SelectedIndex(static_cast<int>(options.SizeFormat()));
        unitBaseMode().SelectedIndex(static_cast<int>(options.SizeBase()));
    }

    void SettingsPage::LoadVersionInformation()
    {
        Microsoft::Windows::ApplicationModel::Resources::ResourceManager const rm{};
        auto versionString = rm.MainResourceMap().GetValue(L"Resources/Version").ValueAsString();
        auto const& path = system_info_utils::get_running_application_path();
        file_version_info const version_info{path.wstring()};
        version_ = hstring{std::format(L"{0} {1}", versionString, version_info.product_version())};

        OSVERSIONINFOEXW info{};
        info.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEXW);
#pragma warning(push)
#pragma warning(disable: 4996 28159)
        // ReSharper disable CppDeprecatedEntity
        if (!GetVersionExW(reinterpret_cast<OSVERSIONINFOW*>(&info))) // NOLINT(clang-diagnostic-deprecated-declarations)
        {
            auto const ec = GetLastError();
            logger::Log().LogMessage(log_level::error, std::format(L"GetVersionExW failed: {0} - {1}\n", to_hex(ec), windows_error::get_windows_error_string(ec)));
        }
        // ReSharper restore CppDeprecatedEntity
#pragma warning(pop)

        SYSTEM_INFO system_info{};
        GetSystemInfo(&system_info);

        osVersion_ = std::format(L"{0}.{1}.{2} - {3}", info.dwMajorVersion, info.dwMinorVersion, info.dwBuildNumber, system_info_utils::windows_version_to_string(info.dwMajorVersion, info.dwMinorVersion, info.dwBuildNumber, info.wProductType, system_info.wProcessorArchitecture, info.wSuiteMask));
        suiteMask_ = info.wSuiteMask;
        for (auto const& value : system_info_utils::suite_mask_to_strings(info.wSuiteMask))
        {
            suiteMaskList_.Append(value);
        }
        productType_ = system_info_utils::product_type_to_string(info.wProductType);
        platformId_ = system_info_utils::platform_id_to_string(info.dwPlatformId);
        csdVersion_ = info.szCSDVersion;
        processorArchitecture_ = system_info_utils::processor_architecture_to_string(system_info.wProcessorArchitecture);
    }

    void SettingsPage::OnNumberDisplayFormatChanged()
    {
        numberFormatMode().SelectedIndex(static_cast<int>(GlobalOptions::Options().NumberDisplayFormat()));
    }

    void SettingsPage::OnSizeNumberDisplayFormatChanged()
    {
        auto const& options = GlobalOptions::Options();
        unitSizeFormatMode().SelectedIndex(static_cast<int>(options.SizeNumberDisplayFormat()));
        unitFormatMode().SelectedIndex(static_cast<int>(options.SizeFormat()));
        unitBaseMode().SelectedIndex(static_cast<int>(options.SizeBase()));
    }

    uint64_t SettingsPage::ExampleSize()
    {
        return 1234567890123456789ULL;
    }

    void SettingsPage::ThemeModeSelectionChanged(Windows::Foundation::IInspectable const& sender, [[maybe_unused]] RoutedEventArgs const& e)
    {
        const auto themeSelectedItem = themeMode().SelectedItem().try_as<Controls::ComboBoxItem>();
        const auto selectedTheme = themeSelectedItem ? unbox_value<hstring>(themeSelectedItem.Tag()) : hstring{};
        auto window = WindowHelper::GetWindowForElement(*this);
        hstring colour;
        if (selectedTheme.empty())
        {
            return;
        }

        Microsoft::Windows::ApplicationModel::Resources::ResourceManager const rm{};

        auto const themeDarkName = rm.MainResourceMap().GetValue(L"Resources/ThemeDarkName").ValueAsString();
        auto const themeLightName = rm.MainResourceMap().GetValue(L"Resources/ThemeLightName").ValueAsString();

        if (selectedTheme == ThemeDarkTag)
        {
            ThemeHelper::RootTheme(ElementTheme::Dark);
            colour = themeDarkName;
        }
        else if (selectedTheme == ThemeLightTag)
        {
            ThemeHelper::RootTheme(ElementTheme::Light);
            colour = themeLightName;
        }
        else
        {
            ThemeHelper::RootTheme(ElementTheme::Default);
            colour = ThemeHelper::WindowActualTheme() == ElementTheme::Dark ? themeDarkName : themeLightName;
        }

        // announce visual change to automation
        auto const themeChangedNotificationMessage = std::format(L"{} {}", rm.MainResourceMap().GetValue(L"Resources/ThemeChangedMessage").ValueAsString(), colour);
        logger::Log().LogMessage(log_level::info, themeChangedNotificationMessage);
        UIHelper::AnnounceActionForAccessibility(sender.as<UIElement>(), hstring{themeChangedNotificationMessage}, L"ThemeChangedNotificationActivityId");
    }

    void SettingsPage::LogLevelModeSelectionChanged([[maybe_unused]] Windows::Foundation::IInspectable const& sender, [[maybe_unused]] RoutedEventArgs const& e)
    {
        const auto new_log_level = static_cast<log_level>(logLevelMode().SelectedIndex());
        logger::Log().LogLevel(new_log_level);
    }

    void SettingsPage::NumberFormatModeSelectionChanged([[maybe_unused]] Windows::Foundation::IInspectable const& sender, [[maybe_unused]] RoutedEventArgs const& e)
    {
        const auto new_number_format = static_cast<NumberDisplayFormat>(numberFormatMode().SelectedIndex());
        GlobalOptions::Options().NumberDisplayFormat(new_number_format);
    }

    void SettingsPage::UnitSizeFormatModeSelectionChanged([[maybe_unused]] Windows::Foundation::IInspectable const& sender, [[maybe_unused]] RoutedEventArgs const& e)
    {
        const auto value = static_cast<SizeNumberDisplayFormat>(unitSizeFormatMode().SelectedIndex());
        GlobalOptions::Options().SizeNumberDisplayFormat(value);
    }

    void SettingsPage::UnitFormatModeSelectionChanged([[maybe_unused]] Windows::Foundation::IInspectable const& sender, [[maybe_unused]] RoutedEventArgs const& e)
    {
        const auto value = static_cast<size_units::print>(unitFormatMode().SelectedIndex());
        GlobalOptions::Options().SizeFormat(value);
    }

    void SettingsPage::UnitBaseModeSelectionChanged([[maybe_unused]] Windows::Foundation::IInspectable const& sender, [[maybe_unused]] RoutedEventArgs const& e)
    {
        const auto value = static_cast<SizeDisplayNumberBase>(unitBaseMode().SelectedIndex());
        GlobalOptions::Options().SizeBase(value);
    }

    void SettingsPage::SetupFlyoutMenus()
    {
        UIHelper::CreateStandardHexNumberMenu(suiteMaskTextBlock());
        UIHelper::CreateStandardSizeNumberMenu(exampleSizeDisplay());
    }
}


