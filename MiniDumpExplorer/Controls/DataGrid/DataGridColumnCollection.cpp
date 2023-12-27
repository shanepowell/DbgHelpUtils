﻿#include "pch.h"
#include "DataGridColumnCollection.h"

#include "DataGrid.h"
#include "DataGridError.h"
#include "DataGridFillerColumn.h"
#include "DbgHelpUtils/exit_scope.h"

using namespace winrt;
using namespace winrt::MiniDumpExplorer::implementation;

namespace DataGridInternal
{
    struct DataGridColumnCollection::impl
    {
        impl(DataGrid* owningGrid)
            : owningGrid_{owningGrid}
        {
        }

        void Init()
        {
            rowGroupSpacerColumn_ = MiniDumpExplorer::DataGridFillerColumn{*owningGrid_};
            fillerColumn_ = MiniDumpExplorer::DataGridFillerColumn{*owningGrid_};
        }

        Windows::Foundation::Collections::IObservableVector<MiniDumpExplorer::DataGridColumn> itemsSource_{ single_threaded_observable_vector<MiniDumpExplorer::DataGridColumn>() };
        DataGrid* owningGrid_;
        MiniDumpExplorer::DataGridFillerColumn rowGroupSpacerColumn_{nullptr};
        double visibleEdgedColumnsWidth_{};
        int32_t visibleStarColumnCount_{};
        int32_t autoGeneratedColumnCount_{};
        std::vector<int32_t> displayIndexMap_{};
        MiniDumpExplorer::DataGridFillerColumn fillerColumn_{nullptr};
        std::vector<MiniDumpExplorer::DataGridColumn> itemsInternal_{};
    };


    DataGridColumnCollection::DataGridColumnCollection(DataGrid* owningGrid)
        : impl_{ std::make_unique<impl>(owningGrid) }
    {
        impl_->Init();
        [[maybe_unused]] auto const collectionChangedEvent = impl_->itemsSource_.VectorChanged({ this, &DataGridColumnCollection::CollectionChanged });
    }

    DataGridColumnCollection::~DataGridColumnCollection() = default;

    int32_t DataGridColumnCollection::AutoGeneratedColumnCount() const
    {
        return impl_->autoGeneratedColumnCount_;
    }

    void DataGridColumnCollection::AutoGeneratedColumnCount(int32_t const value) const
    {
        impl_->autoGeneratedColumnCount_ = value;
    }

    std::vector<int32_t> const& DataGridColumnCollection::DisplayIndexMap() const
    {
        return impl_->displayIndexMap_;
    }

    std::vector<int32_t>& DataGridColumnCollection::DisplayIndexMap()
    {
        return impl_->displayIndexMap_;
    }

    MiniDumpExplorer::DataGridFillerColumn const& DataGridColumnCollection::FillerColumn() const
    {
        return impl_->fillerColumn_;
    }

    MiniDumpExplorer::DataGridColumn DataGridColumnCollection::FirstColumn() const
    {
        return GetFirstColumn(std::nullopt /*isVisible*/, std::nullopt /*isFrozen*/, std::nullopt /*isReadOnly*/);
    }

    MiniDumpExplorer::DataGridColumn DataGridColumnCollection::FirstVisibleColumn() const
    {
        return GetFirstColumn(true /*isVisible*/, std::nullopt /*isFrozen*/, std::nullopt /*isReadOnly*/);
    }

    MiniDumpExplorer::DataGridColumn DataGridColumnCollection::FirstVisibleNonFillerColumn() const
    {
        auto dataGridColumn = FirstVisibleColumn();
        if (dataGridColumn == RowGroupSpacerColumn())
        {
            dataGridColumn = GetNextVisibleColumn(dataGridColumn);
        }

        return dataGridColumn;
    }

    MiniDumpExplorer::DataGridColumn DataGridColumnCollection::FirstVisibleWritableColumn() const
    {
        return GetFirstColumn(true /*isVisible*/, std::nullopt /*isFrozen*/, false /*isReadOnly*/);
    }

