#pragma once

#include "StreamsPage.g.h"

#include "Models/MiniDumpPageBase.h"

namespace dlg_help_utils
{
    class mini_dump;
}

namespace winrt::MiniDumpExplorer::implementation
{
    struct DirectoryStreamEntriesDataSource;
    struct MiniDumpPage;

    struct StreamsPage : StreamsPageT<StreamsPage>, MiniDumpPageBase<StreamsPage>
    {
        StreamsPage();

        // ReSharper disable once CppHidingFunction
        void InitializeComponent();

        // ReSharper disable once CppHidingFunction
        void OnNavigatedTo(Microsoft::UI::Xaml::Navigation::NavigationEventArgs const& e)
        {
            MiniDumpPageBase::OnNavigatedTo(e);
        }

        [[nodiscard]] IDataGridDataSource ItemsSource() const noexcept { return directoryStreamEntriesDataSource_; }

        void SelectStream(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::RoutedEventArgs const& e) const;

    private:
        void OnRowDoubleTapped(Windows::Foundation::IInspectable const& sender, DataGridRowDetailsEventArgs const& e) const;
        void MiniDumpLoaded(MiniDumpExplorer::MiniDumpPageParameters const& parameters) override;
        void SelectCurrentlySelectedStream() const;

    private:
        event_token onRowDoubleTapped_;
        MiniDumpExplorer::DirectoryStreamEntriesDataSource directoryStreamEntriesDataSource_{};
        MiniDumpExplorer::MiniDumpPage miniDumpPage_{nullptr};
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct StreamsPage : StreamsPageT<StreamsPage, implementation::StreamsPage>
    {
    };
}
