#pragma once

#include "StreamsPage.g.h"

namespace dlg_help_utils
{
    class mini_dump;
}

namespace winrt::MiniDumpExplorer::implementation
{
    struct DirectoryStreamEntriesDataSource;
    struct MiniDumpPage;

    struct StreamsPage : StreamsPageT<StreamsPage>
    {
        StreamsPage();

        void OnNavigatedTo(Microsoft::UI::Xaml::Navigation::NavigationEventArgs const& e);
        [[nodiscard]] IDataGridDataSource ItemsSource() const noexcept { return directoryStreamEntriesDataSource_; }

        void SelectStream(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::RoutedEventArgs const& e) const;

    private:
        void OnRowDoubleTapped(Windows::Foundation::IInspectable const& sender, DataGridRowDetailsEventArgs const& e) const;

        void MiniDumpLoaded(dlg_help_utils::mini_dump const& miniDump);

    private:
        event_token miniDumpLoadedEvent_;
        dlg_help_utils::mini_dump const* mini_dump_{nullptr};
        MiniDumpExplorer::DirectoryStreamEntriesDataSource directoryStreamEntriesDataSource_{};
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct StreamsPage : StreamsPageT<StreamsPage, implementation::StreamsPage>
    {
    };
}