    MiniDumpExplorer::DataGridColumn DataGridColumnCollection::FirstVisibleScrollingColumn() const
    {
        return GetFirstColumn(true /*isVisible*/, false /*isFrozen*/, std::nullopt /*isReadOnly*/);
    }

    std::vector<MiniDumpExplorer::DataGridColumn> const& DataGridColumnCollection::ItemsInternal() const
    {
        return impl_->itemsInternal_;
    }

    std::vector<MiniDumpExplorer::DataGridColumn>& DataGridColumnCollection::ItemsInternal()
    {
        return impl_->itemsInternal_;
    }

    Windows::Foundation::Collections::IObservableVector<MiniDumpExplorer::DataGridColumn> const& DataGridColumnCollection::ObservableItems() const
    {
        return impl_->itemsSource_;
    }

    MiniDumpExplorer::DataGridColumn DataGridColumnCollection::LastVisibleColumn() const
    {
        return GetLastColumn(true /*isVisible*/, std::nullopt /*isFrozen*/, std::nullopt /*isReadOnly*/);
    }

    MiniDumpExplorer::DataGridColumn DataGridColumnCollection::LastVisibleScrollingColumn() const
    {
        return GetLastColumn(true /*isVisible*/, false /*isFrozen*/, std::nullopt /*isReadOnly*/);
    }

    MiniDumpExplorer::DataGridColumn DataGridColumnCollection::LastVisibleWritableColumn() const
    {
        return GetLastColumn(true /*isVisible*/, std::nullopt /*isFrozen*/, false /*isReadOnly*/);
    }

    MiniDumpExplorer::DataGridFillerColumn const& DataGridColumnCollection::RowGroupSpacerColumn() const
    {
        return impl_->rowGroupSpacerColumn_;
    }

    int32_t DataGridColumnCollection::VisibleColumnCount() const
    {
        return static_cast<int32_t>(std::ranges::count_if(ItemsInternal(), [](auto const& item) { return item.template as<DataGridColumn>()->IsVisible(); }));
    }

    double DataGridColumnCollection::VisibleEdgedColumnsWidth() const
    {
        return impl_->visibleEdgedColumnsWidth_;
    }

    int32_t DataGridColumnCollection::VisibleStarColumnCount() const
    {
        return impl_->visibleStarColumnCount_;
    }

    bool DataGridColumnCollection::DisplayInOrder(int32_t const columnIndex1, int32_t const columnIndex2) const
    {
        auto const displayIndex1 = ItemsInternal()[columnIndex1].as<DataGridColumn>()->DisplayIndexWithFiller();
        auto const displayIndex2 = ItemsInternal()[columnIndex2].as<DataGridColumn>()->DisplayIndexWithFiller();
        return displayIndex1 < displayIndex2;
    }

    bool DataGridColumnCollection::EnsureRowGrouping(bool const rowGrouping)
    {
        // The insert below could cause the first column to be added.  That causes a refresh
        // which re-enters this method so instead of checking RowGroupSpacerColumn.IsRepresented,
        // we need to check to see if it's actually in our collection instead.
        auto const spacerRepresented = !ItemsInternal().empty() && ItemsInternal()[0] == RowGroupSpacerColumn();
        if (rowGrouping && !spacerRepresented)
        {
            impl_->itemsSource_.InsertAt(0, RowGroupSpacerColumn());
            RowGroupSpacerColumn().as<DataGridFillerColumn>()->IsRepresented(true);
            return true;
        }

        if (!rowGrouping && spacerRepresented)
        {
            assert(ItemsInternal()[0] == RowGroupSpacerColumn());

            // We need to set IsRepresented to false before removing the RowGroupSpacerColumn
            // otherwise, we'll remove the column after it
            RowGroupSpacerColumn().as<DataGridFillerColumn>()->IsRepresented(false);
            RemoveItemPrivate(0, true /*isSpacer*/);
            assert(DisplayIndexMap().size() == ItemsInternal().size());
            return true;
        }

        return false;
    }

