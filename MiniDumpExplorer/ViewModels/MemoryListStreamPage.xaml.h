#pragma once

#include "MemoryListStreamPage.g.h"

#include "Models/MiniDumpPageBase.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct MemoryListStreamPage : MemoryListStreamPageT<MemoryListStreamPage>, MiniDumpPageBase<MemoryListStreamPage>
    {
        MemoryListStreamPage();

        // ReSharper disable once CppHidingFunction
        void InitializeComponent();

        // ReSharper disable once CppHidingFunction
        void OnNavigatedTo(Microsoft::UI::Xaml::Navigation::NavigationEventArgs const& e)
        {
            MiniDumpPageBase::OnNavigatedTo(e);
        }

        [[nodiscard]] IDataGridDataSource ItemsSource() const noexcept { return memoryStreamEntriesDataSource_; }

        void SelectMemoryRange(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::RoutedEventArgs const& e) const;

    private:
        void OnRowDoubleTapped(Windows::Foundation::IInspectable const& sender, DataGridRowDetailsEventArgs const& e) const;
        void MiniDumpLoaded(MiniDumpExplorer::MiniDumpPageParameters const& parameters) override;

    private:
        event_token onRowDoubleTapped_;
        MiniDumpExplorer::MemoryListStreamEntriesDataSource memoryStreamEntriesDataSource_{};
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct MemoryListStreamPage : MemoryListStreamPageT<MemoryListStreamPage, implementation::MemoryListStreamPage>
    {
    };
}
