#pragma once

#include "RecentFileItemsDataSource.g.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct DataGrid;
    struct DataGridColumnEventArgs;
    struct RecentFileItem;

    struct RecentFileItemsDataSource : RecentFileItemsDataSourceT<RecentFileItemsDataSource>
    {
        RecentFileItemsDataSource();

        [[nodiscard]] bool IsReadOnly() const;
        [[nodiscard]] bool CanSort() const;
        [[nodiscard]] bool CanCancelEdit() const;
        [[nodiscard]] bool DataIsPrimitive() const;
        [[nodiscard]] bool IsAddingNew() const;
        [[nodiscard]] bool CanGroup() const;
        [[nodiscard]] bool SupportsNewItemPlaceholder() const;
        [[nodiscard]] bool HasDataProperties() const;
        [[nodiscard]] bool HasDataType() const;
        [[nodiscard]] hstring DataTypeTypeName();
        [[nodiscard]] hstring DataTypeName();
        [[nodiscard]] Microsoft::UI::Xaml::Data::ICollectionView CollectionView() const;
        [[nodiscard]] Windows::Foundation::Collections::IVectorView<IDataGridDataSourcePropertyInfo> DataProperties() const;
        [[nodiscard]] bool GetPropertyIsReadOnly(hstring const& propertyName);
        bool NotifyingDataSourceReset();
        void ClearDataProperties();
        void UpdateDataProperties();
        [[nodiscard]] hstring GetPropertyDisplayName(hstring const& propertyPath);
        void Sort(MiniDumpExplorer::DataGrid const& dataGrid, MiniDumpExplorer::DataGridColumnEventArgs const& args) const;

        void ClearAllRecentFiles();

        Windows::Foundation::IAsyncAction LoadRecentFiles();

    private:
        void SetupDataProperties();
        void SaveRecentFiles();
        fire_and_forget OnRecentFilesChanged(std::vector<std::wstring> const& recentFiles);
        Windows::Foundation::IAsyncAction LoadRecordFilesIntoCollection(std::vector<std::wstring> recentFiles);

    private:
        Microsoft::UI::Xaml::Data::CollectionViewSource collectionViewSource_{};
        Windows::Foundation::Collections::IObservableVector<MiniDumpExplorer::RecentFileItem> recentItems_{winrt::single_threaded_observable_vector<MiniDumpExplorer::RecentFileItem>()};
        Windows::Foundation::Collections::IVector<IDataGridDataSourcePropertyInfo> dataProperties_{winrt::single_threaded_vector<IDataGridDataSourcePropertyInfo>()};
        bool ignoreNextRecentFilesChanged_{false};
        bool loaded_{false};
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct RecentFileItemsDataSource : RecentFileItemsDataSourceT<RecentFileItemsDataSource, implementation::RecentFileItemsDataSource>
    {
    };
}
