#include "pch.h"
#include "RecentFileItemsDataSource.h"

#include <winrt/Windows.UI.Xaml.Interop.h>

#include "DbgHelpUtils/string_compare.h"
#include "DbgHelpUtils/wide_runtime_error.h"
#include "Helpers/GlobalOptions.h"
#include "Models/RecentFileItem.h"
#include "Utility/logger.h"
#include "Utility/SortObservableVector.h"

#if __has_include("RecentFileItemsDataSource.g.cpp")
// ReSharper disable once CppUnusedIncludeDirective
#include "RecentFileItemsDataSource.g.cpp"  // NOLINT(bugprone-suspicious-include)
#endif

#include <unordered_map>

using namespace winrt;
using namespace Microsoft::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    namespace
    {
        enum class CompareResult
        {
            Less,
            Equal,
            Greater
        };

        CompareResult CompareStrings(std::wstring_view const& a, std::wstring_view const& b)
        {
            auto const result = dlg_help_utils::string_utils::icompare(a, b);
            if(result < 0)
            {
                return CompareResult::Less;
            }
            if (result > 0)
            {
                return CompareResult::Greater;
            }
            return CompareResult::Equal;
        }

        std::unordered_map<std::wstring, std::function<CompareResult(MiniDumpExplorer::RecentFileItem const& a, MiniDumpExplorer::RecentFileItem const& b)>> const ColumnSorters
            {
                {L"FileName", [](MiniDumpExplorer::RecentFileItem const& a, MiniDumpExplorer::RecentFileItem const& b)
                    {
                        return CompareStrings(a.Name(), b.Name());
                    }},
                {L"Location", [](MiniDumpExplorer::RecentFileItem const& a, MiniDumpExplorer::RecentFileItem const& b)
                    {
                        return CompareStrings(a.Location(), b.Location());
                    }},
                {L"Size", [](MiniDumpExplorer::RecentFileItem const& a, MiniDumpExplorer::RecentFileItem const& b)
                    {
                        return a.Size() == b.Size() ? CompareResult::Equal : a.Size() < b.Size() ? CompareResult::Less : CompareResult::Greater;
                    }}
            };

        bool UnorderedCompare(MiniDumpExplorer::RecentFileItem const& a, MiniDumpExplorer::RecentFileItem const& b)
        {
            return a.Index() < b.Index();
        }
    }

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

    void RecentFileItemsDataSource::Sort([[maybe_unused]] MiniDumpExplorer::DataGrid const& dataGrid, [[maybe_unused]] MiniDumpExplorer::DataGridColumnEventArgs const& args) const
    {
        auto sortDirection = args.Column().SortDirection();

        if(sortDirection)
        {
            switch(sortDirection.Value())
            {
            case DataGridSortDirection::Ascending:
                sortDirection = {};
                break;
            case DataGridSortDirection::Descending:
                sortDirection = DataGridSortDirection::Ascending;
                break;
            }
        }
        else
        {
            sortDirection = DataGridSortDirection::Descending;
        }

        Utility::SortObservableVector(recentItems_, GetColumnComparer(sortDirection, args.Column().Tag().as<hstring>()));

        args.Column().SortDirection(sortDirection);

        for(auto const& column : dataGrid.Columns())
        {
            if(column != args.Column())
            {
                column.SortDirection({});
            }
        }
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

        LoadRecordFilesIntoCollection(recentFiles);
    }

    // ReSharper restore CppMemberFunctionMayBeStatic

    // ReSharper disable once CppMemberFunctionMayBeConst
    Windows::Foundation::IAsyncAction RecentFileItemsDataSource::LoadRecentFiles()
    {
        if(loaded_)
        {
            co_return;
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

        co_await LoadRecordFilesIntoCollection(GlobalOptions::Options().RecentFiles());
    }

    Windows::Foundation::IAsyncAction RecentFileItemsDataSource::LoadRecordFilesIntoCollection(std::vector<std::wstring> const& recentFiles)
    {
        recentItems_.Clear();

        logger::Log().LogMessage(log_level::debug, loaded_ ? "Loading recent files..." : "Reloading recent files...");

        auto removedItems = false;
        uint32_t index{0};
        for(const auto& file : recentFiles)
        {
            MiniDumpExplorer::RecentFileItem item{index, hstring{file}};
            if(auto const internalItem = item.as<RecentFileItem>();
                internalItem->Exists())
            {
                recentItems_.Append(item);
                ++index;
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

        loaded_ = true;

        logger::Log().LogMessage(log_level::debug, loaded_ ? "Loading recent files icons" : "Reloading recent files icons");

        for (auto recentItem : recentItems_)
        {
            co_await recentItem.as<RecentFileItem>()->LoadIconAsync();
        }

        logger::Log().LogMessage(log_level::debug, loaded_ ? "Loading recent files complete" : "Reloading recent files complete");
    }

    std::unordered_map<MiniDumpExplorer::RecentFileItem, uint32_t> RecentFileItemsDataSource::GetRecentFileCurrentSortOrder() const
    {
        std::unordered_map<MiniDumpExplorer::RecentFileItem, uint32_t> sortOrder;
        uint32_t index{0};
        for(const auto& item : recentItems_)
        {
            sortOrder[item] = index++;
        }
        return sortOrder;
    }

    std::function<bool(MiniDumpExplorer::RecentFileItem const& a, MiniDumpExplorer::RecentFileItem const& b)> RecentFileItemsDataSource::GetColumnComparer(Windows::Foundation::IReference<DataGridSortDirection> const& sortDirection, hstring const& columnTag) const
    {
        if(sortDirection)
        {
            switch(sortDirection.Value())
            {
            case DataGridSortDirection::Ascending:
                return [columnComparer = ColumnSorters.at(std::wstring{columnTag}), currentSortOrder = GetRecentFileCurrentSortOrder()](MiniDumpExplorer::RecentFileItem const& a, MiniDumpExplorer::RecentFileItem const& b)
                {
                    switch(columnComparer(a, b))
                    {
                    case CompareResult::Less:
                        return false;
                    case CompareResult::Equal:
                        return currentSortOrder.at(a) < currentSortOrder.at(b);
                    case CompareResult::Greater:
                        return true;
                    }

                    throw dlg_help_utils::exceptions::wide_runtime_error{std::format(L"Unsupported compare result")};
                };

            case DataGridSortDirection::Descending:
                return [columnComparer = ColumnSorters.at(std::wstring{columnTag}), currentSortOrder = GetRecentFileCurrentSortOrder()](MiniDumpExplorer::RecentFileItem const& a, MiniDumpExplorer::RecentFileItem const& b)
                {
                    switch(columnComparer(a, b))
                    {
                    case CompareResult::Less:
                        return true;
                    case CompareResult::Equal:
                        return currentSortOrder.at(a) < currentSortOrder.at(b);
                    case CompareResult::Greater:
                        return false;
                    }

                    throw dlg_help_utils::exceptions::wide_runtime_error{std::format(L"Unsupported compare result")};
                };

            default:
                throw dlg_help_utils::exceptions::wide_runtime_error{std::format(L"Unsupported sort direction : {}", static_cast<int>(sortDirection.Value()))};
            }
        }

        return UnorderedCompare;
    }
}