    void DataGridColumnCollection::EnsureVisibleEdgedColumnsWidth()
    {
        impl_->visibleStarColumnCount_ = 0;
        impl_->visibleEdgedColumnsWidth_ = 0;
        for (auto const& column : ItemsInternal())
        {
            if(auto const internalColumn = column.as<DataGridColumn>();
                internalColumn->IsVisible())
            {
                internalColumn->EnsureWidth();
                if (internalColumn->Width().IsStar())
                {
                    impl_->visibleStarColumnCount_++;
                }

                impl_->visibleEdgedColumnsWidth_ += internalColumn->ActualWidth();
            }
        }
    }

    MiniDumpExplorer::DataGridColumn DataGridColumnCollection::GetColumnAtDisplayIndex(int32_t const displayIndex) const
    {
        if (displayIndex < 0 || displayIndex >= ItemsInternal().size() || displayIndex >= DisplayIndexMap().size())
        {
            return nullptr;
        }

        auto const columnIndex = DisplayIndexMap()[displayIndex];
        return ItemsInternal()[columnIndex];
    }

    int32_t DataGridColumnCollection::GetColumnCount(bool const isVisible, bool const isFrozen, int32_t const fromColumnIndex, int32_t const toColumnIndex) const
    {
        assert(DisplayInOrder(fromColumnIndex, toColumnIndex));
        assert(ItemsInternal()[toColumnIndex].as<DataGridColumn>()->IsVisible() == isVisible);
        assert(ItemsInternal()[toColumnIndex].IsFrozen() == isFrozen);

        int32_t columnCount = 0;
        auto dataGridColumn = ItemsInternal()[fromColumnIndex];

        while (dataGridColumn != ItemsInternal()[toColumnIndex])
        {
            dataGridColumn = GetNextColumn(dataGridColumn, isVisible, isFrozen, std::nullopt /*isReadOnly*/);
            assert(dataGridColumn);
            columnCount++;
        }

        return columnCount;
    }

    std::experimental::generator<MiniDumpExplorer::DataGridColumn> DataGridColumnCollection::GetDisplayedColumns()
    {
        assert(ItemsInternal().size() == DisplayIndexMap().size());
        for (auto const columnIndex : DisplayIndexMap())
        {
            co_yield ItemsInternal()[columnIndex];
        }
    }

    std::experimental::generator<MiniDumpExplorer::DataGridColumn> DataGridColumnCollection::GetDisplayedColumns(std::function<bool(MiniDumpExplorer::DataGridColumn const&)> const filter) const
    {
        assert(ItemsInternal().size() == DisplayIndexMap().size());
        for (auto const columnIndex : DisplayIndexMap())
        {
            if(auto column = ItemsInternal()[columnIndex];
                filter(column))
            {
                co_yield column;
            }
        }
    }

    std::experimental::generator<MiniDumpExplorer::DataGridColumn> DataGridColumnCollection::GetDisplayedColumns(bool const reverse, std::function<bool(MiniDumpExplorer::DataGridColumn const&)> filter) const
    {
        return reverse ? GetDisplayedColumnsReverse(std::move(filter)) : GetDisplayedColumns(std::move(filter));
    }

    std::experimental::generator<MiniDumpExplorer::DataGridColumn> DataGridColumnCollection::GetDisplayedColumnsReverse(std::function<bool(MiniDumpExplorer::DataGridColumn const&)> const filter) const
    {
        assert(ItemsInternal().size() == DisplayIndexMap().size());
        for (auto displayIndex = DisplayIndexMap().size(); displayIndex > 0; displayIndex--)
        {
            if (auto column = ItemsInternal()[DisplayIndexMap()[displayIndex - 1]];
                filter(column))
            {
                co_yield column;
            }
        }
    }

    MiniDumpExplorer::DataGridColumn DataGridColumnCollection::GetFirstColumn(std::optional<bool> const isVisible, std::optional<bool> const isFrozen, std::optional<bool> const isReadOnly) const
    {
        assert(ItemsInternal().size() == DisplayIndexMap().size());
        int32_t index = 0;
        while (index < DisplayIndexMap().size())
        {
            if (auto dataGridColumn = GetColumnAtDisplayIndex(index);
                (!isVisible.has_value() || dataGridColumn.as<DataGridColumn>()->IsVisible() == isVisible) &&
                (!isFrozen.has_value() || dataGridColumn.IsFrozen() == isFrozen) &&
                (!isReadOnly.has_value() || dataGridColumn.IsReadOnly() == isReadOnly))
            {
                return dataGridColumn;
            }

            ++index;
        }

        return nullptr;
    }

