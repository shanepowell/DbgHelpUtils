#pragma once

#include "ThreadExEntryPage.g.h"

#include "Models/MiniDumpPageBase.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct ThreadExListStreamEntry;

    struct ThreadExEntryPage : ThreadExEntryPageT<ThreadExEntryPage>, MiniDumpPageBase<ThreadExEntryPage>
    {
        ThreadExEntryPage();

        // ReSharper disable once CppHidingFunction
        void InitializeComponent();

        // ReSharper disable once CppHidingFunction
        void OnNavigatedTo(Microsoft::UI::Xaml::Navigation::NavigationEventArgs const& e)
        {
            MiniDumpPageBase::OnNavigatedTo(e);
        }

        MiniDumpExplorer::ThreadExListStreamEntry Thread() const { return thread_; }

    private:
        void SetupFlyoutMenus();
        void MiniDumpLoaded(MiniDumpExplorer::MiniDumpPageParameters const& parameters) override;

    private:
        MiniDumpExplorer::ThreadExListStreamEntry thread_;
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct ThreadExEntryPage : ThreadExEntryPageT<ThreadExEntryPage, implementation::ThreadExEntryPage>
    {
    };
}
