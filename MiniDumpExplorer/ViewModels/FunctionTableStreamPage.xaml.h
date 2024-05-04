#pragma once

#include "FunctionTableStreamPage.g.h"

#include "Models/NotifyPropertyChangedBase.h"
#include "Models/MiniDumpPageBase.h"
#include "DbgHelpUtils/function_table_stream.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct FunctionTableStreamEntriesDataSource;

    struct FunctionTableStreamPage : FunctionTableStreamPageT<FunctionTableStreamPage>, MiniDumpPageBase<FunctionTableStreamPage>, NotifyPropertyChangedBase<FunctionTableStreamPage>
    {
        FunctionTableStreamPage();

        // ReSharper disable once CppHidingFunction
        void InitializeComponent();

        // ReSharper disable once CppHidingFunction
        void OnNavigatedTo(Microsoft::UI::Xaml::Navigation::NavigationEventArgs const& e)
        {
            MiniDumpPageBase::OnNavigatedTo(e);
        }

        [[nodiscard]] IDataGridDataSource ItemsSource() const noexcept { return functionTableStreamEntriesDataSource_; }

        uint32_t Index() const { return static_cast<uint32_t>(function_table_.index()); }
        uint32_t TotalFunctions() const { return function_table_.size(); }
        hstring EntryType() const { return entry_type_; }

        void SelectFunction(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::RoutedEventArgs const& e) const;

    private:
        void OnRowDoubleTapped(Windows::Foundation::IInspectable const& sender, DataGridRowDetailsEventArgs const& e) const;
        void MiniDumpLoaded(MiniDumpExplorer::MiniDumpPageParameters const& parameters) override;

    private:
        dlg_help_utils::function_table_stream function_table_{};
        hstring entry_type_;
        event_token onRowDoubleTapped_;
        MiniDumpExplorer::FunctionTableStreamEntriesDataSource functionTableStreamEntriesDataSource_{};
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct FunctionTableStreamPage : FunctionTableStreamPageT<FunctionTableStreamPage, implementation::FunctionTableStreamPage>
    {
    };
}