    MiniDumpExplorer::DataGridColumn DataGridColumnCollection::GetLastColumn(std::optional<bool> const isVisible, std::optional<bool> const isFrozen, std::optional<bool> const isReadOnly) const
    {
        assert(ItemsInternal().size() == DisplayIndexMap().size());
        auto index = static_cast<int32_t>(DisplayIndexMap().size());
        while (index > 0)
        {
            if (auto dataGridColumn = GetColumnAtDisplayIndex(index - 1);
                (!isVisible.has_value() || dataGridColumn.as<DataGridColumn>()->IsVisible() == isVisible) &&
                (!isFrozen.has_value() || dataGridColumn.IsFrozen() == isFrozen) &&
                (!isReadOnly.has_value() || dataGridColumn.IsReadOnly() == isReadOnly))
            {
                return dataGridColumn;
            }

            --index;
        }

        return nullptr;
    }

    MiniDumpExplorer::DataGridColumn DataGridColumnCollection::GetNextColumn(MiniDumpExplorer::DataGridColumn const& dataGridColumnStart) const
    {
        return GetNextColumn(dataGridColumnStart, std::nullopt /*isVisible*/, std::nullopt /*isFrozen*/, std::nullopt /*isReadOnly*/);
    }

    MiniDumpExplorer::DataGridColumn DataGridColumnCollection::GetNextColumn(MiniDumpExplorer::DataGridColumn const& dataGridColumnStart, std::optional<bool> const isVisible, std::optional<bool> const isFrozen, std::optional<bool> const isReadOnly) const
    {
        assert(dataGridColumnStart);
        assert(std::ranges::find(ItemsInternal(), dataGridColumnStart) != ItemsInternal().end());
        assert(ItemsInternal().size() == DisplayIndexMap().size());

        auto index = dataGridColumnStart.as<DataGridColumn>()->DisplayIndexWithFiller() + 1;
        while (index < DisplayIndexMap().size())
        {
            if (auto dataGridColumn = GetColumnAtDisplayIndex(index);
                (!isVisible.has_value() || dataGridColumn.as<DataGridColumn>()->IsVisible() == isVisible) &&
                (!isFrozen.has_value() || dataGridColumn.IsFrozen() == isFrozen) &&
                (!isReadOnly.has_value() || dataGridColumn.IsReadOnly() == isReadOnly))
            {
                return dataGridColumn;
            }

            ++index;
        }

        return nullptr;
    }

    MiniDumpExplorer::DataGridColumn DataGridColumnCollection::GetNextVisibleColumn(MiniDumpExplorer::DataGridColumn const& dataGridColumnStart) const
    {
        return GetNextColumn(dataGridColumnStart, true /*isVisible*/, std::nullopt /*isFrozen*/, std::nullopt /*isReadOnly*/);
    }

    MiniDumpExplorer::DataGridColumn DataGridColumnCollection::GetNextVisibleFrozenColumn(MiniDumpExplorer::DataGridColumn const& dataGridColumnStart) const
    {
        return GetNextColumn(dataGridColumnStart, true /*isVisible*/, true /*isFrozen*/, std::nullopt /*isReadOnly*/);
    }

    MiniDumpExplorer::DataGridColumn DataGridColumnCollection::GetNextVisibleWritableColumn(MiniDumpExplorer::DataGridColumn const& dataGridColumnStart) const
    {
        return GetNextColumn(dataGridColumnStart, true /*isVisible*/, std::nullopt /*isFrozen*/, false /*isReadOnly*/);
    }

