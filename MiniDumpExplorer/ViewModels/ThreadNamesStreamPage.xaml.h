#pragma once

#include "ThreadNamesStreamPage.g.h"

#include "Models/MiniDumpPageBase.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct ThreadNamesStreamEntriesDataSource;

    struct ThreadNamesStreamPage : ThreadNamesStreamPageT<ThreadNamesStreamPage>, MiniDumpPageBase<ThreadNamesStreamPage>
    {
        ThreadNamesStreamPage();

        // ReSharper disable once CppHidingFunction
        void OnNavigatedTo(Microsoft::UI::Xaml::Navigation::NavigationEventArgs const& e)
        {
            MiniDumpPageBase::OnNavigatedTo(e);
        }

        [[nodiscard]] IDataGridDataSource ItemsSource() const noexcept { return threadsStreamEntriesDataSource_; }

    private:
        void MiniDumpLoaded(MiniDumpExplorer::MiniDumpPageParameters const& parameters) override;

    private:
        MiniDumpExplorer::ThreadNamesStreamEntriesDataSource threadsStreamEntriesDataSource_{};
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct ThreadNamesStreamPage : ThreadNamesStreamPageT<ThreadNamesStreamPage, implementation::ThreadNamesStreamPage>
    {
    };
}
