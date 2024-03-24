#pragma once

#include "SettingsPage.g.h"

#include "Models/GlobalOptionsNotifyPropertyChangedBase.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct SettingsPage : SettingsPageT<SettingsPage>, GlobalOptionsNotifyPropertyChangedBase<SettingsPage>
    {
        SettingsPage();

        hstring Version() const { return version_; }
        hstring OsVersion() const { return osVersion_; }
        uint16_t SuiteMask() const { return suiteMask_; }
        hstring ProductType() const { return productType_; }
        hstring PlatformId() const { return platformId_; }
        hstring CsdVersion() const { return csdVersion_; }
        hstring ProcessorArchitecture() const { return processorArchitecture_; }
        Windows::Foundation::Collections::IObservableVector<hstring> SuiteMaskList() const { return suiteMaskList_; }
        static uint64_t ExampleSize();

        void ThemeModeSelectionChanged(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void LogLevelModeSelectionChanged(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void NumberFormatModeSelectionChanged(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void UnitSizeFormatModeSelectionChanged(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void UnitFormatModeSelectionChanged(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void UnitBaseModeSelectionChanged(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::RoutedEventArgs const& e);

    private:
        void SetupFlyoutMenus();
        void OnSettingsPageLoaded(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void LoadVersionInformation();

        void OnNumberDisplayFormatChanged() override;
        void OnSizeNumberDisplayFormatChanged() override;

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
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct SettingsPage : SettingsPageT<SettingsPage, implementation::SettingsPage>
    {
    };
}