    MiniDumpExplorer::DataGridColumn DataGridColumnCollection::GetPreviousColumn(MiniDumpExplorer::DataGridColumn const& dataGridColumnStart, std::optional<bool> const isVisible, std::optional<bool> const isFrozen, std::optional<bool> const isReadOnly) const
    {
        assert(dataGridColumnStart);
        assert(std::ranges::find(ItemsInternal(), dataGridColumnStart) != ItemsInternal().end());
        assert(ItemsInternal().size() == DisplayIndexMap().size());

        auto index = dataGridColumnStart.as<DataGridColumn>()->DisplayIndexWithFiller() - 1;
        while (index >= 0)
        {
            if (auto dataGridColumn = GetColumnAtDisplayIndex(index);
                (!isVisible.has_value() || dataGridColumn.as<DataGridColumn>()->IsVisible() == isVisible) &&
                (!isFrozen.has_value() || dataGridColumn.IsFrozen() == isFrozen) &&
                (!isReadOnly.has_value() || dataGridColumn.IsReadOnly() == isReadOnly))
            {
                return dataGridColumn;
            }

            --index;
        }

        return nullptr;
    }

    MiniDumpExplorer::DataGridColumn DataGridColumnCollection::GetPreviousVisibleNonFillerColumn(MiniDumpExplorer::DataGridColumn const& dataGridColumnStart) const
    {
        auto column = GetPreviousColumn(dataGridColumnStart, true /*isVisible*/, std::nullopt /*isFrozen*/, std::nullopt /*isReadOnly*/);
        return column.try_as<DataGridFillerColumn>() ? nullptr : column;
    }

    MiniDumpExplorer::DataGridColumn DataGridColumnCollection::GetPreviousVisibleScrollingColumn(MiniDumpExplorer::DataGridColumn const& dataGridColumnStart) const
    {
        return GetPreviousColumn(dataGridColumnStart, true /*isVisible*/, false /*isFrozen*/, std::nullopt /*isReadOnly*/);
    }

    MiniDumpExplorer::DataGridColumn DataGridColumnCollection::GetPreviousVisibleWritableColumn(MiniDumpExplorer::DataGridColumn const& dataGridColumnStart) const
    {
        return GetPreviousColumn(dataGridColumnStart, true /*isVisible*/, std::nullopt /*isFrozen*/, false /*isReadOnly*/);
    }

    int32_t DataGridColumnCollection::GetVisibleColumnCount(int32_t const fromColumnIndex, int32_t const toColumnIndex) const
    {
        assert(DisplayInOrder(fromColumnIndex, toColumnIndex));
        assert(ItemsInternal()[toColumnIndex].as<DataGridColumn>()->IsVisible());

        int32_t columnCount = 0;
        auto dataGridColumn = ItemsInternal()[fromColumnIndex];

        while (dataGridColumn != ItemsInternal()[toColumnIndex])
        {
            dataGridColumn = GetNextVisibleColumn(dataGridColumn);
            assert(dataGridColumn);
            ++columnCount;
        }

        return columnCount;
    }

    std::experimental::generator<MiniDumpExplorer::DataGridColumn> DataGridColumnCollection::GetVisibleColumns() const
    {
        return GetDisplayedColumns([](auto const& column) { return column.template as<DataGridColumn>()->IsVisible(); });
    }

    std::experimental::generator<MiniDumpExplorer::DataGridColumn> DataGridColumnCollection::GetVisibleFrozenColumns() const
    {
        return GetDisplayedColumns([](auto const& column) { return column.IsFrozen() && column.template as<DataGridColumn>()->IsVisible(); });
    }

    std::experimental::generator<MiniDumpExplorer::DataGridColumn> DataGridColumnCollection::GetVisibleScrollingColumns() const
    {
        return GetDisplayedColumns([](auto const& column) { return !column.IsFrozen() && column.template as<DataGridColumn>()->IsVisible(); });
    }

    double DataGridColumnCollection::GetVisibleFrozenEdgedColumnsWidth() const
    {
        double visibleFrozenColumnsWidth = 0;
        for (auto const& column : ItemsInternal())
        {
            if(auto const internalColumn = column.as<DataGridColumn>();
                internalColumn->IsVisible() && internalColumn->IsFrozen())
            {
                visibleFrozenColumnsWidth += internalColumn->ActualWidth();
            }
        }

        return visibleFrozenColumnsWidth;
    }

