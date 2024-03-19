#include "pch.h"
#include "RecentFileItemsDataSource.h"

#include <winrt/Windows.UI.Xaml.Interop.h>

#include "Helpers/GlobalOptions.h"
#include "Models/RecentFileItem.h"
#include "Utility/logger.h"

#if __has_include("RecentFileItemsDataSource.g.cpp")
// ReSharper disable once CppUnusedIncludeDirective
#include "RecentFileItemsDataSource.g.cpp"  // NOLINT(bugprone-suspicious-include)
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    RecentFileItemsDataSource::RecentFileItemsDataSource()
    {
        SetupDataProperties();
        collectionViewSource_.Source(recentItems_);
    }

    // ReSharper disable CppMemberFunctionMayBeStatic

    bool RecentFileItemsDataSource::IsReadOnly() const
    {
        return true;
    }

    bool RecentFileItemsDataSource::CanSort() const
    {
        return true;
    }

    bool RecentFileItemsDataSource::CanCancelEdit() const
    {
        return false;
    }

    bool RecentFileItemsDataSource::DataIsPrimitive() const
    {
        return false;
    }

    bool RecentFileItemsDataSource::IsAddingNew() const
    {
        return false;
    }

    bool RecentFileItemsDataSource::CanGroup() const
    {
        return false;
    }

    bool RecentFileItemsDataSource::SupportsNewItemPlaceholder() const
    {
        return false;
    }

    bool RecentFileItemsDataSource::HasDataProperties() const
    {
        return true;
    }

    bool RecentFileItemsDataSource::HasDataType() const
    {
        return true;
    }

    hstring RecentFileItemsDataSource::DataTypeTypeName()
    {
        return xaml_typename<MiniDumpExplorer::RecentFileItem>().Name;
    }

    hstring RecentFileItemsDataSource::DataTypeName()
    {
        return L"RecentFileItem";
    }

    bool RecentFileItemsDataSource::GetPropertyIsReadOnly([[maybe_unused]] hstring const& propertyName)
    {
        return true;
    }

    // ReSharper restore CppMemberFunctionMayBeStatic

    Data::ICollectionView RecentFileItemsDataSource::CollectionView() const
    {
        return collectionViewSource_.View();
    }

    Windows::Foundation::Collections::IVectorView<IDataGridDataSourcePropertyInfo> RecentFileItemsDataSource::DataProperties() const
    {
        return dataProperties_.GetView();
    }

    // ReSharper disable CppMemberFunctionMayBeStatic

    bool RecentFileItemsDataSource::NotifyingDataSourceReset()
    {
        return false;
    }

    void RecentFileItemsDataSource::ClearDataProperties()
    {
    }

    void RecentFileItemsDataSource::UpdateDataProperties()
    {
    }

    hstring RecentFileItemsDataSource::GetPropertyDisplayName(hstring const& propertyPath)
    {
        return propertyPath;
    }

    // ReSharper disable once CppMemberFunctionMayBeConst
    void RecentFileItemsDataSource::ClearAllRecentFiles()
    {
        recentItems_.Clear();
        GlobalOptions::Options().RecentFiles({});
    }

    // ReSharper disable once CppMemberFunctionMayBeConst
    void RecentFileItemsDataSource::SetupDataProperties()
    {
    }

    // ReSharper disable once CppMemberFunctionMayBeConst
    void RecentFileItemsDataSource::SaveRecentFiles()
    {
        logger::Log().LogMessage(log_level::debug, "Saving current recent files list");
        std::vector<std::wstring> recentFiles;
        recentFiles.reserve(recentItems_.Size());
        for(const auto& item : recentItems_)
        {
            recentFiles.emplace_back(item.as<RecentFileItem>()->FullPath());
        }
        GlobalOptions::Options().RecentFiles(recentFiles);
    }

    fire_and_forget RecentFileItemsDataSource::OnRecentFilesChanged(std::vector<std::wstring> const& recentFiles)
    {
        if(ignoreNextRecentFilesChanged_)
        {
            ignoreNextRecentFilesChanged_ = false;
            co_return;
        }

        recentItems_.Clear();

        logger::Log().LogMessage(log_level::debug, "Reloading recent files icons");

        auto removedItems = false;
        for(const auto& file : recentFiles)
        {
            MiniDumpExplorer::RecentFileItem item{hstring{file}};
            if(auto const internalItem = item.as<RecentFileItem>();
                internalItem->Exists())
            {
                recentItems_.Append(item);
            }
            else
            {
                removedItems = true;
            }
        }

        if(removedItems)
        {
            ignoreNextRecentFilesChanged_ = true;
            SaveRecentFiles();
        }

        logger::Log().LogMessage(log_level::debug, "Reloading recent files icons");

        for (auto recentItem : recentItems_)
        {
            co_await recentItem.as<RecentFileItem>()->LoadIconAsync();
        }

        logger::Log().LogMessage(log_level::debug, "Reloading recent files complete");
    }

    // ReSharper restore CppMemberFunctionMayBeStatic

    // ReSharper disable once CppMemberFunctionMayBeConst
    Windows::Foundation::IAsyncAction RecentFileItemsDataSource::LoadRecentFiles()
    {
        if(loaded_)
        {
            co_return;
        }

        logger::Log().LogMessage(log_level::debug, "Loading recent files...");

        auto removedItems = false;
        for(const auto& file : GlobalOptions::Options().RecentFiles())
        {
            MiniDumpExplorer::RecentFileItem item{hstring{file}};
            if(auto const internalItem = item.as<RecentFileItem>();
                internalItem->Exists())
            {
                recentItems_.Append(item);
            }
            else
            {
                removedItems = true;
            }
        }

        if(removedItems)
        {
            SaveRecentFiles();
        }

        GlobalOptions::Options().OnRecentFiles([ptr = get_weak()](auto const& recentFiles)
            {
                if(auto const self = ptr.get())
                {
                    self->OnRecentFilesChanged(recentFiles);
                    return true;
                }

                return false;
            });

        loaded_ = true;

        logger::Log().LogMessage(log_level::debug, "Loading recent files icons");

        for (auto recentItem : recentItems_)
        {
            co_await recentItem.as<RecentFileItem>()->LoadIconAsync();
        }

        logger::Log().LogMessage(log_level::debug, "Loading recent files complete");
    }
}
