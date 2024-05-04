#pragma once

#include "MemoryInfoListStreamPage.g.h"

#include "Models/NotifyPropertyChangedBase.h"
#include "Models/MiniDumpPageBase.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct MemoryInfoListStreamEntriesDataSource;

    struct MemoryInfoListStreamPage : MemoryInfoListStreamPageT<MemoryInfoListStreamPage>, MiniDumpPageBase<MemoryInfoListStreamPage>, NotifyPropertyChangedBase<MemoryInfoListStreamPage>
    {
        MemoryInfoListStreamPage();

        // ReSharper disable once CppHidingFunction
        void InitializeComponent();

        // ReSharper disable once CppHidingFunction
        void OnNavigatedTo(Microsoft::UI::Xaml::Navigation::NavigationEventArgs const& e)
        {
            MiniDumpPageBase::OnNavigatedTo(e);
        }

        [[nodiscard]] IDataGridDataSource ItemsSource() const noexcept { return memoryInfoStreamEntriesDataSource_; }

        uint32_t Index() const { return index_; }
        uint64_t TotalEntries() const { return total_entries_; }

        void SelectMemoryRange(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::RoutedEventArgs const& e) const;

    private:
        void OnRowDoubleTapped(Windows::Foundation::IInspectable const& sender, DataGridRowDetailsEventArgs const& e) const;
        void MiniDumpLoaded(MiniDumpExplorer::MiniDumpPageParameters const& parameters) override;

    private:
        event_token onRowDoubleTapped_;
        uint32_t index_{};
        uint64_t total_entries_{};
        MiniDumpExplorer::MemoryInfoListStreamEntriesDataSource memoryInfoStreamEntriesDataSource_{};
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct MemoryInfoListStreamPage : MemoryInfoListStreamPageT<MemoryInfoListStreamPage, implementation::MemoryInfoListStreamPage>
    {
    };
}
