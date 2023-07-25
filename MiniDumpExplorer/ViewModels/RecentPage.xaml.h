#pragma once

#include "RecentPage.g.h"
#include "Models/RecentFileItem.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct RecentPage : RecentPageT<RecentPage>
    {
        RecentPage();

        Windows::Foundation::Collections::IObservableVector<MiniDumpExplorer::RecentFileItem> RecentFileItems() const { return recentFileItems_; }
        void ClearAllRecentFiles(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::RoutedEventArgs const& e) const;

    private:
        Windows::Foundation::Collections::IObservableVector<MiniDumpExplorer::RecentFileItem> recentFileItems_;
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct RecentPage : RecentPageT<RecentPage, implementation::RecentPage>
    {
    };
}
