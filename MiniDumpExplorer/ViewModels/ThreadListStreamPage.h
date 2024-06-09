#pragma once

#include "ThreadListStreamPage.g.h"

#include "Models/NotifyPropertyChangedBase.h"
#include "Models/MiniDumpPageBase.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct ThreadListStreamEntriesDataSource;

    struct ThreadListStreamPage : ThreadListStreamPageT<ThreadListStreamPage>, MiniDumpPageBase<ThreadListStreamPage>, NotifyPropertyChangedBase<ThreadListStreamPage>
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

        uint32_t Index() const { return index_; }
        uint32_t TotalEntries() const { return total_entries_; }

        void SelectThread(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::RoutedEventArgs const& e) const;

        static void CreateNavigationViewItemEntriesForStream(Microsoft::UI::Xaml::Controls::NavigationViewItem const& item, MiniDumpExplorer::MiniDumpPageParameters const& parameters, dlg_help_utils::mini_dump const& mini_dump);

    private:
        void OnRowDoubleTapped(Windows::Foundation::IInspectable const& sender, DataGridRowDetailsEventArgs const& e) const;
        void MiniDumpLoaded(MiniDumpExplorer::MiniDumpPageParameters const& parameters) override;
        void SelectCurrentlySelectedEntry() const;

        static std::wstring CreateFindNavigationTag(uint32_t index, uint32_t sub_index);

    private:
        event_token onRowDoubleTapped_;
        uint32_t index_{};
        uint32_t total_entries_{};
        MiniDumpExplorer::ThreadListStreamEntriesDataSource threadsStreamEntriesDataSource_{};
        MiniDumpExplorer::MiniDumpPage miniDumpPage_{nullptr};
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct ThreadListStreamPage : ThreadListStreamPageT<ThreadListStreamPage, implementation::ThreadListStreamPage>
    {
    };
}
