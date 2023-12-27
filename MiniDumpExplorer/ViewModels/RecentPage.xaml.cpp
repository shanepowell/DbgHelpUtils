#include "pch.h"
#include "RecentPage.xaml.h"

#include <winrt/Windows.Storage.h>

#include "RecentFileItemsDataSource.h"
#include "Helpers/WindowHelper.h"
#include "Models/RecentFileItem.h"
#include "Utility/logger.h"

#if __has_include("RecentPage.g.cpp")
// ReSharper disable once CppUnusedIncludeDirective
#include "RecentPage.g.cpp"  // NOLINT(bugprone-suspicious-include)
#endif


using namespace winrt;
using namespace Microsoft::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    RecentPage::RecentPage()
    {
        InitializeComponent();

        [[maybe_unused]] auto task = RecentPageLoad();

        onRowDoubleTapped_ = RecentFilesDataGrid().RowDoubleTapped({ this, &RecentPage::OnRowDoubleTapped });
    }

    void RecentPage::ClearAllRecentFiles([[maybe_unused]] Windows::Foundation::IInspectable const& sender, [[maybe_unused]] RoutedEventArgs const& e) const
    {
        recentFileItemsDataSource_.as<RecentFileItemsDataSource>()->ClearAllRecentFiles();
    }

    void RecentPage::LoadSelectedFile([[maybe_unused]] Windows::Foundation::IInspectable const& sender, [[maybe_unused]] RoutedEventArgs const& e) const
    {
        // ReSharper disable once CppExpressionWithoutSideEffects
        LoadCurrentlySelectedFile();
    }

    fire_and_forget RecentPage::RecentPageLoad() const
    {
        try
        {
            auto const recentFileItemsDataSource = recentFileItemsDataSource_.as<RecentFileItemsDataSource>();
            co_await recentFileItemsDataSource->LoadRecentFiles();
        }
        catch(...)
        {
            logger::HandleUnknownException();
        }
    }

    fire_and_forget RecentPage::LoadCurrentlySelectedFile() const
    {
        if (auto selectedItem = recentFileItemsDataSource_.CollectionView().CurrentItem();
            selectedItem)
        {
            if (auto const recentFileItem = selectedItem.as<RecentFileItem>();
                recentFileItem)
            {
                if(auto const mainWindow = WindowHelper::GetWindowForElement(*this).as<MainWindow>();
                    mainWindow)
                {
                    auto file = co_await Windows::Storage::StorageFile::GetFileFromPathAsync(recentFileItem->FullPath());
                    mainWindow.OpenFileInTab(file);
                }
            }
        }
    }

    void RecentPage::OnRowDoubleTapped([[maybe_unused]] Windows::Foundation::IInspectable const& sender, [[maybe_unused]] DataGridRowDetailsEventArgs const& e) const
    {
        // ReSharper disable once CppExpressionWithoutSideEffects
        LoadCurrentlySelectedFile();
    }
}
