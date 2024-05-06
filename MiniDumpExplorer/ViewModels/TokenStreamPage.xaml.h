#pragma once

#include "TokenStreamPage.g.h"

#include "Models/NotifyPropertyChangedBase.h"
#include "Models/MiniDumpPageBase.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct TokenStreamEntriesDataSource;

    struct TokenStreamPage : TokenStreamPageT<TokenStreamPage>, MiniDumpPageBase<TokenStreamPage>, NotifyPropertyChangedBase<TokenStreamPage>
    {
        TokenStreamPage();

        // ReSharper disable once CppHidingFunction
        void InitializeComponent();

        // ReSharper disable once CppHidingFunction
        void OnNavigatedTo(Microsoft::UI::Xaml::Navigation::NavigationEventArgs const& e)
        {
            MiniDumpPageBase::OnNavigatedTo(e);
        }

        [[nodiscard]] IDataGridDataSource ItemsSource() const noexcept { return tokenEntriesDataSource_; }

        uint32_t Index() const { return index_; }
        uint64_t TotalEntries() const { return total_entries_; }

        void SelectToken(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::RoutedEventArgs const& e) const;

    private:
        void OnRowDoubleTapped(Windows::Foundation::IInspectable const& sender, DataGridRowDetailsEventArgs const& e) const;
        void MiniDumpLoaded(MiniDumpExplorer::MiniDumpPageParameters const& parameters) override;

    private:
        event_token onRowDoubleTapped_;
        uint32_t index_{};
        uint64_t total_entries_{};
        MiniDumpExplorer::TokenStreamEntriesDataSource tokenEntriesDataSource_{};
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct TokenStreamPage : TokenStreamPageT<TokenStreamPage, implementation::TokenStreamPage>
    {
    };
}
