#pragma once

#include "ThreadListStreamPage.g.h"

#include "Models/MiniDumpPageBase.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct ThreadListStreamEntriesDataSource;

    struct ThreadListStreamPage : ThreadListStreamPageT<ThreadListStreamPage>, MiniDumpPageBase<ThreadListStreamPage>
    {
        ThreadListStreamPage();

        // ReSharper disable once CppHidingFunction
        void InitializeComponent();

        // ReSharper disable once CppHidingFunction
        void OnNavigatedTo(Microsoft::UI::Xaml::Navigation::NavigationEventArgs const& e)
        {
            MiniDumpPageBase::OnNavigatedTo(e);
        }

        [[nodiscard]] IDataGridDataSource ItemsSource() const noexcept { return threadsStreamEntriesDataSource_; }

        void SelectThread(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::RoutedEventArgs const& e) const;

    private:
        void OnRowDoubleTapped(Windows::Foundation::IInspectable const& sender, DataGridRowDetailsEventArgs const& e) const;
        void MiniDumpLoaded(MiniDumpExplorer::MiniDumpPageParameters const& parameters) override;

    private:
        event_token onRowDoubleTapped_;
        MiniDumpExplorer::ThreadListStreamEntriesDataSource threadsStreamEntriesDataSource_{};
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct ThreadListStreamPage : ThreadListStreamPageT<ThreadListStreamPage, implementation::ThreadListStreamPage>
    {
    };
}
