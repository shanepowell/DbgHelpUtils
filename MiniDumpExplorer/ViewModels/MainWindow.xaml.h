#pragma once

#include "MainWindow.g.h"

#include "Models/NotifyPropertyChangedBase.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct MainWindow : MainWindowT<MainWindow>, NotifyPropertyChangedBase<MainWindow>
    {
        MainWindow();

        // ReSharper disable once CppHidingFunction
        void InitializeComponent();

        fire_and_forget MenuFileOpen_Click(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::RoutedEventArgs const& args);
        fire_and_forget MenuFileOpen_Exit(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::RoutedEventArgs const& args) const;

        fire_and_forget TabView_AddTabButtonClick(Microsoft::UI::Xaml::Controls::TabView const& sender, Windows::Foundation::IInspectable const& args);
        void TabView_SelectionChanged(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::Controls::SelectionChangedEventArgs const& args);
        void TabView_TabCloseRequested(Microsoft::UI::Xaml::Controls::TabView const& sender, Microsoft::UI::Xaml::Controls::TabViewTabCloseRequestedEventArgs const& args);

        hstring SelectedTabTitle();

        Windows::Foundation::IAsyncAction OpenFileInTab(Windows::Storage::StorageFile const& file);

    private:
        void OpenDefaultTab();
        Windows::Foundation::IAsyncAction OpenFileInNewTab();
        static Windows::Foundation::IAsyncOperation<Microsoft::UI::Xaml::Controls::TabViewItem> CreateNewTab(Windows::Storage::StorageFile const& file);
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct MainWindow : MainWindowT<MainWindow, implementation::MainWindow>
    {
    };
}