    void DataGridColumnCollection::CollectionChanged([[maybe_unused]] Windows::Foundation::Collections::IObservableVector<MiniDumpExplorer::DataGridColumn> const& sender, Windows::Foundation::Collections::IVectorChangedEventArgs const& event)
    {
        switch(event.CollectionChange())
        {
            case Windows::Foundation::Collections::CollectionChange::ItemInserted:
                InsertItem(event.Index(), impl_->itemsSource_.GetAt(event.Index()));
                break;

            case Windows::Foundation::Collections::CollectionChange::ItemRemoved:
                RemoveItem(event.Index());
                break;

            case Windows::Foundation::Collections::CollectionChange::ItemChanged:
                break;

            case Windows::Foundation::Collections::CollectionChange::Reset:
                ClearItems();
                break;
        }
    }

    void DataGridColumnCollection::ClearItems()
    {
        assert(impl_->owningGrid_);
        impl_->owningGrid_->NoCurrentCellChangeCount(impl_->owningGrid_->NoCurrentCellChangeCount() + 1);
        dlg_help_utils::scope_exit const reset_no_current_cell_change_count{[this]() { impl_->owningGrid_->NoCurrentCellChangeCount(impl_->owningGrid_->NoCurrentCellChangeCount() - 1); }};
        if (!ItemsInternal().empty())
        {
            if (impl_->owningGrid_->InDisplayIndexAdjustments())
            {
                // We are within columns display indexes adjustments. We do not allow changing the column collection while adjusting display indexes.
                throw DataGridError::DataGrid::CannotChangeColumnCollectionWhileAdjustingDisplayIndexes();
            }

            impl_->owningGrid_->OnClearingColumns();
            for (auto const& column : ItemsInternal())
            {
                // Detach the column...
                column.as<DataGridColumn>()->OwningGrid(nullptr);
            }

            ItemsInternal().clear();
            DisplayIndexMap().clear();
            AutoGeneratedColumnCount(0);
            impl_->owningGrid_->OnColumnCollectionChanged_PreNotification(false /*columnsGrew*/);
            //impl_->itemsSource_.Clear();
            impl_->visibleEdgedColumnsWidth_ = 0;
            impl_->owningGrid_->OnColumnCollectionChanged_PostNotification(false /*columnsGrew*/);
        }
    }

    void DataGridColumnCollection::InsertItem(int32_t const columnIndex, MiniDumpExplorer::DataGridColumn const& dataGridColumn) const
    {
        assert(impl_->owningGrid_);
        impl_->owningGrid_->NoCurrentCellChangeCount(impl_->owningGrid_->NoCurrentCellChangeCount() + 1);
        dlg_help_utils::scope_exit const reset_no_current_cell_change_count{[this]() { impl_->owningGrid_->NoCurrentCellChangeCount(impl_->owningGrid_->NoCurrentCellChangeCount() - 1); }};
        if (impl_->owningGrid_->InDisplayIndexAdjustments())
        {
            // We are within columns display indexes adjustments. We do not allow changing the column collection while adjusting display indexes.
            throw DataGridError::DataGrid::CannotChangeColumnCollectionWhileAdjustingDisplayIndexes();
        }

        if (!dataGridColumn)
        {
            throw dlg_help_utils::exceptions::wide_runtime_error{L"Argument null: dataGridColumn"};
        }

        auto columnIndexWithFiller = columnIndex;
        if (dataGridColumn != RowGroupSpacerColumn())
        {
            if(auto const internalRowGroupSpacerColumn = RowGroupSpacerColumn().as<DataGridFillerColumn>();
                internalRowGroupSpacerColumn->IsRepresented())
            {
                columnIndexWithFiller++;
            }
        }

        // get the new current cell coordinates
        auto const newCurrentCellCoordinates = impl_->owningGrid_->OnInsertingColumn(columnIndex, dataGridColumn);

        // insert the column into our internal list
        impl_->itemsInternal_.insert(impl_->itemsInternal_.begin() + columnIndexWithFiller, dataGridColumn);
        auto const internalDataGridColumn = dataGridColumn.as<DataGridColumn>();
        internalDataGridColumn->Index(static_cast<int32_t>(columnIndexWithFiller));
        internalDataGridColumn->OwningGrid(*impl_->owningGrid_);
        internalDataGridColumn->RemoveEditingElement();
        if (internalDataGridColumn->IsVisible())
        {
            impl_->visibleEdgedColumnsWidth_ += dataGridColumn.ActualWidth();
        }

        // continue with the base insert
        impl_->owningGrid_->OnInsertedColumn_PreNotification(dataGridColumn);
        impl_->owningGrid_->OnColumnCollectionChanged_PreNotification(true /*columnsGrew*/);

        if (dataGridColumn != RowGroupSpacerColumn())
        {
            //impl_->itemsSource_.InsertAt(static_cast<uint32_t>(columnIndex), dataGridColumn);
        }

        impl_->owningGrid_->OnInsertedColumn_PostNotification(newCurrentCellCoordinates, dataGridColumn.DisplayIndex());
        impl_->owningGrid_->OnColumnCollectionChanged_PostNotification(true /*columnsGrew*/);
    }

