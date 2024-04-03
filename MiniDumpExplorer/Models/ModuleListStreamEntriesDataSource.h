#pragma once

#include "ModuleListStreamEntriesDataSource.g.h"

namespace dlg_help_utils
{
    class module_list_stream;
}

namespace winrt::MiniDumpExplorer::implementation
{
    struct DataGrid;
    struct DataGridColumnEventArgs;
    struct ModuleListStreamEntry;

    struct ModuleListStreamEntriesDataSource : ModuleListStreamEntriesDataSourceT<ModuleListStreamEntriesDataSource>
    {
        ModuleListStreamEntriesDataSource();

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

        void LoadMiniDumpModuleStream(dlg_help_utils::module_list_stream const& module_list) const;

    private:
        void SetupDataProperties();

    private:
        Microsoft::UI::Xaml::Data::CollectionViewSource collectionViewSource_{};
        Windows::Foundation::Collections::IObservableVector<MiniDumpExplorer::ModuleListStreamEntry> entries_{winrt::single_threaded_observable_vector<MiniDumpExplorer::ModuleListStreamEntry>()};
        Windows::Foundation::Collections::IVector<IDataGridDataSourcePropertyInfo> dataProperties_{winrt::single_threaded_vector<IDataGridDataSourcePropertyInfo>()};
        bool ignoreNextRecentFilesChanged_{false};
        bool loaded_{false};
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct ModuleListStreamEntriesDataSource : ModuleListStreamEntriesDataSourceT<ModuleListStreamEntriesDataSource, implementation::ModuleListStreamEntriesDataSource>
    {
    };
}
