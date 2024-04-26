#pragma once

#include "UnloadedModuleListStreamEntriesDataSource.g.h"

namespace dlg_help_utils
{
    class unloaded_module_list_stream;
    namespace time_utils
    {
        struct locale_timezone_info;
    }
}

namespace winrt::MiniDumpExplorer::implementation
{
    struct DataGrid;
    struct DataGridColumnEventArgs;
    struct UnloadedModuleListStreamEntry;

    struct UnloadedModuleListStreamEntriesDataSource : UnloadedModuleListStreamEntriesDataSourceT<UnloadedModuleListStreamEntriesDataSource>
    {
        UnloadedModuleListStreamEntriesDataSource();

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

        fire_and_forget LoadMiniDumpUnloadedModuleStream(dlg_help_utils::unloaded_module_list_stream module_list);

    private:
        void SetupDataProperties();

    private:
        Microsoft::UI::Xaml::Data::CollectionViewSource collectionViewSource_{};
        Windows::Foundation::Collections::IObservableVector<MiniDumpExplorer::UnloadedModuleListStreamEntry> entries_{winrt::single_threaded_observable_vector<MiniDumpExplorer::UnloadedModuleListStreamEntry>()};
        Windows::Foundation::Collections::IVector<IDataGridDataSourcePropertyInfo> dataProperties_{winrt::single_threaded_vector<IDataGridDataSourcePropertyInfo>()};
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct UnloadedModuleListStreamEntriesDataSource : UnloadedModuleListStreamEntriesDataSourceT<UnloadedModuleListStreamEntriesDataSource, implementation::UnloadedModuleListStreamEntriesDataSource>
    {
    };
}
