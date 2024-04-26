#pragma once

#include "ThreadInfoListStreamPage.g.h"

#include "Models/MiniDumpPageBase.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct ThreadInfoListStreamEntriesDataSource;

    struct ThreadInfoListStreamPage : ThreadInfoListStreamPageT<ThreadInfoListStreamPage>, MiniDumpPageBase<ThreadInfoListStreamPage>
    {
        ThreadInfoListStreamPage();

        // ReSharper disable once CppHidingFunction
        void InitializeComponent();

        // ReSharper disable once CppHidingFunction
        void OnNavigatedTo(Microsoft::UI::Xaml::Navigation::NavigationEventArgs const& e)
        {
            MiniDumpPageBase::OnNavigatedTo(e);
        }

        [[nodiscard]] IDataGridDataSource ItemsSource() const noexcept { return threadsInfoStreamEntriesDataSource_; }

        void SelectThread(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::RoutedEventArgs const& e) const;

    private:
        void OnRowDoubleTapped(Windows::Foundation::IInspectable const& sender, DataGridRowDetailsEventArgs const& e) const;
        void MiniDumpLoaded(MiniDumpExplorer::MiniDumpPageParameters const& parameters) override;

    private:
        event_token onRowDoubleTapped_;
        MiniDumpExplorer::ThreadInfoListStreamEntriesDataSource threadsInfoStreamEntriesDataSource_{};
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct ThreadInfoListStreamPage : ThreadInfoListStreamPageT<ThreadInfoListStreamPage, implementation::ThreadInfoListStreamPage>
    {
    };
}
