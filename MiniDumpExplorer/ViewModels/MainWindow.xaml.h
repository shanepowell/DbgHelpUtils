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

        fire_and_forget TitleBarIcon_PointerPressed(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::Input::PointerRoutedEventArgs const& args);
        void TitleBarIcon_MouseDoubleClick(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::RoutedEventArgs const& args) const;

    private:
        void MainWindow_Activated(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::WindowActivatedEventArgs const& args);
        void AppWindow_Changed(Microsoft::UI::Windowing::AppWindow const& sender, Microsoft::UI::Windowing::AppWindowChangedEventArgs const& args);
        void AppTitleBar_OnLoaded(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::RoutedEventArgs const& args);
        void AppTitleBar_OnSizeChanged(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::SizeChangedEventArgs const& args);

        void OpenDefaultTab();
        Windows::Foundation::IAsyncAction OpenFileInNewTab();

        void SetRegionsForCustomTitleBar();
        void ShowSystemMenu() const;

        static Windows::Foundation::IAsyncOperation<Microsoft::UI::Xaml::Controls::TabViewItem> CreateNewTab(Windows::Storage::StorageFile file);

    private:
        event_token activatedEvent_{};
        event_token appWindowChangedEvent_{};
        event_token loadedEvent_{};
        event_token sizeChangedEvent_{};
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct MainWindow : MainWindowT<MainWindow, implementation::MainWindow>
    {
    };
}