    void DataGridColumnCollection::RemoveItem(int32_t const columnIndex)
    {
        RemoveItemPrivate(columnIndex, false /*isSpacer*/);
    }

    void DataGridColumnCollection::RemoveItemPrivate(int32_t const columnIndex, bool const isSpacer)
    {
        assert(impl_->owningGrid_);
        impl_->owningGrid_->NoCurrentCellChangeCount(impl_->owningGrid_->NoCurrentCellChangeCount() + 1);
        dlg_help_utils::scope_exit const reset_no_current_cell_change_count{[this]() { impl_->owningGrid_->NoCurrentCellChangeCount(impl_->owningGrid_->NoCurrentCellChangeCount() - 1); }};
        if (impl_->owningGrid_->InDisplayIndexAdjustments())
        {
            // We are within columns display indexes adjustments. We do not allow changing the column collection while adjusting display indexes.
            throw DataGridError::DataGrid::CannotChangeColumnCollectionWhileAdjustingDisplayIndexes();
        }

        auto columnIndexWithFiller = columnIndex;
        if (!isSpacer && RowGroupSpacerColumn().as<DataGridFillerColumn>()->IsRepresented())
        {
            columnIndexWithFiller++;
        }

        assert(columnIndexWithFiller < ItemsInternal().size());

        auto const dataGridColumn = ItemsInternal()[columnIndexWithFiller];
        auto const internalDataGridColumn = dataGridColumn.as<DataGridColumn>();
        auto const newCurrentCellCoordinates = impl_->owningGrid_->OnRemovingColumn(dataGridColumn);
        ItemsInternal().erase(ItemsInternal().begin() + columnIndex + columnIndexWithFiller);
        if(internalDataGridColumn->IsVisible())
        {
            impl_->visibleEdgedColumnsWidth_ -= dataGridColumn.ActualWidth();
        }

        internalDataGridColumn->OwningGrid(nullptr);
        internalDataGridColumn->RemoveEditingElement();

        // continue with the base remove
        impl_->owningGrid_->OnRemovedColumn_PreNotification(dataGridColumn);
        impl_->owningGrid_->OnColumnCollectionChanged_PreNotification(false /*columnsGrew*/);
        if (!isSpacer)
        {
            //impl_->itemsSource_.RemoveAt(static_cast<uint32_t>(columnIndex));
        }

        impl_->owningGrid_->OnRemovedColumn_PostNotification(newCurrentCellCoordinates);
        impl_->owningGrid_->OnColumnCollectionChanged_PostNotification(false /*columnsGrew*/);
    }

#ifndef NDEBUG
    bool DataGridColumnCollection::Debug_VerifyColumnDisplayIndexes()
    {
        for (int32_t columnDisplayIndex = 0; columnDisplayIndex < ItemsInternal().size(); columnDisplayIndex++)
        {
            if (!GetColumnAtDisplayIndex(columnDisplayIndex))
            {
                return false;
            }
        }

        return true;
    }
#endif
}

