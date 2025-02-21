#pragma once

#include "ThreadEntryPage.g.h"

#include "Models/MiniDumpPageBase.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct ThreadListStreamEntry;

    struct ThreadEntryPage : ThreadEntryPageT<ThreadEntryPage>, MiniDumpPageBase<ThreadEntryPage>
    {
        ThreadEntryPage();

        // ReSharper disable once CppHidingFunction
        void InitializeComponent();

        // ReSharper disable once CppHidingFunction
        void OnNavigatedTo(Microsoft::UI::Xaml::Navigation::NavigationEventArgs const& e)
        {
            MiniDumpPageBase::OnNavigatedTo(e);
        }

        MiniDumpExplorer::ThreadListStreamEntry Thread() const { return thread_; }

        static void OnTreeViewItemExpanding(Microsoft::UI::Xaml::Controls::TreeView const& sender, Microsoft::UI::Xaml::Controls::TreeViewExpandingEventArgs const& args);

    private:
        void SetupFlyoutMenus();
        void MiniDumpLoaded(MiniDumpExplorer::MiniDumpPageParameters const& parameters) override;

    private:
        MiniDumpExplorer::ThreadListStreamEntry thread_;
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct ThreadEntryPage : ThreadEntryPageT<ThreadEntryPage, implementation::ThreadEntryPage>
    {
    };
}
