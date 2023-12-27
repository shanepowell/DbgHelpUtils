#pragma once

#include "SettingsPage.g.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct SettingsPage : SettingsPageT<SettingsPage>
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

        void ThemeModeSelectionChanged(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void LogLevelModeSelectionChanged(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void NumberFormatModeSelectionChanged(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void UnitSizeFormatModeSelectionChanged(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void UnitFormatModeSelectionChanged(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void UnitBaseModeSelectionChanged(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::RoutedEventArgs const& e);

        event_token PropertyChanged(Microsoft::UI::Xaml::Data::PropertyChangedEventHandler const& handler);
        void PropertyChanged(event_token const& token) noexcept;

        static bool DisplayHexadecimalNumericFormat();
        static void DisplayHexadecimalNumericFormat(bool value);

        void CopySuiteMaskToClipboard(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::RoutedEventArgs const& e);

    private:
        void OnSettingsPageLoaded(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::RoutedEventArgs const& e);
        void LoadVersionInformation();
        void RaisePropertyChanged(hstring const& propertyName);
        void OnNumberDisplayFormatChanged();
        void OnSizeNumberDisplayFormatChanged();

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
        event<Microsoft::UI::Xaml::Data::PropertyChangedEventHandler> propertyChanged_;
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct SettingsPage : SettingsPageT<SettingsPage, implementation::SettingsPage>
    {
    };
}
