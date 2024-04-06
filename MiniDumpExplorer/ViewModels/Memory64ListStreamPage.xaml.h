#pragma once

#include "Memory64ListStreamPage.g.h"

#include "Models/MiniDumpPageBase.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct Memory64ListStreamEntriesDataSource;

    struct Memory64ListStreamPage : Memory64ListStreamPageT<Memory64ListStreamPage>, MiniDumpPageBase<Memory64ListStreamPage>
    {
        Memory64ListStreamPage();

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
        MiniDumpExplorer::Memory64ListStreamEntriesDataSource memoryStreamEntriesDataSource_{};
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct Memory64ListStreamPage : Memory64ListStreamPageT<Memory64ListStreamPage, implementation::Memory64ListStreamPage>
    {
    };
}
