#pragma once

#include "DirectoryStreamEntriesDataSource.g.h"

namespace dlg_help_utils
{
    class mini_dump;
}

namespace winrt::MiniDumpExplorer::implementation
{
    struct DirectoryStreamEntry;

    struct DirectoryStreamEntriesDataSource : DirectoryStreamEntriesDataSourceT<DirectoryStreamEntriesDataSource>
    {
        DirectoryStreamEntriesDataSource();

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

        void LoadMiniDumpStreams(dlg_help_utils::mini_dump const& mini_dump);

    private:
        void SetupDataProperties();

    private:
        Microsoft::UI::Xaml::Data::CollectionViewSource collectionViewSource_{};
        Windows::Foundation::Collections::IObservableVector<MiniDumpExplorer::DirectoryStreamEntry> entries_{winrt::single_threaded_observable_vector<MiniDumpExplorer::DirectoryStreamEntry>()};
        Windows::Foundation::Collections::IVector<IDataGridDataSourcePropertyInfo> dataProperties_{winrt::single_threaded_vector<IDataGridDataSourcePropertyInfo>()};
        bool ignoreNextRecentFilesChanged_{false};
        bool loaded_{false};
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct DirectoryStreamEntriesDataSource : DirectoryStreamEntriesDataSourceT<DirectoryStreamEntriesDataSource, implementation::DirectoryStreamEntriesDataSource>
    {
    };
}
