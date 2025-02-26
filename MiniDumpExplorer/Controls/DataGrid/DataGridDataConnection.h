#pragma once

#include <winrt/MiniDumpExplorer.h>

#include "DataGridSelectionAction.h"
#include "Utility/WeakEventListener.h"

namespace winrt::MiniDumpExplorer
{
    struct IDataGridDataSource;
    struct DataGrid;  // NOLINT(bugprone-forward-declaration-namespace)

    namespace implementation
    {
        struct DataGrid;
    }
}

namespace DataGridInternal
{

    class DataGridDataConnection
    {
    public:
        static constexpr auto InvalidIndex = int32_t{-1};

        DataGridDataConnection(winrt::MiniDumpExplorer::implementation::DataGrid* owner);

        bool AllowEdit() const;
        bool AllowSort() const;
        bool CanCancelEdit() const;
        winrt::Microsoft::UI::Xaml::Data::ICollectionView CollectionView() const;
        winrt::MiniDumpExplorer::IDataGridDataSource const& DataSource() const { return dataSource_; }
        void DataSource(winrt::MiniDumpExplorer::IDataGridDataSource const& value) { dataSource_ = value; }
        int32_t Count() const;
        bool DataIsPrimitive() const;
        bool HasDataType() const;
        winrt::hstring DataTypeTypeName() const;
        winrt::hstring DataTypeName() const;
        winrt::Windows::Foundation::Collections::IVectorView<winrt::MiniDumpExplorer::IDataGridDataSourcePropertyInfo> DataProperties() const;
        winrt::hstring DisplayName() const;
        bool HasMoreItems() const;
        bool IsDataSourceIncremental() const;
        bool IsLoadingMoreItems() const;
        bool EndingEdit() const;
        bool EventsWired() const;
        bool IsAddingNew() const;
        bool IsGrouping() const;
        int32_t NewItemPlaceholderIndex() const;
        bool ShouldAutoGenerateColumns() const;
        bool BeginEdit(winrt::Windows::Foundation::IInspectable const& dataItem);
        bool CancelEdit(winrt::Windows::Foundation::IInspectable const& dataItem);
        bool EndEdit(winrt::Windows::Foundation::IInspectable const& dataItem);
        winrt::Windows::Foundation::IInspectable GetDataItem(int32_t index) const;
        bool GetPropertyIsReadOnly(std::wstring const& propertyName) const;
        int32_t IndexOf(winrt::Windows::Foundation::IInspectable const& dataItem) const;
        void LoadMoreItems(int32_t count);
        winrt::hstring GetPropertyDisplayName(winrt::hstring const& propertyPath) const;
        static winrt::MiniDumpExplorer::IDataGridDataSource CreateView(winrt::Windows::Foundation::IInspectable const& source);
        void ClearDataProperties() const;
        void MoveCurrentTo(winrt::Windows::Foundation::IInspectable const& item, int32_t backupSlot, int32_t columnIndex, DataGridSelectionAction action, bool scrollIntoView);
        void UnWireEvents(winrt::Windows::Foundation::IInspectable const& value);
        void WireEvents(winrt::Windows::Foundation::IInspectable const& value);

    private:
        void OnCollectionViewCurrentChanged(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::Foundation::IInspectable const& e);
        void OnCollectionViewCurrentChanging(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::Data::CurrentChangingEventArgs const& e) const;
        void NotifyingDataSourceCollectionChanged(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::Interop::NotifyCollectionChangedEventArgs const& e) const;
        void NotifyingDataSourceVectorChanged(winrt::Windows::Foundation::Collections::IObservableVector<winrt::Windows::Foundation::IInspectable> const& sender, winrt::Windows::Foundation::Collections::IVectorChangedEventArgs const& e) const;
        void NotifyingDataSourceAdd(int32_t index) const;
        void NotifyingDataSourceReset() const;
        void NotifyingIncrementalItemsSource(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::Data::PropertyChangedEventArgs const& e) const;
        void OnLoadingOperationCompleted(winrt::Windows::Foundation::IAsyncOperation<winrt::Microsoft::UI::Xaml::Data::LoadMoreItemsResult> const& asyncInfo, winrt::Windows::Foundation::AsyncStatus const& asyncStatus);
        void UpdateDataProperties() const;
        void UpdateIncrementalItemsSource();

    private:
        int32_t backupSlotForCurrentChanged_{};
        int32_t columnForCurrentChanged_{};
        winrt::MiniDumpExplorer::IDataGridDataSource dataSource_{nullptr};
        bool expectingCurrentChanged_{};
        winrt::Microsoft::UI::Xaml::Data::ISupportIncrementalLoading incrementalItemsSource_{nullptr};
        winrt::Windows::Foundation::IInspectable itemToSelectOnCurrentChanged_{nullptr};
        winrt::Windows::Foundation::IAsyncOperation<winrt::Microsoft::UI::Xaml::Data::LoadMoreItemsResult> loadingOperation_{nullptr};
        winrt::MiniDumpExplorer::implementation::DataGrid* owner_;
        bool scrollForCurrentChanged_{};
        DataGridSelectionAction selectionActionForCurrentChanged_{};
        std::unique_ptr<Utility::WeakEventListener<winrt::MiniDumpExplorer::DataGrid, winrt::Windows::Foundation::IInspectable, winrt::Microsoft::UI::Xaml::Interop::NotifyCollectionChangedEventArgs>> weakCollectionChangedListener_{};
        std::unique_ptr<Utility::WeakEventListener<winrt::MiniDumpExplorer::DataGrid, winrt::Windows::Foundation::Collections::IObservableVector<winrt::Windows::Foundation::IInspectable>, winrt::Windows::Foundation::Collections::IVectorChangedEventArgs>> weakVectorChangedListener_{};
        std::unique_ptr<Utility::WeakEventListener<winrt::MiniDumpExplorer::DataGrid, winrt::Windows::Foundation::IInspectable, winrt::Microsoft::UI::Xaml::Data::CurrentChangingEventArgs>> weakCurrentChangingListener_{};
        std::unique_ptr<Utility::WeakEventListener<winrt::MiniDumpExplorer::DataGrid, winrt::Windows::Foundation::IInspectable, winrt::Windows::Foundation::IInspectable>> weakCurrentChangedListener_{};
        std::unique_ptr<Utility::WeakEventListener<winrt::MiniDumpExplorer::DataGrid, winrt::Windows::Foundation::IInspectable, winrt::Microsoft::UI::Xaml::Data::PropertyChangedEventArgs>> weakIncrementalItemsSourcePropertyChangedListener_{};
        bool endingEdit_{};
        bool eventsWired_{};
    };

}
