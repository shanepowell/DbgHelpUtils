#pragma once

#include "SettingsPage.g.h"

#include "Models/GlobalOptionsNotifyPropertyChangedBase.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct M128A;

    struct SettingsPage : SettingsPageT<SettingsPage>, GlobalOptionsNotifyPropertyChangedBase<SettingsPage>
    {
        SettingsPage();

        // ReSharper disable once CppHidingFunction
        void InitializeComponent();

        hstring Version() const { return version_; }
        hstring OsVersion() const { return osVersion_; }
        uint16_t SuiteMask() const { return suiteMask_; }
        hstring ProductType() const { return productType_; }
        hstring PlatformId() const { return platformId_; }
        hstring CsdVersion() const { return csdVersion_; }
        hstring ProcessorArchitecture() const { return processorArchitecture_; }
        Windows::Foundation::Collections::IObservableVector<hstring> SuiteMaskList() const { return suiteMaskList_; }
        static uint64_t ExampleNumber();
        static uint64_t ExampleSize();
        static uint64_t ExampleDuration();
        static uint64_t ExampleDate();
        MiniDumpExplorer::M128A ExampleM128A();
        static double ExampleDouble();

        static bool SymbolLoadDebug();
        static void SymbolLoadDebug(bool value);
        static bool SymbolLoadMemoryDebug();
        static void SymbolLoadMemoryDebug(bool value);
        static bool FloatingPointScientificDisplayFormat();
        static void FloatingPointScientificDisplayFormat(bool value);

        void ThemeModeSelectionChanged(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void LogLevelModeSelectionChanged(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void NumberFormatModeSelectionChanged(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void UnitSizeFormatModeSelectionChanged(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void UnitFormatModeSelectionChanged(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void UnitBaseModeSelectionChanged(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void TimeStampLocaleChanged(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void DateFormatChanged(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void DateLayoutChanged(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void UseAltCalendarToggled(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void Time24HourToggled(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void NoTimeMarkerToggled(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void NoTimeSecondsToggled(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void NoTimeMinutesOrSecondsToggled(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void DurationFormatChanged(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void M128aUnitFormatModeSelectionChanged(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::RoutedEventArgs const& e);

    private:
        void SetupFlyoutMenus();
        void OnSettingsPageLoaded(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void LoadVersionInformation();
        void SetExampleM128ANumber() const;

        void OnNumberDisplayFormatChanged() override;
        void OnSizeNumberDisplayFormatChanged() override;
        void OnTimeStampFormatChanged() override;
        void OnDurationFormatChanged() override;
        void OnM128AViewDisplayFormatChanged() override;
        void OnFloatingPointScientificDisplayFormatChanged() override;
        
        static _M128A MakeExampleM128A();
        static int ToDateFormatIndex(uint32_t dateFormatFlags);
        static int ToDateLayoutIndex(uint32_t dateFormatFlags);

    private:
        event_token onSettingsPageLoadedEvent_;
        hstring version_{};
        hstring osVersion_{};
        uint16_t suiteMask_{};
        Windows::Foundation::Collections::IObservableVector<hstring> suiteMaskList_{single_threaded_observable_vector<hstring>()};
        hstring productType_{};
        hstring platformId_{};
        hstring csdVersion_{};
        hstring processorArchitecture_{};
        MiniDumpExplorer::M128A exampleM128A_{};
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct SettingsPage : SettingsPageT<SettingsPage, implementation::SettingsPage>
    {
    };
}
