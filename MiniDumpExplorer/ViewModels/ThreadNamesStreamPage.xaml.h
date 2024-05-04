#pragma once

#include "ThreadNamesStreamPage.g.h"

#include "Models/NotifyPropertyChangedBase.h"
#include "Models/MiniDumpPageBase.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct ThreadNamesStreamEntriesDataSource;

    struct ThreadNamesStreamPage : ThreadNamesStreamPageT<ThreadNamesStreamPage>, MiniDumpPageBase<ThreadNamesStreamPage>, NotifyPropertyChangedBase<ThreadNamesStreamPage>
    {
        ThreadNamesStreamPage();

        // ReSharper disable once CppHidingFunction
        void OnNavigatedTo(Microsoft::UI::Xaml::Navigation::NavigationEventArgs const& e)
        {
            MiniDumpPageBase::OnNavigatedTo(e);
        }

        [[nodiscard]] IDataGridDataSource ItemsSource() const noexcept { return threadsStreamEntriesDataSource_; }

        uint32_t Index() const { return index_; }
        uint32_t TotalEntries() const { return total_entries_; }

    private:
        void MiniDumpLoaded(MiniDumpExplorer::MiniDumpPageParameters const& parameters) override;

    private:
        uint32_t index_{};
        uint32_t total_entries_{};
        MiniDumpExplorer::ThreadNamesStreamEntriesDataSource threadsStreamEntriesDataSource_{};
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct ThreadNamesStreamPage : ThreadNamesStreamPageT<ThreadNamesStreamPage, implementation::ThreadNamesStreamPage>
    {
    };
}
