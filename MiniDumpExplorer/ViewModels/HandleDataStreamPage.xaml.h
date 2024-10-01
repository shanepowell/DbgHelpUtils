#pragma once

#include "HandleDataStreamPage.g.h"

#include "Models/NotifyPropertyChangedBase.h"
#include "Models/MiniDumpPageBase.h"
#include "DbgHelpUtils/handle_data_stream.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct HandleDataStreamEntriesDataSource;

    struct HandleDataStreamPage : HandleDataStreamPageT<HandleDataStreamPage>, MiniDumpPageBase<HandleDataStreamPage>, NotifyPropertyChangedBase<HandleDataStreamPage>
    {
        HandleDataStreamPage();

        // ReSharper disable once CppHidingFunction
        void InitializeComponent();

        // ReSharper disable once CppHidingFunction
        void OnNavigatedTo(Microsoft::UI::Xaml::Navigation::NavigationEventArgs const& e)
        {
            MiniDumpPageBase::OnNavigatedTo(e);
        }

        [[nodiscard]] IDataGridDataSource ItemsSource() const noexcept { return handleDataStreamEntriesDataSource_; }

        uint32_t Index() const { return index_; }
        uint32_t HandleDescriptorVersion() const { return handle_data_.handle_descriptor_version(); }
        uint32_t TotalHandles() const { return handle_data_.size(); }

        void SelectHandleType(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::RoutedEventArgs const& e) const;

    private:
        void OnRowDoubleTapped(Windows::Foundation::IInspectable const& sender, DataGridRowDetailsEventArgs const& e) const;
        void MiniDumpLoaded(MiniDumpExplorer::MiniDumpPageParameters const& parameters) override;

    private:
        uint32_t index_{};
        dlg_help_utils::handle_data_stream handle_data_{};
        event_token onRowDoubleTapped_{};
        MiniDumpExplorer::HandleDataStreamEntriesDataSource handleDataStreamEntriesDataSource_{};
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct HandleDataStreamPage : HandleDataStreamPageT<HandleDataStreamPage, implementation::HandleDataStreamPage>
    {
    };
}
