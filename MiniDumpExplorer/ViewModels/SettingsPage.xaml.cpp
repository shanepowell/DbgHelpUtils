#include "pch.h"
#include "SettingsPage.xaml.h"

#include "Helpers/GlobalOptions.h"
#include "Helpers/ThemeHelper.h"
#include "Helpers/UIHelper.h"
#include "Helpers/WindowHelper.h"
#include "Models/M128A.h"
#include "Utility/logger.h"

#include "DbgHelpUtils/file_version_info.h"
#include "DbgHelpUtils/stream_hex_dump.h"
#include "DbgHelpUtils/system_info_utils.h"
#include "DbgHelpUtils/windows_error.h"

#include <winrt/Microsoft.Windows.ApplicationModel.Resources.h>

#include "DbgHelpUtils/m128a_utils.h"

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
        : GlobalOptionsNotifyPropertyChangedBase(
            {
                L"SuiteMask",
                L"ExampleNumber"
            },
            {
                L"ExampleSize"
            }, 
            {
                L"ExampleDate"
            },
            {
                L"ExampleDuration"
            },
            {
                L"ExampleM128A"
            },
            {
                L"ExampleDouble"
            })
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
        timeStampLocale().SelectedIndex(static_cast<int>(options.TimeStampLocale()));
        dateFormat().SelectedIndex(ToDateFormatIndex(options.DateFormatFlags()));
        dateLayout().SelectedIndex(ToDateLayoutIndex(options.DateFormatFlags()));
        useAltCalendar().IsOn((options.DateFormatFlags() & DATE_USE_ALT_CALENDAR) == DATE_USE_ALT_CALENDAR);
        time24Hour().IsOn((options.TimeFormatFlags() & TIME_FORCE24HOURFORMAT) == TIME_FORCE24HOURFORMAT);
        noTimeMarker().IsOn((options.TimeFormatFlags() & TIME_NOTIMEMARKER) == TIME_NOTIMEMARKER);
        noTimeSeconds().IsOn((options.TimeFormatFlags() & TIME_NOSECONDS) == TIME_NOSECONDS);
        noTimeMinutesOrSeconds().IsOn((options.TimeFormatFlags() & TIME_NOMINUTESORSECONDS) == TIME_NOMINUTESORSECONDS);
        durationFormat().SelectedIndex(static_cast<int>(options.DurationFormat()));
        m128aUnitFormatMode().SelectedIndex(static_cast<int>(options.M128AViewDisplayFormat()));

        SetExampleM128ANumber();
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

    void SettingsPage::SetExampleM128ANumber() const
    {
        auto m128aExample = exampleM128A_.as<M128A>();
        m128aExample->Set(MakeExampleM128A());
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

    void SettingsPage::OnTimeStampFormatChanged()
    {
        auto const& options = GlobalOptions::Options();
        timeStampLocale().SelectedIndex(static_cast<int>(options.TimeStampLocale()));
        dateFormat().SelectedIndex(ToDateFormatIndex(options.DateFormatFlags()));
        dateLayout().SelectedIndex(ToDateLayoutIndex(options.DateFormatFlags()));
        useAltCalendar().IsOn((options.DateFormatFlags() & DATE_USE_ALT_CALENDAR) == DATE_USE_ALT_CALENDAR);
        time24Hour().IsOn((options.TimeFormatFlags() & TIME_FORCE24HOURFORMAT) == TIME_FORCE24HOURFORMAT);
        noTimeMarker().IsOn((options.TimeFormatFlags() & TIME_NOTIMEMARKER) == TIME_NOTIMEMARKER);
        noTimeSeconds().IsOn((options.TimeFormatFlags() & TIME_NOSECONDS) == TIME_NOSECONDS);
        noTimeMinutesOrSeconds().IsOn((options.TimeFormatFlags() & TIME_NOMINUTESORSECONDS) == TIME_NOMINUTESORSECONDS);
    }

    void SettingsPage::OnDurationFormatChanged()
    {
        auto const& options = GlobalOptions::Options();
        durationFormat().SelectedIndex(static_cast<int>(options.DurationFormat()));
    }

    void SettingsPage::OnM128AViewDisplayFormatChanged()
    {
        auto const& options = GlobalOptions::Options();
        m128aUnitFormatMode().SelectedIndex(static_cast<int>(options.M128AViewDisplayFormat()));

        SetExampleM128ANumber();
    }

    void SettingsPage::OnFloatingPointScientificDisplayFormatChanged()
    {
        RaisePropertyChanged(L"FloatingPointScientificDisplayFormat");
    }

    _M128A SettingsPage::MakeExampleM128A()
    {
        switch (GlobalOptions::Options().M128AViewDisplayFormat())
        {
        case M128AViewType::Int128:
        case M128AViewType::UInt128:
        case M128AViewType::Int64:
        case M128AViewType::UInt64:
        case M128AViewType::Int32:
        case M128AViewType::UInt32:
        case M128AViewType::Int16:
        case M128AViewType::UInt16:
        case M128AViewType::Int8:
        case M128AViewType::UInt8:
        default:
            return {.Low= 9876543210987654321ULL, .High= -1234567890123456789LL};
        case M128AViewType::Float32:
            return m128a_utils::from_floats(234.5678f, -234.5678f, 987.654f, -987.654f);
        case M128AViewType::Float64:
            return m128a_utils::from_doubles(234.5678, -234.5678);
        }
    }

    int SettingsPage::ToDateFormatIndex(uint32_t const dateFormatFlags)
    {
        if ((dateFormatFlags & DATE_LONGDATE) == DATE_LONGDATE)
        {
            return 0;
        }

        if ((dateFormatFlags & DATE_SHORTDATE) == DATE_SHORTDATE)
        {
            return 1;
        }

        if ((dateFormatFlags & DATE_YEARMONTH) == DATE_YEARMONTH)
        {
            return 2;
        }

        if ((dateFormatFlags & DATE_MONTHDAY) == DATE_MONTHDAY)
        {
            return 3;
        }

        return 1;
    }

    int SettingsPage::ToDateLayoutIndex(uint32_t const dateFormatFlags)
    {
        
        if ((dateFormatFlags & DATE_AUTOLAYOUT) == DATE_AUTOLAYOUT)
        {
            return 0;
        }

        if ((dateFormatFlags & DATE_LTRREADING) == DATE_LTRREADING)
        {
            return 1;
        }

        if ((dateFormatFlags & DATE_RTLREADING) == DATE_RTLREADING)
        {
            return 2;
        }

        return 0;
    }

    uint64_t SettingsPage::ExampleNumber()
    {
        return 1234567890123456789ULL;
    }

    uint64_t SettingsPage::ExampleSize()
    {
        return 1234567890123456789ULL;
    }

    uint64_t SettingsPage::ExampleDate()
    {
        static const auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        return now;
    }

    MiniDumpExplorer::M128A SettingsPage::ExampleM128A()
    {
        return exampleM128A_;
    }

    double SettingsPage::ExampleDouble()
    {
        return 123.456e-2;
    }

    bool SettingsPage::SymbolLoadDebug()
    {
        return GlobalOptions::Options().SymbolLoadDebug();
    }

    void SettingsPage::SymbolLoadDebug(bool const value)
    {
        GlobalOptions::Options().SymbolLoadDebug(value);
    }

    bool SettingsPage::SymbolLoadMemoryDebug()
    {
        return GlobalOptions::Options().SymbolLoadDebugMemory();
    }

    void SettingsPage::SymbolLoadMemoryDebug(bool const value)
    {
        GlobalOptions::Options().SymbolLoadDebugMemory(value);
    }

    bool SettingsPage::FloatingPointScientificDisplayFormat()
    {
        return GlobalOptions::Options().FloatingPointScientificDisplayFormat();
    }

    void SettingsPage::FloatingPointScientificDisplayFormat(bool const value)
    {
        GlobalOptions::Options().FloatingPointScientificDisplayFormat(value);
    }

    uint64_t SettingsPage::ExampleDuration()
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
        const auto new_number_format = static_cast<NumberDisplayFormatType>(numberFormatMode().SelectedIndex());
        GlobalOptions::Options().NumberDisplayFormat(new_number_format);
    }

    void SettingsPage::UnitSizeFormatModeSelectionChanged([[maybe_unused]] Windows::Foundation::IInspectable const& sender, [[maybe_unused]] RoutedEventArgs const& e)
    {
        const auto value = static_cast<SizeNumberDisplayFormatType>(unitSizeFormatMode().SelectedIndex());
        GlobalOptions::Options().SizeNumberDisplayFormat(value);
    }

    void SettingsPage::UnitFormatModeSelectionChanged([[maybe_unused]] Windows::Foundation::IInspectable const& sender, [[maybe_unused]] RoutedEventArgs const& e)
    {
        const auto value = static_cast<size_units::print>(unitFormatMode().SelectedIndex());
        GlobalOptions::Options().SizeFormat(value);
    }

    void SettingsPage::UnitBaseModeSelectionChanged([[maybe_unused]] Windows::Foundation::IInspectable const& sender, [[maybe_unused]] RoutedEventArgs const& e)
    {
        const auto value = static_cast<SizeDisplayNumberBaseType>(unitBaseMode().SelectedIndex());
        GlobalOptions::Options().SizeBase(value);
    }

    void SettingsPage::TimeStampLocaleChanged([[maybe_unused]] Windows::Foundation::IInspectable const& sender, [[maybe_unused]] RoutedEventArgs const& e)
    {
        const auto value = static_cast<TimeStampLocaleType>(timeStampLocale().SelectedIndex());
        GlobalOptions::Options().TimeStampLocale(value);
    }

    void SettingsPage::DateFormatChanged([[maybe_unused]] Windows::Foundation::IInspectable const& sender, [[maybe_unused]] RoutedEventArgs const& e)
    {
        auto const dateFormatFlags = [this]()
        {
            switch (dateFormat().SelectedIndex())
            {
            case 0:
                return DATE_LONGDATE;
            case 1:
                return DATE_SHORTDATE;
            case 2:
                return DATE_YEARMONTH;
            case 3:
                return DATE_MONTHDAY;
            default:
                return 0;
            }
        }();

        GlobalOptions::Options().DateFormatFlags(GlobalOptions::Options().DateFormatFlags() & ~(DATE_LONGDATE | DATE_SHORTDATE | DATE_YEARMONTH | DATE_MONTHDAY) | dateFormatFlags);
    }

    void SettingsPage::DateLayoutChanged([[maybe_unused]] Windows::Foundation::IInspectable const& sender, [[maybe_unused]] RoutedEventArgs const& e)
    {
        auto const dateFormatFlags = [this]()
        {
            switch (dateLayout().SelectedIndex())
            {
            case 0:
                return DATE_AUTOLAYOUT;
            case 1:
                return DATE_LTRREADING;
            case 2:
                return DATE_RTLREADING;
            default:
                return 0;
            }
        }();

        GlobalOptions::Options().DateFormatFlags(GlobalOptions::Options().DateFormatFlags() & ~(DATE_AUTOLAYOUT | DATE_LTRREADING | DATE_RTLREADING) | dateFormatFlags);
    }

    void SettingsPage::UseAltCalendarToggled([[maybe_unused]] Windows::Foundation::IInspectable const& sender, [[maybe_unused]] RoutedEventArgs const& e)
    {
        if (auto const isChecked = useAltCalendar().IsOn();
            isChecked)
        {
            GlobalOptions::Options().DateFormatFlags(GlobalOptions::Options().DateFormatFlags() | DATE_USE_ALT_CALENDAR);
        }
        else
        {
            GlobalOptions::Options().DateFormatFlags(GlobalOptions::Options().DateFormatFlags() & ~DATE_USE_ALT_CALENDAR);
        }
    }

    void SettingsPage::Time24HourToggled([[maybe_unused]] Windows::Foundation::IInspectable const& sender, [[maybe_unused]] RoutedEventArgs const& e)
    {
        if (auto const isChecked = time24Hour().IsOn();
            isChecked)
        {
            GlobalOptions::Options().TimeFormatFlags(GlobalOptions::Options().TimeFormatFlags() | TIME_FORCE24HOURFORMAT);
        }
        else
        {
            GlobalOptions::Options().TimeFormatFlags(GlobalOptions::Options().TimeFormatFlags() & ~TIME_FORCE24HOURFORMAT);
        }
    }

    void SettingsPage::NoTimeMarkerToggled([[maybe_unused]] Windows::Foundation::IInspectable const& sender, [[maybe_unused]] RoutedEventArgs const& e)
    {
        if (auto const isChecked = noTimeMarker().IsOn();
            isChecked)
        {
            GlobalOptions::Options().TimeFormatFlags(GlobalOptions::Options().TimeFormatFlags() | TIME_NOTIMEMARKER);
        }
        else
        {
            GlobalOptions::Options().TimeFormatFlags(GlobalOptions::Options().TimeFormatFlags() & ~TIME_NOTIMEMARKER);
        }
    }

    void SettingsPage::NoTimeSecondsToggled([[maybe_unused]] Windows::Foundation::IInspectable const& sender, [[maybe_unused]] RoutedEventArgs const& e)
    {
        if (auto const isChecked = noTimeSeconds().IsOn();
            isChecked)
        {
            GlobalOptions::Options().TimeFormatFlags(GlobalOptions::Options().TimeFormatFlags() | TIME_NOSECONDS);
        }
        else
        {
            GlobalOptions::Options().TimeFormatFlags(GlobalOptions::Options().TimeFormatFlags() & ~TIME_NOSECONDS);
        }
    }

    void SettingsPage::NoTimeMinutesOrSecondsToggled([[maybe_unused]] Windows::Foundation::IInspectable const& sender, [[maybe_unused]] RoutedEventArgs const& e)
    {
        if (auto const isChecked = noTimeMinutesOrSeconds().IsOn();
            isChecked)
        {
            GlobalOptions::Options().TimeFormatFlags(GlobalOptions::Options().TimeFormatFlags() | TIME_NOMINUTESORSECONDS);
        }
        else
        {
            GlobalOptions::Options().TimeFormatFlags(GlobalOptions::Options().TimeFormatFlags() & ~TIME_NOMINUTESORSECONDS);
        }
    }

    void SettingsPage::DurationFormatChanged([[maybe_unused]] Windows::Foundation::IInspectable const& sender, [[maybe_unused]] RoutedEventArgs const& e)
    {
        const auto value = static_cast<DurationFormatType>(durationFormat().SelectedIndex());
        GlobalOptions::Options().DurationFormat(value);
    }

    void SettingsPage::M128aUnitFormatModeSelectionChanged([[maybe_unused]] Windows::Foundation::IInspectable const& sender, [[maybe_unused]] RoutedEventArgs const& e)
    {
        const auto value = static_cast<M128AViewType>(m128aUnitFormatMode().SelectedIndex());
        GlobalOptions::Options().M128AViewDisplayFormat(value);
    }

    void SettingsPage::SetupFlyoutMenus()
    {
        UIHelper::CreateStandardHexNumberMenu(exampleNumberDisplay(), suiteMaskTextBlock());
        UIHelper::CreateStandardSizeNumberMenu(exampleSizeDisplay());
        UIHelper::CreateStandardTimeStampMenu(exampleDateDisplay());
        UIHelper::CreateStandardDurationMenu(exampleDurationDisplay());
        UIHelper::CreateStandardFloatingPointMenu(exampleFloatingPointDisplay());
    }
}


