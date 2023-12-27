#pragma once

#include "MainWindow.g.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct MainWindow : MainWindowT<MainWindow>
    {
        MainWindow();

        fire_and_forget MenuFileOpen_Click(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::RoutedEventArgs const& args);
        fire_and_forget MenuFileOpen_Exit(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::RoutedEventArgs const& args) const;

        fire_and_forget TabView_AddTabButtonClick(Microsoft::UI::Xaml::Controls::TabView const& sender, Windows::Foundation::IInspectable const& args);
        void TabView_SelectionChanged(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::Controls::SelectionChangedEventArgs const& args);
        void TabView_TabCloseRequested(Microsoft::UI::Xaml::Controls::TabView const& sender, Microsoft::UI::Xaml::Controls::TabViewTabCloseRequestedEventArgs const& args);

        hstring SelectedTabTitle();

        event_token PropertyChanged(Microsoft::UI::Xaml::Data::PropertyChangedEventHandler const& handler);
        void PropertyChanged(event_token const& token) noexcept;

        void OpenFileInTab(Windows::Storage::StorageFile const& file);

    private:
        void OpenDefaultTab();
        Windows::Foundation::IAsyncAction OpenFileInNewTab();
        void RaisePropertyChanged(hstring const& propertyName);
        static Microsoft::UI::Xaml::Controls::TabViewItem CreateNewTab(Windows::Storage::StorageFile const& file);

    private:
        event<Microsoft::UI::Xaml::Data::PropertyChangedEventHandler> propertyChanged_;
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct MainWindow : MainWindowT<MainWindow, implementation::MainWindow>
    {
    };
}
