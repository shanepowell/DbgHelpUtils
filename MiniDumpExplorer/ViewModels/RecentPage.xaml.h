#pragma once

#include "RecentPage.g.h"
#include "Models/RecentFileItem.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct RecentFileItemsDataSource;

    struct RecentPage : RecentPageT<RecentPage>
    {
        RecentPage();

        // ReSharper disable once CppHidingFunction
        void InitializeComponent();

        [[nodiscard]] IDataGridDataSource ItemsSource() const noexcept { return recentFileItemsDataSource_; }
        void ClearAllRecentFiles(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::RoutedEventArgs const& e) const;
        void LoadSelectedFile(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::RoutedEventArgs const& e) const;

    private:
        fire_and_forget RecentPageLoad() const;
        fire_and_forget LoadCurrentlySelectedFile() const;
        void OnRowDoubleTapped(Windows::Foundation::IInspectable const& sender, DataGridRowDetailsEventArgs const& e) const;

    private:
        event_token onRecentPageLoadedEvent_;
        event_token onRowDoubleTapped_;
        MiniDumpExplorer::RecentFileItemsDataSource recentFileItemsDataSource_{};
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct RecentPage : RecentPageT<RecentPage, implementation::RecentPage>
    {
    };
}
