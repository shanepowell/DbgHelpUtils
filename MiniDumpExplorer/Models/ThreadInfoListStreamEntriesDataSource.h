#pragma once

#include "ThreadInfoListStreamEntriesDataSource.g.h"

namespace dlg_help_utils
{
    namespace time_utils
    {
        struct locale_timezone_info;
    }

    class thread_info_list_stream;
}

namespace winrt::MiniDumpExplorer::implementation
{
    struct DataGrid;
    struct DataGridColumnEventArgs;
    struct ThreadInfoListStreamEntry;

    struct ThreadInfoListStreamEntriesDataSource : ThreadInfoListStreamEntriesDataSourceT<ThreadInfoListStreamEntriesDataSource>
    {
        ThreadInfoListStreamEntriesDataSource();

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

        fire_and_forget LoadMiniDumpThreadInfoStream(dlg_help_utils::thread_info_list_stream thread_list, dlg_help_utils::time_utils::locale_timezone_info dump_file_timezone_info);

    private:
        void SetupDataProperties();

    private:
        Microsoft::UI::Xaml::Data::CollectionViewSource collectionViewSource_{};
        Windows::Foundation::Collections::IObservableVector<MiniDumpExplorer::ThreadInfoListStreamEntry> entries_{winrt::single_threaded_observable_vector<MiniDumpExplorer::ThreadInfoListStreamEntry>()};
        Windows::Foundation::Collections::IVector<IDataGridDataSourcePropertyInfo> dataProperties_{winrt::single_threaded_vector<IDataGridDataSourcePropertyInfo>()};
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct ThreadInfoListStreamEntriesDataSource : ThreadInfoListStreamEntriesDataSourceT<ThreadInfoListStreamEntriesDataSource, implementation::ThreadInfoListStreamEntriesDataSource>
    {
    };
}
