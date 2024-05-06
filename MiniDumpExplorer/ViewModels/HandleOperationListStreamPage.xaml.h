#pragma once

#include "HandleOperationListStreamPage.g.h"

#include "Models/NotifyPropertyChangedBase.h"
#include "Models/MiniDumpPageBase.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct HandleOperationListStreamEntriesDataSource;

    struct HandleOperationListStreamPage : HandleOperationListStreamPageT<HandleOperationListStreamPage>, MiniDumpPageBase<HandleOperationListStreamPage>, NotifyPropertyChangedBase<HandleOperationListStreamPage>
    {
        HandleOperationListStreamPage();

        // ReSharper disable once CppHidingFunction
        void InitializeComponent();

        // ReSharper disable once CppHidingFunction
        void OnNavigatedTo(Microsoft::UI::Xaml::Navigation::NavigationEventArgs const& e)
        {
            MiniDumpPageBase::OnNavigatedTo(e);
        }

        [[nodiscard]] IDataGridDataSource ItemsSource() const noexcept { return handleOperationEntriesDataSource_; }

        uint32_t Index() const { return index_; }
        uint32_t TotalEntries() const { return total_entries_; }

        void SelectHandleOperation(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::RoutedEventArgs const& e) const;

    private:
        void OnRowDoubleTapped(Windows::Foundation::IInspectable const& sender, DataGridRowDetailsEventArgs const& e) const;
        void MiniDumpLoaded(MiniDumpExplorer::MiniDumpPageParameters const& parameters) override;

    private:
        event_token onRowDoubleTapped_;
        uint32_t index_{};
        uint32_t total_entries_{};
        MiniDumpExplorer::HandleOperationListStreamEntriesDataSource handleOperationEntriesDataSource_{};
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct HandleOperationListStreamPage : HandleOperationListStreamPageT<HandleOperationListStreamPage, implementation::HandleOperationListStreamPage>
    {
    };
}
