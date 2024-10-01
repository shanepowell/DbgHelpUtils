#include "pch.h"

#include "DataGrid.h"
#include "DataGridBoundColumn.h"
#include "DataGridCell.h"
// ReSharper disable once CppUnusedIncludeDirective
#include "DataGridCheckBoxColumn.h"
#include "DataGridColumn.h"
#include "DataGridError.h"
#include "DataGridFillerColumn.h"
#include "DataGridRow.h"
// ReSharper disable once CppUnusedIncludeDirective
#include "DataGridTextColumn.h"
#include "DbgHelpUtils/double_utils.h"
#include "DbgHelpUtils/exit_scope.h"
#include "DbgHelpUtils/string_compare.h"
#include "Helpers/UIHelper.h"

using namespace winrt;
using namespace Microsoft::UI::Xaml;
using namespace std::string_literals;
using namespace std::string_view_literals;

namespace winrt::MiniDumpExplorer::implementation
{
    namespace
    {
        // Used to work around double arithmetic rounding.
        constexpr double RoundingDelta = 0.0001;
        constexpr int32_t DefaultColumnDisplayOrder = 10000;
    }

    void DataGrid::OnColumnDisplayIndexChanged(MiniDumpExplorer::DataGridColumnEventArgs const& e)
    {
        columnDisplayIndexChangedHandler_(*this, e);
    }

    void DataGrid::OnColumnReordered(MiniDumpExplorer::DataGridColumnEventArgs const& e)
    {
        EnsureVerticalGridLines();
        columnReorderedHandler_(*this, e);
    }

    void DataGrid::OnColumnReordering(MiniDumpExplorer::DataGridColumnReorderingEventArgs const& e)
    {
        columnReorderingHandler_(*this, e);
    }

    void DataGrid::OnColumnSorting(MiniDumpExplorer::DataGridColumnEventArgs const& e)
    {
        if(auto const itemsSource = ItemsSource(); itemsSource)
        {
            itemsSource.Sort(*this, e);
        }
        sortingHandler_(*this, e);
    }

    // Returns the column's width
    double DataGrid::GetEdgedColumnWidth(MiniDumpExplorer::DataGridColumn const& dataGridColumn)
    {
        assert(dataGridColumn);
        return dataGridColumn.ActualWidth();
    }

    double DataGrid::AdjustColumnWidths(int32_t const displayIndex, double amount, bool const userInitiated)
    {
        if (!double_utils::is_zero(amount))
        {
            if (amount < 0)
            {
                amount = DecreaseColumnWidths(displayIndex, amount, userInitiated);
            }
            else
            {
                amount = IncreaseColumnWidths(displayIndex, amount, userInitiated);
            }
        }

        return amount;
    }

    void DataGrid::AutoSizeColumn(MiniDumpExplorer::DataGridColumn const& column, double const desiredWidth)
    {
        assert(column.WidthLength().IsAuto() || column.WidthLength().IsSizeToCells() || column.WidthLength().IsSizeToHeader() || (!UsesStarSizing() && column.WidthLength().IsStar()));

        auto const internalColumn = column.as<DataGridColumn>();

        // ReSharper disable GrammarMistakeInComment
        // If we're using star sizing and this is the first time we've measured this particular auto-column,
        // we want to allow all rows to get measured before we setup the star widths.  We won't know the final
        // desired value of the column until all rows have been measured.  Because of this, we wait until
        // an Arrange occurs before we adjust star widths.
        // ReSharper restore GrammarMistakeInComment
        if (UsesStarSizing() && !internalColumn->IsInitialDesiredWidthDetermined())
        {
            AutoSizingColumns(true);
        }

        // Update the column's DesiredValue if it needs to grow to fit the new desired value
        if (desiredWidth > column.WidthLength().DesiredValue() || std::isnan(column.WidthLength().DesiredValue()))
        {
            // If this auto-growth occurs after the column's initial desired width has been determined,
            // then the growth should act like a resize (squish columns to the right).  Otherwise, if
            // this column is newly added, we'll just set its display value directly.
            if (UsesStarSizing() && internalColumn->IsInitialDesiredWidthDetermined())
            {
                internalColumn->Resize(column.WidthLength().Value(), column.WidthLength().UnitType(), desiredWidth, desiredWidth, false);
            }
            else
            {
                internalColumn->SetWidthInternalNoCallback(MiniDumpExplorer::DataGridLength{ column.WidthLength().Value(), column.WidthLength().UnitType(), desiredWidth, desiredWidth });
                OnColumnWidthChanged(column);
            }
        }
    }

    bool DataGrid::ColumnRequiresRightGridLine(MiniDumpExplorer::DataGridColumn const& dataGridColumn, bool const includeLastRightGridLineWhenPresent)
    {
        return (GridLinesVisibility() == DataGridGridLinesVisibility::Vertical || GridLinesVisibility() == DataGridGridLinesVisibility::All) && VerticalGridLinesBrush() &&
            (dataGridColumn != ColumnsInternal().LastVisibleColumn() || (includeLastRightGridLineWhenPresent && ColumnsInternal().FillerColumn().as<DataGridFillerColumn>()->IsActive()));
    }

    // Decreases the widths of all columns with DisplayIndex >= displayIndex such that the total
    // width is decreased by the given amount, if possible.  If the total desired adjustment amount
    // could not be met, the remaining amount of adjustment is returned.
    double DataGrid::DecreaseColumnWidths(int32_t const displayIndex, double amount, bool const userInitiated)
    {
        // 1. Take space from non-star columns with widths larger than desired widths (left to right).
        amount = DecreaseNonStarColumnWidths(displayIndex, [](MiniDumpExplorer::DataGridColumn const& c) { return c.WidthLength().DesiredValue(); }, amount, false, false);

        // 2. Take space from star columns until they reach their min.
        amount = AdjustStarColumnWidths(displayIndex, amount, userInitiated);

        // 3. Take space from non-star columns that have already been initialized, until they reach their min (right to left).
        amount = DecreaseNonStarColumnWidths(displayIndex, [](MiniDumpExplorer::DataGridColumn const& c) { return c.as<DataGridColumn>()->ActualMinWidth(); }, amount, true, false);

        // 4. Take space from all non-star columns until they reach their min, even if they are new (right to left).
        amount = DecreaseNonStarColumnWidths(displayIndex, [](MiniDumpExplorer::DataGridColumn const& c) { return c.as<DataGridColumn>()->ActualMinWidth(); }, amount, true, true);

        return amount;
    }

    bool DataGrid::GetColumnReadOnlyState(MiniDumpExplorer::DataGridColumn const& dataGridColumn, bool const isReadOnly)
    {
        assert(dataGridColumn);

        if (auto const dataGridBoundColumn = dataGridColumn.try_as<DataGridBoundColumn>();
            dataGridBoundColumn && dataGridBoundColumn->Binding())
        {
            std::wstring path{};
            if (dataGridBoundColumn->Binding().Path())
            {
                path = dataGridBoundColumn->Binding().Path().Path();
            }

            if (!path.empty())
            {
                return DataConnection().GetPropertyIsReadOnly(path) || isReadOnly;
            }
        }

        return isReadOnly;
    }

    // Increases the widths of all columns with DisplayIndex >= displayIndex such that the total
    // width is increased by the given amount, if possible.  If the total desired adjustment amount
    // could not be met, the remaining amount of adjustment is returned.
    double DataGrid::IncreaseColumnWidths(int32_t const displayIndex, double amount, bool const userInitiated)
    {
        // 1. Give space to non-star columns that are smaller than their desired widths (left to right).
        amount = IncreaseNonStarColumnWidths(displayIndex, [](MiniDumpExplorer::DataGridColumn const& c) { return c.WidthLength().DesiredValue(); }, amount, false, false);

        // 2. Give space to star columns until they reach their max.
        amount = AdjustStarColumnWidths(displayIndex, amount, userInitiated);

        // 3. Give space to non-star columns that have already been initialized, until they reach their max (right to left).
        amount = IncreaseNonStarColumnWidths(displayIndex, [](MiniDumpExplorer::DataGridColumn const& c) { return c.as<DataGridColumn>()->ActualMaxWidth(); }, amount, true, false);

        // 4. Give space to all non-star columns until they reach their max, even if they are new (right to left).
        amount = IncreaseNonStarColumnWidths(displayIndex, [](MiniDumpExplorer::DataGridColumn const& c) { return c.as<DataGridColumn>()->ActualMaxWidth(); }, amount, true, false);

        return amount;
    }

    void DataGrid::OnClearingColumns()
    {
        // Rows need to be cleared first. There cannot be rows without also having columns.
        ClearRows(false);

        // Removing all the column header cells
        RemoveDisplayedColumnHeaders();

        horizontalOffset_ = negHorizontalOffset_ = 0;

        if (hScrollBar_ && hScrollBar_.Visibility() == Visibility::Visible)
        {
            hScrollBar_.Value(0);
        }
    }

    // Invalidates the widths of all columns because the resizing behavior of an individual column has changed.
    void DataGrid::OnColumnCanUserResizeChanged(MiniDumpExplorer::DataGridColumn const& column)
    {
        if (column.as<DataGridColumn>()->IsVisible())
        {
            EnsureHorizontalLayout();
        }
    }

    void DataGrid::OnColumnCellStyleChanged(MiniDumpExplorer::DataGridColumn const& column, Microsoft::UI::Xaml::Style const& previousStyle)
    {
        auto const internalColumn = column.as<DataGridColumn>();

        // Set HeaderCell.Style for displayed rows if HeaderCell.Style is not already set
        for (auto const& row : GetAllRows())
        {
            auto const internalRow = row.as<DataGridRow>();
            internalRow->Cells()[internalColumn->Index()].as<DataGridCell>()->EnsureStyle(previousStyle);
        }

        InvalidateRowHeightEstimate();
    }

    void DataGrid::OnColumnCollectionChanged_PostNotification(bool const columnsGrew)
    {
        if (columnsGrew &&
            CurrentColumnIndex() == InvalidIndex)
        {
            MakeFirstDisplayedCellCurrentCell();
        }

        if (autoGeneratingColumnOperationCount_ == 0)
        {
            EnsureRowsPresenterVisibility();
            InvalidateRowHeightEstimate();
        }
    }

    void DataGrid::OnColumnCollectionChanged_PreNotification(bool const columnsGrew)
    {
        // dataGridColumn==null means the collection was refreshed.
        if (columnsGrew && autoGeneratingColumnOperationCount_ == 0 && ColumnsItemsInternal().size() == 1)
        {
            RefreshRows(false /*recycleRows*/, true /*clearRows*/);
        }
        else
        {
            InvalidateMeasure();
        }
    }

    void DataGrid::OnColumnDisplayIndexChanged(MiniDumpExplorer::DataGridColumn const& dataGridColumn)
    {
        assert(dataGridColumn);
        MiniDumpExplorer::DataGridColumnEventArgs const e{ dataGridColumn };

        // Call protected method to raise event
        if (dataGridColumn != ColumnsInternal().RowGroupSpacerColumn())
        {
            OnColumnDisplayIndexChanged(e);
        }
    }

    void DataGrid::OnColumnDisplayIndexChanged_PostNotification()
    {
        // Notifications for adjusted display indexes.
        FlushDisplayIndexChanged(true /*raiseEvent*/);

        // Our displayed columns may have changed so recompute them
        UpdateDisplayedColumns();

        // Invalidate layout
        CorrectColumnFrozenStates();
        EnsureHorizontalLayout();
    }
    void DataGrid::OnColumnDisplayIndexChanging(MiniDumpExplorer::DataGridColumn const& targetColumn, int32_t const newDisplayIndex)
    {
        auto const internalTargetColumn = targetColumn.as<DataGridColumn>();

        assert(targetColumn);
        assert(newDisplayIndex != internalTargetColumn->DisplayIndexWithFiller());

        if (InDisplayIndexAdjustments())
        {
            // We are within columns display indexes adjustments. We do not allow changing display indexes while adjusting them.
            throw DataGridError::DataGrid::CannotChangeColumnCollectionWhileAdjustingDisplayIndexes();
        }

        InDisplayIndexAdjustments(true);

        auto handle = dlg_help_utils::make_scope_exit([this]()
            {
                InDisplayIndexAdjustments(false);
            });


        auto const trackChange = targetColumn != ColumnsInternal().RowGroupSpacerColumn();

        // Move is legal - let's adjust the affected display indexes.
        if (newDisplayIndex < internalTargetColumn->DisplayIndexWithFiller())
        {
            // DisplayIndex decreases. All columns with newDisplayIndex <= DisplayIndex < targetColumn.DisplayIndex
            // get their DisplayIndex incremented.
            for (auto i = newDisplayIndex; i < internalTargetColumn->DisplayIndexWithFiller(); ++i)
            {
                auto const column = ColumnsInternal().GetColumnAtDisplayIndex(i).as<DataGridColumn>();
                column->DisplayIndexWithFiller(column->DisplayIndexWithFiller() + 1);
                if (trackChange)
                {
                    column->DisplayIndexHasChanged(true); // OnColumnDisplayIndexChanged needs to be raised later on
                }
            }
        }
        else
        {
            // DisplayIndex increases. All columns with targetColumn.DisplayIndex < DisplayIndex <= newDisplayIndex
            // get their DisplayIndex decremented.
            for (int32_t i = newDisplayIndex; i > internalTargetColumn->DisplayIndexWithFiller(); --i)
            {
                auto const column = ColumnsInternal().GetColumnAtDisplayIndex(i).as<DataGridColumn>();
                column->DisplayIndexWithFiller(column->DisplayIndexWithFiller() - 1);
                if (trackChange)
                {
                    column->DisplayIndexHasChanged(true); // OnColumnDisplayIndexChanged needs to be raised later on
                }
            }
        }

        // Now let's actually change the order of the DisplayIndexMap
        auto& displayIndexMap = ColumnsInternal().DisplayIndexMap();
        auto const index = internalTargetColumn->Index();
        if (internalTargetColumn->DisplayIndexWithFiller() != InvalidIndex)
        {
            if (auto const it = std::ranges::find(displayIndexMap, index);
                it != displayIndexMap.end())
            {
                displayIndexMap.erase(it);
            }
        }

        displayIndexMap.insert(displayIndexMap.begin() + newDisplayIndex, index);

        // Note that displayIndex of moved column is updated by caller.
    }

    void DataGrid::OnColumnBindingChanged(MiniDumpExplorer::DataGridBoundColumn const& column)
    {
        // Update Binding in Displayed rows by regenerating the affected elements
        if (rowsPresenter_)
        {
            for (auto const& row : GetAllRows())
            {
                PopulateCellContent(false /*isCellEdited*/, column, row, row.as<DataGridRow>()->Cells()[column.as<DataGridBoundColumn>()->Index()]);
            }
        }
    }

    void DataGrid::OnColumnElementStyleChanged(MiniDumpExplorer::DataGridBoundColumn const& column)
    {
        // Update Element Style in Displayed rows
        for (auto const& row : GetAllRows())
        {
            if (auto const element = column.GetCellContent(row);
                element)
            {
                UIHelper::SetStyleWithType(element, column.ElementStyle());
            }
        }

        InvalidateRowHeightEstimate();
    }

    void DataGrid::OnColumnHeaderDragStarted(Controls::Primitives::DragStartedEventArgs const& e)
    {
        if (columnHeaderDragStartedHandler_)
        {
            columnHeaderDragStartedHandler_(*this, e);
        }
    }

    void DataGrid::OnColumnHeaderDragDelta(Controls::Primitives::DragDeltaEventArgs const& e)
    {
        if (columnHeaderDragDeltaHandler_)
        {
            columnHeaderDragDeltaHandler_(*this, e);
        }
    }

    void DataGrid::OnColumnHeaderDragCompleted(Controls::Primitives::DragCompletedEventArgs const& e)
    {
        if (columnHeaderDragCompletedHandler_)
        {
            columnHeaderDragCompletedHandler_(*this, e);
        }
    }

    void DataGrid::OnColumnMaxWidthChanged(MiniDumpExplorer::DataGridColumn const& column, double const oldValue)
    {
        assert(column);

        if (auto const internalColumn = column.as<DataGridColumn>();
            internalColumn->Visibility() == Visibility::Visible && oldValue != internalColumn->ActualMaxWidth())
        {
            if (internalColumn->ActualMaxWidth() < internalColumn->WidthLength().DisplayValue())
            {
                // If the maximum width has caused the column to decrease in size, try first to resize
                // the columns to the right to make up for the difference in width, but don't limit the column's
                // final display value to how much they could be resized.
                AdjustColumnWidths(internalColumn->DisplayIndex() + 1, internalColumn->WidthLength().DisplayValue() - internalColumn->ActualMaxWidth(), false);
                internalColumn->SetWidthDisplayValue(internalColumn->ActualMaxWidth());
            }
            else if (internalColumn->WidthLength().DisplayValue() == oldValue && internalColumn->WidthLength().DesiredValue() > internalColumn->WidthLength().DisplayValue())
            {
                // If the column was previously limited by its maximum value but has more room now,
                // attempt to resize the column to its desired width.
                internalColumn->Resize(internalColumn->WidthLength().Value(), internalColumn->WidthLength().UnitType(), internalColumn->WidthLength().DesiredValue(), internalColumn->WidthLength().DesiredValue(), false);
            }

            OnColumnWidthChanged(column);
        }
    }

    void DataGrid::OnColumnMinWidthChanged(MiniDumpExplorer::DataGridColumn const& column, double const oldValue)
    {
        assert(column);

        if (auto const internalColumn = column.as<DataGridColumn>();
            internalColumn->Visibility() == Visibility::Visible && oldValue != internalColumn->ActualMinWidth())
        {
            if (internalColumn->ActualMinWidth() > internalColumn->WidthLength().DisplayValue())
            {
                // If the minimum width has caused the column to increase in size, try first to resize
                // the columns to the right to make up for the difference in width, but don't limit the column's
                // final display value to how much they could be resized.
                AdjustColumnWidths(internalColumn->DisplayIndex() + 1, internalColumn->WidthLength().DisplayValue() - internalColumn->ActualMinWidth(), false);
                internalColumn->SetWidthDisplayValue(internalColumn->ActualMinWidth());
            }
            else if (internalColumn->WidthLength().DisplayValue() == oldValue && internalColumn->WidthLength().DesiredValue() < internalColumn->WidthLength().DisplayValue())
            {
                // If the column was previously limited by its minimum value but can be smaller now,
                // attempt to resize the column to its desired width.
                internalColumn->Resize(internalColumn->WidthLength().Value(), internalColumn->WidthLength().UnitType(), internalColumn->WidthLength().DesiredValue(), internalColumn->WidthLength().DesiredValue(), false);
            }

            OnColumnWidthChanged(column);
        }
    }

    void DataGrid::OnColumnReadOnlyStateChanging(MiniDumpExplorer::DataGridColumn const& dataGridColumn, bool const isReadOnly)
    {
        assert(dataGridColumn);
        if (auto const internalDataGridColumn = dataGridColumn.as<DataGridColumn>();
            isReadOnly && CurrentColumnIndex() == internalDataGridColumn->Index())
        {
            // Edited column becomes read-only. Exit editing mode.
            if (!EndCellEdit(DataGridEditAction::Commit, true /*exitEditingMode*/, ContainsFocus() /*keepFocus*/, true /*raiseEvents*/))
            {
                EndCellEdit(DataGridEditAction::Cancel, true /*exitEditingMode*/, ContainsFocus() /*keepFocus*/, false /*raiseEvents*/);
            }
        }
    }

    void DataGrid::OnColumnVisibleStateChanged(MiniDumpExplorer::DataGridColumn const& updatedColumn)
    {
        assert(updatedColumn);

        CorrectColumnFrozenStates();
        UpdateDisplayedColumns();
        EnsureRowsPresenterVisibility();
        EnsureHorizontalLayout();
        InvalidateColumnHeadersMeasure();

        auto const internalUpdatedColumn = updatedColumn.as<DataGridColumn>();
        if (internalUpdatedColumn->IsVisible() &&
            ColumnsInternal().VisibleColumnCount() == 1 && CurrentColumnIndex() == InvalidIndex)
        {
            assert(SelectedIndex() == DataConnection().IndexOf(SelectedItem()));
            if (SelectedIndex() != InvalidIndex)
            {
                SetAndSelectCurrentCell(internalUpdatedColumn->Index(), SelectedIndex(), true /*forceCurrentCellSelection*/);
            }
            else
            {
                MakeFirstDisplayedCellCurrentCell();
            }
        }

        // We need to explicitly collapse the cells of the invisible column because layout only goes through
        // visible ones
        if (updatedColumn.Visibility() == Visibility::Collapsed)
        {
            for (auto const& row : GetAllRows())
            {
                row.as<DataGridRow>()->Cells()[internalUpdatedColumn->Index()].Visibility(Visibility::Collapsed);
            }
        }
    }

    void DataGrid::OnColumnVisibleStateChanging(MiniDumpExplorer::DataGridColumn const& targetColumn)
    {
        assert(targetColumn);

        if (auto const internalTargetColumn = targetColumn.as<DataGridColumn>();
            internalTargetColumn->IsVisible() && CurrentColumn() == targetColumn)
        {
            // Column of the current cell is made invisible. Trying to move the current cell to a neighbor column. May throw an exception.
            auto dataGridColumn = ColumnsInternal().GetNextVisibleColumn(targetColumn);
            if (!dataGridColumn)
            {
                dataGridColumn = ColumnsInternal().GetPreviousVisibleNonFillerColumn(targetColumn);
            }

            if (!dataGridColumn)
            {
                SetCurrentCellCore(InvalidIndex, InvalidIndex);
            }
            else
            {
                SetCurrentCellCore(dataGridColumn.as<DataGridColumn>()->Index(), CurrentSlot());
            }
        }
    }

    void DataGrid::OnColumnWidthChanged(MiniDumpExplorer::DataGridColumn const& updatedColumn)
    {
        assert(updatedColumn);
        if (updatedColumn.as<DataGridColumn>()->IsVisible())
        {
            EnsureHorizontalLayout();
        }
    }

    void DataGrid::OnFillerColumnWidthNeeded(double const finalWidth)
    {
        auto const fillerColumn = ColumnsInternal().FillerColumn().as<DataGridFillerColumn>();
        auto const totalColumnsWidth = ColumnsInternal().VisibleEdgedColumnsWidth();
        fillerColumn->FillerWidth(finalWidth - totalColumnsWidth > RoundingDelta ? finalWidth - totalColumnsWidth : 0);
    }

    void DataGrid::OnInsertedColumn_PostNotification(DataGridInternal::DataGridCellCoordinates const& newCurrentCellCoordinates, int32_t const newDisplayIndex)
    {
        // Update current cell if needed
        if (newCurrentCellCoordinates.ColumnIndex != InvalidIndex)
        {
            assert(CurrentColumnIndex() == InvalidIndex);
            SetAndSelectCurrentCell(
                newCurrentCellCoordinates.ColumnIndex,
                newCurrentCellCoordinates.Slot,
                ColumnsInternal().VisibleColumnCount() == 1 /*forceCurrentCellSelection*/);

            if (newDisplayIndex < FrozenColumnCountWithFiller())
            {
                CorrectColumnFrozenStates();
            }
        }
    }

    void DataGrid::OnInsertedColumn_PreNotification(MiniDumpExplorer::DataGridColumn const& insertedColumn)
    {
        // Fix the Index of all following columns
        CorrectColumnIndexesAfterInsertion(insertedColumn, 1);

        auto const internalInsertedColumn = insertedColumn.as<DataGridColumn>();
        assert(internalInsertedColumn->Index() != InvalidIndex);
        assert(internalInsertedColumn->Index() < ColumnsItemsInternal().size());
        assert(internalInsertedColumn->OwningGrid() == *this);

        CorrectColumnDisplayIndexesAfterInsertion(insertedColumn);

        InsertDisplayedColumnHeader(insertedColumn);

        // Insert the missing data cells
        if (SlotCount() > 0)
        {
            auto const newColumnCount = ColumnsItemsInternal().size();

            for (auto const& row : GetAllRows())
            {
                if (auto const internalRow = row.as<DataGridRow>();
                    internalRow->Cells().Count() < newColumnCount)
                {
                    AddNewCellPrivate(row, insertedColumn);
                }
            }
        }

        if (internalInsertedColumn->IsVisible())
        {
            EnsureHorizontalLayout();
        }

        if (auto const boundColumn = insertedColumn.try_as<DataGridBoundColumn>();
            boundColumn && !boundColumn->IsAutoGenerated())
        {
            boundColumn->SetHeaderFromBinding();
        }
    }

    DataGridInternal::DataGridCellCoordinates DataGrid::OnInsertingColumn(int32_t const columnIndexInserted, MiniDumpExplorer::DataGridColumn const& insertColumn)
    {
        auto const internalInsertColumn = insertColumn.as<DataGridColumn>();
        assert(insertColumn);

        if (internalInsertColumn->OwningGrid() && insertColumn != ColumnsInternal().RowGroupSpacerColumn())
        {
            throw DataGridError::DataGrid::ColumnCannotBeReassignedToDifferentDataGrid();
        }

        // Reset current cell if there is one, no matter the relative position of the columns involved
        if (CurrentColumnIndex() != InvalidIndex)
        {
            temporarilyResetCurrentCell_ = true;
            auto const columnIndex = columnIndexInserted <= CurrentColumnIndex() ? CurrentColumnIndex() + 1 : CurrentColumnIndex();
            auto const slot = CurrentSlot();
            ResetCurrentCellCore();
            return DataGridInternal::DataGridCellCoordinates{ .ColumnIndex= columnIndex, .Slot= slot};
        }

        return DataGridInternal::DataGridCellCoordinates{ .ColumnIndex= InvalidIndex, .Slot= InvalidIndex };
    }

    void DataGrid::OnRemovedColumn_PostNotification(DataGridInternal::DataGridCellCoordinates const& newCurrentCellCoordinates)
    {
        // Update current cell if needed
        if (newCurrentCellCoordinates.ColumnIndex != InvalidIndex)
        {
            assert(CurrentColumnIndex() == InvalidIndex);
            SetAndSelectCurrentCell(newCurrentCellCoordinates.ColumnIndex, newCurrentCellCoordinates.Slot, false /*forceCurrentCellSelection*/);
        }
    }

    void DataGrid::OnRemovedColumn_PreNotification(MiniDumpExplorer::DataGridColumn const& removedColumn)
    {
        auto const internalRemovedColumn = removedColumn.as<DataGridColumn>();
        assert(internalRemovedColumn->Index() != InvalidIndex);
        assert(!internalRemovedColumn->OwningGrid());

        // Intentionally keep the DisplayIndex intact after detaching the column.
        CorrectColumnIndexesAfterDeletion(removedColumn);

        CorrectColumnDisplayIndexesAfterDeletion(removedColumn);

        // If the detached column was frozen, a new column needs to take its place
        if (removedColumn.IsFrozen())
        {
            internalRemovedColumn->IsFrozen(false);
            CorrectColumnFrozenStates();
        }

        UpdateDisplayedColumns();

        // Fix the existing rows by removing cells at correct index
        auto const newColumnCount = ColumnsItemsInternal().size();

        if (rowsPresenter_)
        {
            for (auto const& row : GetAllRows())
            {
                if (auto const internalRow = row.as<DataGridRow>();
                    internalRow->Cells().Count() > newColumnCount)
                {
                    internalRow->Cells().RemoveAt(internalRemovedColumn->Index());
                }
            }

            rowsPresenter_.InvalidateArrange();
        }

        RemoveDisplayedColumnHeader(removedColumn);
    }

    DataGridInternal::DataGridCellCoordinates DataGrid::OnRemovingColumn(MiniDumpExplorer::DataGridColumn const& dataGridColumn)
    {
        assert(dataGridColumn);

        auto const internalDataGridColumn = dataGridColumn.as<DataGridColumn>();
        assert(internalDataGridColumn->Index() != InvalidIndex);
        assert(internalDataGridColumn->Index() < ColumnsItemsInternal().size());

        DataGridInternal::DataGridCellCoordinates newCurrentCellCoordinates{ .ColumnIndex= InvalidIndex, .Slot= InvalidIndex };

        temporarilyResetCurrentCell_ = false;
        auto const columnIndex = internalDataGridColumn->Index();

        // Reset the current cell's address if there is one.
        if (CurrentColumnIndex() != InvalidIndex)
        {
            auto newCurrentColumnIndex = CurrentColumnIndex();
            if (columnIndex == newCurrentColumnIndex)
            {
                if (auto const dataGridColumnNext = ColumnsInternal().GetNextVisibleColumn(ColumnsItemsInternal()[columnIndex]).try_as<DataGridColumn>();
                    dataGridColumnNext)
                {
                    if (dataGridColumnNext->Index() > columnIndex)
                    {
                        newCurrentColumnIndex = dataGridColumnNext->Index() - 1;
                    }
                    else
                    {
                        newCurrentColumnIndex = dataGridColumnNext->Index();
                    }
                }
                else if (auto const dataGridColumnPrevious = ColumnsInternal().GetPreviousVisibleNonFillerColumn(ColumnsItemsInternal()[columnIndex]).try_as<DataGridColumn>();
                    dataGridColumnPrevious)
                {
                    if (dataGridColumnPrevious->Index() > columnIndex)
                    {
                        newCurrentColumnIndex = dataGridColumnPrevious->Index() - 1;
                    }
                    else
                    {
                        newCurrentColumnIndex = dataGridColumnPrevious->Index();
                    }
                }
                else
                {
                    newCurrentColumnIndex = InvalidIndex;
                }
            }
            else if (columnIndex < newCurrentColumnIndex)
            {
                newCurrentColumnIndex--;
            }

            newCurrentCellCoordinates.ColumnIndex = newCurrentColumnIndex;
            newCurrentCellCoordinates.Slot = newCurrentColumnIndex == InvalidIndex ? InvalidIndex : CurrentSlot();
            if (columnIndex == CurrentColumnIndex())
            {
                // If the commit fails, force a cancel edit
                if (!CommitEdit(DataGridEditingUnit::Row, false /*exitEditingMode*/))
                {
                    CancelEdit(DataGridEditingUnit::Row, false /*raiseEvents*/);
                }
            }
            else
            {
                // Underlying data of deleted column is gone. It cannot be accessed any more.
                // Do not end editing mode so that CellValidation doesn't get raised, since that event needs the current formatted value.
                temporarilyResetCurrentCell_ = true;
            }

            [[maybe_unused]] auto const success = SetCurrentCellCore(InvalidIndex, InvalidIndex);
            assert(success);
        }

        // If the last column is removed, delete all the rows first.
        if (ColumnsItemsInternal().size() == 1)
        {
            ClearRows(false);
        }

        // Is deleted column scrolled off-screen?
        if (internalDataGridColumn->IsVisible() &&
            !internalDataGridColumn->IsFrozen() &&
            DisplayData().FirstDisplayedScrollingCol() != InvalidIndex)
        {
            // Deleted column is part of scrolling columns.
            if (DisplayData().FirstDisplayedScrollingCol() == internalDataGridColumn->Index())
            {
                // Deleted column is first scrolling column
                horizontalOffset_ -= negHorizontalOffset_;
                negHorizontalOffset_ = 0;
            }
            else if (!ColumnsInternal().DisplayInOrder(DisplayData().FirstDisplayedScrollingCol(), internalDataGridColumn->Index()))
            {
                // Deleted column is displayed before first scrolling column
                assert(horizontalOffset_ >= GetEdgedColumnWidth(dataGridColumn));
                horizontalOffset_ -= GetEdgedColumnWidth(dataGridColumn);
            }

            if (hScrollBar_ && hScrollBar_.Visibility() == Visibility::Visible)
            {
                hScrollBar_.Value(horizontalOffset_);
            }
        }

        return newCurrentCellCoordinates;
    }

    // Called when a column property changes, and its cells need to adjust that column change.
    void DataGrid::RefreshColumnElements(MiniDumpExplorer::DataGridColumn const& dataGridColumn, std::wstring_view const& propertyName)
    {
        assert(dataGridColumn);

        // Take care of the non-displayed loaded rows
        for (auto const& dataGridRow : loadedRows_)
        {
            assert(dataGridRow);
            if (!IsSlotVisible(dataGridRow.as<DataGridRow>()->Slot()))
            {
                RefreshCellElement(dataGridColumn, dataGridRow, propertyName);
            }
        }

        // Take care of the displayed rows
        if (rowsPresenter_)
        {
            for (auto const& row : GetAllRows())
            {
                RefreshCellElement(dataGridColumn, row, propertyName);
            }

            // This update could change layout, so we need to update our estimate and invalidate
            InvalidateRowHeightEstimate();
            InvalidateMeasure();
        }
    }

    // Decreases the width of a non-star column by the given amount, if possible.  If the total desired
    // adjustment amount could not be met, the remaining amount of adjustment is returned.  The adjustment
    // stops when the column's target width has been met.
    double DataGrid::DecreaseNonStarColumnWidth(MiniDumpExplorer::DataGridColumn const& column, double const targetWidth, double const amount)
    {
        auto const internalColumn = column.as<DataGridColumn>();
        assert(amount < 0);
        assert(internalColumn->WidthLength().UnitType() != DataGridLengthUnitType::Star);

        if (double_utils::greater_than_or_close(targetWidth, internalColumn->WidthLength().DisplayValue()))
        {
            return amount;
        }

        auto const adjustment = std::max(
            internalColumn->ActualMinWidth() - internalColumn->WidthLength().DisplayValue(),
            std::max(targetWidth - internalColumn->WidthLength().DisplayValue(), amount));

        internalColumn->SetWidthDisplayValue(internalColumn->WidthLength().DisplayValue() + adjustment);
        return amount - adjustment;
    }

    MiniDumpExplorer::DataGridAutoGeneratingColumnEventArgs DataGrid::GenerateColumn(std::wstring_view const& typeName, std::wstring_view const& propertyName, std::wstring_view const& header)
    {
        // Create a new DataBoundColumn for the Property
        auto const newColumn = GetDataGridColumnFromType(typeName);
        Data::Binding const binding{};
        binding.Path(PropertyPath{ propertyName });
        newColumn.Binding(binding);
        newColumn.Header(box_value(hstring{header}));
        newColumn.as<DataGridBoundColumn>()->IsAutoGenerated(true);
        return MiniDumpExplorer::DataGridAutoGeneratingColumnEventArgs{ propertyName, newColumn };
    }

    MiniDumpExplorer::DataGridBoundColumn DataGrid::GetDataGridColumnFromType(std::wstring_view const& typeName)
    {
        if (dlg_help_utils::string_utils::iequals(L"bool"sv, typeName))
        {
            return MiniDumpExplorer::DataGridCheckBoxColumn{};
        }

        if (dlg_help_utils::string_utils::iequals(L"std::optional<bool>"sv, typeName))
        {
            MiniDumpExplorer::DataGridCheckBoxColumn column{};
            column.IsThreeState(true);
            return column;
        }

        return MiniDumpExplorer::DataGridTextColumn{};
    }

    // Increases the width of a non-star column by the given amount, if possible.  If the total desired
    // adjustment amount could not be met, the remaining amount of adjustment is returned.  The adjustment
    // stops when the column's target width has been met.
    double DataGrid::IncreaseNonStarColumnWidth(MiniDumpExplorer::DataGridColumn const& column, double const targetWidth, double const amount)
    {
        auto const internalColumn = column.as<DataGridColumn>();
        assert(amount > 0);
        assert(internalColumn->WidthLength().UnitType() != DataGridLengthUnitType::Star);

        if (targetWidth <= internalColumn->WidthLength().DisplayValue())
        {
            return amount;
        }

        auto const adjustment = std::min(
            internalColumn->ActualMaxWidth() - internalColumn->WidthLength().DisplayValue(),
            std::min(targetWidth - internalColumn->WidthLength().DisplayValue(), amount));

        internalColumn->SetWidthDisplayValue(internalColumn->WidthLength().DisplayValue() + adjustment);
        return amount - adjustment;
    }

    void DataGrid::RefreshCellElement(MiniDumpExplorer::DataGridColumn const& dataGridColumn, MiniDumpExplorer::DataGridRow const& dataGridRow, std::wstring_view const& propertyName)
    {
        assert(dataGridColumn);
        assert(dataGridRow);

        auto const internalDataGridRow = dataGridRow.as<DataGridRow>();
        auto const internalDataGridColumn = dataGridColumn.as<DataGridColumn>();

        auto const dataGridCell = internalDataGridRow->Cells()[internalDataGridColumn->Index()];
        assert(dataGridCell);

        if (auto const element = dataGridCell.Content().try_as<FrameworkElement>();
            element)
        {
            internalDataGridColumn->RefreshCellContent(element, internalDataGridRow->ComputedForeground(), propertyName);
        }
    }

    bool DataGrid::AddGeneratedColumn(MiniDumpExplorer::DataGridAutoGeneratingColumnEventArgs const& e)
    {
        // Raise the AutoGeneratingColumn event in case the user wants to Cancel or Replace the
        // column being generated
        OnAutoGeneratingColumn(e);
        if (e.Cancel())
        {
            return false;
        }

        if (e.Column())
        {
            // Set the IsAutoGenerated flag here in case the user provides a custom auto-generated column
            e.Column().as<DataGridColumn>()->IsAutoGenerated(true);
        }

        ColumnsInternal().ObservableItems().Append(e.Column());
        ColumnsInternal().AutoGeneratedColumnCount(ColumnsInternal().AutoGeneratedColumnCount() + 1);
        return true;
    }

    // Adjusts the widths of all star columns with DisplayIndex >= displayIndex such that the total
    // width is adjusted by the given amount, if possible.  If the total desired adjustment amount
    // could not be met, the remaining amount of adjustment is returned.
    double DataGrid::AdjustStarColumnWidths(int32_t const displayIndex, double const adjustment, bool const userInitiated)
    {
        auto remainingAdjustment = adjustment;
        if (double_utils::is_zero(remainingAdjustment))
        {
            return remainingAdjustment;
        }

        auto const increase = remainingAdjustment > 0;

        // Make an initial pass through the star columns to total up some values.
        auto scaleStarWeights = false;
        auto totalStarColumnsWidth = 0.0;
        auto totalStarColumnsWidthLimit = 0.0;
        auto totalStarWeights = 0.0;
        std::vector<MiniDumpExplorer::DataGridColumn> starColumns;
        for(auto const& column : ColumnsInternal().GetDisplayedColumns([userInitiated](MiniDumpExplorer::DataGridColumn const& column)
            {
                auto const& internalColumn = column.as<DataGridColumn>();
                return internalColumn->WidthLength().IsStar() && internalColumn->IsVisible() && (internalColumn->ActualCanUserResize() || !userInitiated);
            }))
        {
            auto const& internalColumn = column.as<DataGridColumn>();
            if (internalColumn->DisplayIndex() < displayIndex)
            {
                scaleStarWeights = true;
                continue;
            }

            starColumns.push_back(column);
            totalStarWeights += internalColumn->WidthLength().Value();
            totalStarColumnsWidth += internalColumn->WidthLength().DisplayValue();
            totalStarColumnsWidthLimit += increase ? internalColumn->ActualMaxWidth() : internalColumn->ActualMinWidth();
        }

        // Set the new desired widths according to how much all the star columns can be adjusted without any
        // of them being limited by their minimum or maximum widths (as that would distort their ratios).
        auto adjustmentLimit = totalStarColumnsWidthLimit - totalStarColumnsWidth;
        adjustmentLimit = increase ? std::min(adjustmentLimit, adjustment) : std::max(adjustmentLimit, adjustment);
        for(auto const& starColumn : starColumns)
        {
            auto const& internalStarColumn = starColumn.as<DataGridColumn>();
            internalStarColumn->SetWidthDesiredValue((totalStarColumnsWidth + adjustmentLimit) * internalStarColumn->WidthLength().Value() / totalStarWeights);
        }

        // Adjust the star column widths first towards their desired values, and then towards their limits.
        remainingAdjustment = AdjustStarColumnWidths(displayIndex, remainingAdjustment, userInitiated, [](MiniDumpExplorer::DataGridColumn const& column) { return column.WidthLength().DesiredValue(); });
        remainingAdjustment = AdjustStarColumnWidths(displayIndex, remainingAdjustment, userInitiated, [increase](MiniDumpExplorer::DataGridColumn const& column)
            {
                auto const& internalColumn = column.as<DataGridColumn>();
                return increase ? internalColumn->ActualMaxWidth() : internalColumn->ActualMinWidth();
            });

        // Set the new star value weights according to how much the total column widths have changed.
        // Only do this if there were other star columns to the left, though.  If there weren't any then that means
        // all the star columns were adjusted at the same time, and therefore, their ratios have not changed.
        if (scaleStarWeights)
        {
            auto const starRatio = (totalStarColumnsWidth + adjustment - remainingAdjustment) / totalStarColumnsWidth;
            for(auto const& starColumn : starColumns)
            {
                auto const& internalStarColumn = starColumn.as<DataGridColumn>();
                internalStarColumn->SetWidthStarValue(std::min(std::numeric_limits<double>::max(), starRatio * internalStarColumn->WidthLength().Value()));
            }
        }

        return remainingAdjustment;
    }

    // Adjusts the widths of all star columns with DisplayIndex >= displayIndex such that the total
    // width is adjusted by the given amount, if possible.  If the total desired adjustment amount
    // could not be met, the remaining amount of adjustment is returned.  The columns will stop adjusting
    // once they hit their target widths.
    double DataGrid::AdjustStarColumnWidths(int32_t displayIndex, double remainingAdjustment, bool userInitiated, std::function<double(MiniDumpExplorer::DataGridColumn const&)> const& targetWidth)
    {
        if (double_utils::is_zero(remainingAdjustment))
        {
            return remainingAdjustment;
        }

        auto const increase = remainingAdjustment > 0;

        auto totalStarWeights = 0.0;
        auto totalStarColumnsWidth = 0.0;

        // Order the star columns according to which one will hit their target width (or min/max limit) first.
        // Each KeyValuePair represents a column (as the key) and an ordering factor (as the value).  The ordering factor
        // is computed based on the distance from each column's current display width to its target width.  Because each column
        // could have different star ratios, though, this distance is then adjusted according to its star value.  A column with
        // a larger star value, for example, will change size more rapidly than a column with a lower star value.
        std::vector<std::tuple<MiniDumpExplorer::DataGridColumn, double>> starColumnPairs;
        for(auto const& column : ColumnsInternal().GetDisplayedColumns(
            [displayIndex, userInitiated](MiniDumpExplorer::DataGridColumn const& column)
                {
                    auto const& internalColumn = column.as<DataGridColumn>();
                    return internalColumn->WidthLength().IsStar() && internalColumn->DisplayIndex() >= displayIndex && internalColumn->IsVisible() && internalColumn->WidthLength().Value() > 0 && (internalColumn->ActualCanUserResize() || !userInitiated);
                }))
        {
            auto const& internalColumn = column.as<DataGridColumn>();
            int32_t insertIndex = 0;
            auto distanceToTarget = std::min(internalColumn->ActualMaxWidth(), std::max(targetWidth(column), internalColumn->ActualMinWidth())) - internalColumn->WidthLength().DisplayValue();
            auto factor = (increase ? std::max(0.0, distanceToTarget) : std::min(0.0, distanceToTarget)) / internalColumn->WidthLength().Value();
            for(auto const& [starColumn, starFactor] : starColumnPairs)
            {
                if (increase ? factor <= starFactor : factor >= starFactor)
                {
                    break;
                }

                ++insertIndex;
            }

            starColumnPairs.insert(starColumnPairs.begin() + insertIndex, std::make_tuple(column, factor));
            totalStarWeights += internalColumn->WidthLength().Value();
            totalStarColumnsWidth += internalColumn->WidthLength().DisplayValue();
        }

        // Adjust the column widths one at a time until they either hit their individual target width
        // or the total remaining amount to adjust has been depleted.
        for (auto const& [starColumn, starFactor] : starColumnPairs)
        {
            auto const& internalStarColumn = starColumn.as<DataGridColumn>();
            auto const distanceToTarget = starFactor * internalStarColumn->WidthLength().Value();
            auto const distanceAvailable = internalStarColumn->WidthLength().Value() * remainingAdjustment / totalStarWeights;
            auto const adjustment = increase ? std::min(distanceToTarget, distanceAvailable) : std::max(distanceToTarget, distanceAvailable);

            remainingAdjustment -= adjustment;
            totalStarWeights -= internalStarColumn->WidthLength().Value();
            internalStarColumn->SetWidthDisplayValue(std::max(MinimumStarColumnWidth, internalStarColumn->WidthLength().DisplayValue() + adjustment));
        }

        return remainingAdjustment;
    }

    void DataGrid::AutoGenerateColumnsPrivate()
    {

        if (!measured_ || autoGeneratingColumnOperationCount_ > 0)
        {
            // Reading the DataType when we generate columns could cause the CollectionView to
            // raise a Reset if its Enumeration changed.  In that case, we don't want to generate again.
            return;
        }

        ++autoGeneratingColumnOperationCount_;
        auto handle = dlg_help_utils::make_scope_exit([this]()
            {
                --autoGeneratingColumnOperationCount_;
            });

        // Always remove existing auto-generated columns before generating new ones
        RemoveAutoGeneratedColumns();
        GenerateColumnsFromProperties();
        EnsureRowsPresenterVisibility();
        InvalidateRowHeightEstimate();
    }

    bool DataGrid::ComputeDisplayedColumns()
    {
        auto invalidate = false;
        auto const displayWidth = CellsWidth();
        auto cx = 0.0;
        int32_t firstDisplayedFrozenCol = InvalidIndex;
        auto firstDisplayedScrollingCol = DisplayData().FirstDisplayedScrollingCol();

        // the same problem with negative numbers:
        // if the width passed in is negative, then return 0
        if (displayWidth <= 0 || ColumnsInternal().VisibleColumnCount() == 0)
        {
            DisplayData().FirstDisplayedScrollingCol(InvalidIndex);
            DisplayData().LastTotallyDisplayedScrollingCol(InvalidIndex);
            return invalidate;
        }

        for(auto const& dataGridColumn : ColumnsInternal().GetVisibleFrozenColumns())
        {
            if (firstDisplayedFrozenCol == InvalidIndex)
            {
                firstDisplayedFrozenCol = dataGridColumn.as<DataGridColumn>()->Index();
            }

            cx += GetEdgedColumnWidth(dataGridColumn);
            if (cx >= displayWidth)
            {
                break;
            }
        }

        assert(cx <= ColumnsInternal().GetVisibleFrozenEdgedColumnsWidth());

        if (cx < displayWidth && firstDisplayedScrollingCol != InvalidIndex)
        {
            auto dataGridColumn = ColumnsItemsInternal()[firstDisplayedScrollingCol];
            if (dataGridColumn.IsFrozen())
            {
                dataGridColumn = ColumnsInternal().FirstVisibleScrollingColumn();
                negHorizontalOffset_ = 0;
                if (!dataGridColumn)
                {
                    DisplayData().FirstDisplayedScrollingCol(InvalidIndex);
                    DisplayData().LastTotallyDisplayedScrollingCol(InvalidIndex);
                    return invalidate;
                }
                else
                {
                    firstDisplayedScrollingCol = dataGridColumn.as<DataGridColumn>()->Index();
                }
            }

            cx -= negHorizontalOffset_;
            int32_t visibleScrollingColumnsTmp = 0;
            while (cx < displayWidth && dataGridColumn)
            {
                cx += GetEdgedColumnWidth(dataGridColumn);
                ++visibleScrollingColumnsTmp;
                dataGridColumn = ColumnsInternal().GetNextVisibleColumn(dataGridColumn);
            }

            auto numVisibleScrollingCols = visibleScrollingColumnsTmp;

            // if we inflate the data area then we paint columns to the left of firstDisplayedScrollingCol
            if (cx < displayWidth)
            {
                assert(firstDisplayedScrollingCol != InvalidIndex);

                // first minimize value of negHorizontalOffset_
                if (negHorizontalOffset_ > 0)
                {
                    invalidate = true;
                    if (displayWidth - cx > negHorizontalOffset_)
                    {
                        cx += negHorizontalOffset_;
                        horizontalOffset_ -= negHorizontalOffset_;
                        if (horizontalOffset_ < RoundingDelta)
                        {
                            // Snap to zero to avoid trying to partially scroll in first scrolled off column below
                            horizontalOffset_ = 0;
                        }

                        negHorizontalOffset_ = 0;
                    }
                    else
                    {
                        horizontalOffset_ -= displayWidth - cx;
                        negHorizontalOffset_ -= displayWidth - cx;
                        cx = displayWidth;
                    }

                    // Make sure the HorizontalAdjustment is not greater than the new HorizontalOffset
                    // since it would cause an assertion failure in DataGridCellsPresenter.ShouldDisplayCell
                    // called by DataGridCellsPresenter.MeasureOverride.
                    horizontalAdjustment_ = std::min(horizontalAdjustment_, horizontalOffset_);
                }

                // second try to scroll entire columns
                if (cx < displayWidth && horizontalOffset_ > 0)
                {
                    assert(negHorizontalOffset_ == 0);
                    dataGridColumn = ColumnsInternal().GetPreviousVisibleScrollingColumn(ColumnsItemsInternal()[firstDisplayedScrollingCol]);
                    while (dataGridColumn && cx + GetEdgedColumnWidth(dataGridColumn) <= displayWidth)
                    {
                        cx += GetEdgedColumnWidth(dataGridColumn);
                        visibleScrollingColumnsTmp++;
                        invalidate = true;
                        firstDisplayedScrollingCol = dataGridColumn.as<DataGridColumn>()->Index();
                        horizontalOffset_ -= GetEdgedColumnWidth(dataGridColumn);
                        dataGridColumn = ColumnsInternal().GetPreviousVisibleScrollingColumn(dataGridColumn);
                    }
                }

                // third try to partially scroll in first scrolled off column
                if (cx < displayWidth && horizontalOffset_ > 0)
                {
                    assert(negHorizontalOffset_ == 0);
                    dataGridColumn = ColumnsInternal().GetPreviousVisibleScrollingColumn(ColumnsItemsInternal()[firstDisplayedScrollingCol]);
                    assert(dataGridColumn);
                    assert(GetEdgedColumnWidth(dataGridColumn) > displayWidth - cx);
                    firstDisplayedScrollingCol = dataGridColumn.as<DataGridColumn>()->Index();
                    negHorizontalOffset_ = GetEdgedColumnWidth(dataGridColumn) - displayWidth + cx;
                    horizontalOffset_ -= displayWidth - cx;
                    visibleScrollingColumnsTmp++;
                    invalidate = true;
                    cx = displayWidth;
                    assert(negHorizontalOffset_ == GetNegHorizontalOffsetFromHorizontalOffset(horizontalOffset_));
                }

                // update the number of visible columns to the new reality
                assert(numVisibleScrollingCols <= visibleScrollingColumnsTmp);
                numVisibleScrollingCols = visibleScrollingColumnsTmp;
            }

            auto jumpFromFirstVisibleScrollingCol = numVisibleScrollingCols - 1;
            if (cx > displayWidth && jumpFromFirstVisibleScrollingCol != InvalidIndex)
            {
                if(jumpFromFirstVisibleScrollingCol == 0)
                {
                    jumpFromFirstVisibleScrollingCol = InvalidIndex;
                }
                else
                {
                    --jumpFromFirstVisibleScrollingCol;
                }
            }

            if (jumpFromFirstVisibleScrollingCol == InvalidIndex)
            {
                DisplayData().LastTotallyDisplayedScrollingCol(InvalidIndex); // no totally visible scrolling column at all
            }
            else
            {
                assert(firstDisplayedScrollingCol != InvalidIndex);
                dataGridColumn = ColumnsItemsInternal()[firstDisplayedScrollingCol];
                for (int jump = 0; jump < jumpFromFirstVisibleScrollingCol; jump++)
                {
                    dataGridColumn = ColumnsInternal().GetNextVisibleColumn(dataGridColumn);
                    assert(dataGridColumn);
                }

                DisplayData().LastTotallyDisplayedScrollingCol(dataGridColumn.as<DataGridColumn>()->Index());
            }
        }
        else
        {
            DisplayData().LastTotallyDisplayedScrollingCol(InvalidIndex);
        }

        DisplayData().FirstDisplayedScrollingCol(firstDisplayedScrollingCol);

        return invalidate;
    }

    int32_t DataGrid::ComputeFirstVisibleScrollingColumn()
    {
        if (ColumnsInternal().GetVisibleFrozenEdgedColumnsWidth() >= CellsWidth())
        {
            // Not enough room for scrolling columns.
            negHorizontalOffset_ = 0;
            return InvalidIndex;
        }

        auto dataGridColumn = ColumnsInternal().FirstVisibleScrollingColumn();

        if (horizontalOffset_ == 0)
        {
            negHorizontalOffset_ = 0;
            return !dataGridColumn ? InvalidIndex : dataGridColumn.as<DataGridColumn>()->Index();
        }

        double cx = 0;
        while (dataGridColumn)
        {
            cx += GetEdgedColumnWidth(dataGridColumn);
            if (cx > horizontalOffset_)
            {
                break;
            }

            dataGridColumn = ColumnsInternal().GetNextVisibleColumn(dataGridColumn);
        }

        if (!dataGridColumn)
        {
            assert(cx <= horizontalOffset_);
            dataGridColumn = ColumnsInternal().FirstVisibleScrollingColumn();
            if (!dataGridColumn)
            {
                negHorizontalOffset_ = 0;
                return InvalidIndex;
            }

            if (negHorizontalOffset_ != horizontalOffset_)
            {
                negHorizontalOffset_ = 0;
            }

            return dataGridColumn.as<DataGridColumn>()->Index();
        }

        negHorizontalOffset_ = GetEdgedColumnWidth(dataGridColumn) - (cx - horizontalOffset_);
        return dataGridColumn.as<DataGridColumn>()->Index();
    }

    void DataGrid::CorrectColumnDisplayIndexesAfterDeletion(MiniDumpExplorer::DataGridColumn const& deletedColumn)
    {
        // Column indexes have already been adjusted.
        // This column has already been detached and has retained its old Index and DisplayIndex
        auto const internalDeletedColumn = deletedColumn.as<DataGridColumn>();
        assert(internalDeletedColumn);
        assert(!internalDeletedColumn->OwningGrid());
        assert(internalDeletedColumn->Index() != InvalidIndex);
        assert(internalDeletedColumn->DisplayIndexWithFiller() != InvalidIndex);

        InDisplayIndexAdjustments(true);
        auto handle = dlg_help_utils::make_scope_exit([this]()
            {
                InDisplayIndexAdjustments(false);
                FlushDisplayIndexChanged(false /*raiseEvent*/);
            });

        // The DisplayIndex of columns greater than the deleted column need to be decremented,
        // as do the DisplayIndexMap values of modified column Indexes
        auto& displayIndexMap = ColumnsInternal().DisplayIndexMap();
        displayIndexMap.erase(displayIndexMap.begin() + internalDeletedColumn->DisplayIndexWithFiller());
        for (int32_t displayIndex = 0; displayIndex < displayIndexMap.size(); displayIndex++)
        {
            if (displayIndexMap[displayIndex] > internalDeletedColumn->Index())
            {
                --displayIndexMap[displayIndex];
            }

            if (displayIndex >= internalDeletedColumn->DisplayIndexWithFiller())
            {
                auto const column = ColumnsInternal().GetColumnAtDisplayIndex(displayIndex).as<DataGridColumn>();
                column->DisplayIndexWithFiller(column->DisplayIndexWithFiller() - 1);
                column->DisplayIndexHasChanged(true); // OnColumnDisplayIndexChanged needs to be raised later on
            }
        }

#ifndef NDEBUG
        assert(ColumnsInternal().Debug_VerifyColumnDisplayIndexes());
#endif

        // Now raise all the OnColumnDisplayIndexChanged events
        FlushDisplayIndexChanged(true /*raiseEvent*/);
    }

    void DataGrid::CorrectColumnDisplayIndexesAfterInsertion(MiniDumpExplorer::DataGridColumn const& insertedColumn)
    {
        auto const internalInsertedColumn = insertedColumn.as<DataGridColumn>();
        assert(internalInsertedColumn);
        assert(internalInsertedColumn->OwningGrid() == *this);
        if (internalInsertedColumn->DisplayIndexWithFiller() == InvalidIndex || internalInsertedColumn->DisplayIndexWithFiller() >= ColumnsItemsInternal().size())
        {
            // Developer did not assign a DisplayIndex or picked a large number.
            // Choose the Index as the DisplayIndex.
            internalInsertedColumn->DisplayIndexWithFiller(internalInsertedColumn->Index());
        }

        InDisplayIndexAdjustments(true);

        auto const handle = dlg_help_utils::make_scope_exit([this]()
            {
                InDisplayIndexAdjustments(false);
                FlushDisplayIndexChanged(false /*raiseEvent*/);
            });

        // The DisplayIndex of columns greater than the inserted column need to be incremented,
        // as do the DisplayIndexMap values of modified column Indexes
        auto& displayIndexMap = ColumnsInternal().DisplayIndexMap();
        for (int32_t displayIndex = 0; displayIndex < displayIndexMap.size(); displayIndex++)
        {
            if (displayIndexMap[displayIndex] >= internalInsertedColumn->Index())
            {
                displayIndexMap[displayIndex]++;
            }

            if (displayIndex >= internalInsertedColumn->DisplayIndexWithFiller())
            {
                auto const column = ColumnsInternal().GetColumnAtDisplayIndex(displayIndex).as<DataGridColumn>();
                column->DisplayIndexWithFiller(column->DisplayIndexWithFiller() + 1);
                column->DisplayIndexHasChanged(true); // OnColumnDisplayIndexChanged needs to be raised later on
            }
        }

        displayIndexMap.insert(displayIndexMap.begin() + internalInsertedColumn->DisplayIndexWithFiller(), internalInsertedColumn->Index());

#ifndef NDEBUG
        assert(ColumnsInternal().Debug_VerifyColumnDisplayIndexes());
#endif

        // Now raise all the OnColumnDisplayIndexChanged events
        FlushDisplayIndexChanged(true /*raiseEvent*/);
    }

    void DataGrid::CorrectColumnFrozenStates()
    {
        int32_t index = 0;
        auto frozenColumnWidth = 0.0;
        auto oldFrozenColumnWidth = 0.0;
        for(auto const& column : ColumnsInternal().GetDisplayedColumns())
        {
            if (column.IsFrozen())
            {
                oldFrozenColumnWidth += column.ActualWidth();
            }

            column.as<DataGridColumn>()->IsFrozen(index < FrozenColumnCountWithFiller());
            if (column.IsFrozen())
            {
                frozenColumnWidth += column.ActualWidth();
            }

            ++index;
        }

        if (HorizontalOffset() > std::max(0.0, frozenColumnWidth - oldFrozenColumnWidth))
        {
            UpdateHorizontalOffset(HorizontalOffset() - frozenColumnWidth + oldFrozenColumnWidth);
        }
        else
        {
            UpdateHorizontalOffset(0);
        }
    }

    void DataGrid::CorrectColumnIndexesAfterDeletion(MiniDumpExplorer::DataGridColumn const& deletedColumn) const
    {
        assert(deletedColumn);
        auto const internalDeletedColumn = deletedColumn.as<DataGridColumn>();
        for (auto columnIndex = internalDeletedColumn->Index(); columnIndex < ColumnsItemsInternal().size(); ++columnIndex)
        {
            auto const column = ColumnsItemsInternal()[columnIndex].as<DataGridColumn>();
            column->Index(column->Index() - 1);
            assert(column->Index() == columnIndex);
        }
    }

    void DataGrid::CorrectColumnIndexesAfterInsertion(MiniDumpExplorer::DataGridColumn const& insertedColumn, int32_t const insertionCount) const
    {
        assert(insertedColumn);
        assert(insertionCount > 0);
        auto const internalInsertedColumn = insertedColumn.as<DataGridColumn>();
        for (auto columnIndex = internalInsertedColumn->Index() + insertionCount; columnIndex < ColumnsItemsInternal().size(); ++columnIndex)
        {
            ColumnsItemsInternal()[columnIndex].as<DataGridColumn>()->Index(static_cast<int32_t>(columnIndex));
        }
    }

    // Decreases the widths of all non-star columns with DisplayIndex >= displayIndex such that the total
    // width is decreased by the given amount, if possible.  If the total desired adjustment amount
    // could not be met, the remaining amount of adjustment is returned.  The adjustment stops when
    // the column's target width has been met.
    double DataGrid::DecreaseNonStarColumnWidths(int32_t const displayIndex, std::function<double(MiniDumpExplorer::DataGridColumn const&)> const& targetWidth, double amount, bool const reverse, bool const affectNewColumns)
    {
        if (double_utils::greater_than_or_close(amount, 0))
        {
            return amount;
        }

        for(auto const& column : ColumnsInternal().GetDisplayedColumns(
            reverse,
            [displayIndex, affectNewColumns] (MiniDumpExplorer::DataGridColumn const& column)
            {
                auto const internalColumn = column.as<DataGridColumn>();
                return internalColumn->IsVisible() &&
                        internalColumn->WidthLength().UnitType() != DataGridLengthUnitType::Star &&
                        internalColumn->DisplayIndex() >= displayIndex &&
                        internalColumn->ActualCanUserResize() &&
                        (affectNewColumns || internalColumn->IsInitialDesiredWidthDetermined());
            }))
        {
            amount = DecreaseNonStarColumnWidth(column, std::max(column.as<DataGridColumn>()->ActualMinWidth(), targetWidth(column)), amount);
            if (double_utils::is_zero(amount))
            {
                break;
            }
        }

        return amount;
    }

    void DataGrid::FlushDisplayIndexChanged(bool const raiseEvent)
    {
        for(auto const& column : ColumnsItemsInternal())
        {
            if (auto const internalColumn = column.as<DataGridColumn>();
                internalColumn->DisplayIndexHasChanged())
            {
                internalColumn->DisplayIndexHasChanged(false);
                if (raiseEvent)
                {
                    assert(column != ColumnsInternal().RowGroupSpacerColumn());
                    OnColumnDisplayIndexChanged(column);
                }
            }
        }
    }

    void DataGrid::GenerateColumnsFromProperties()
    {
        // Auto-generated Columns are added at the end so the user columns appear first
        if (auto const dataProperties = DataConnection().DataProperties();
            dataProperties && dataProperties.Size() > 0)
        {
            std::vector<std::tuple<int32_t, MiniDumpExplorer::DataGridAutoGeneratingColumnEventArgs>> columnOrderPairs;

            // Generate the columns
            for(auto const& propertyInfo : dataProperties)
            {
                if(propertyInfo.AutoGenerateField() == false)
                {
                    // Abort column generation because we aren't supposed to auto-generate this field
                    continue;
                }

                auto columnHeader = propertyInfo.Name();
                auto columnOrder = DefaultColumnDisplayOrder;

                // Check if DisplayAttribute is defined on the property
                if(auto const displayName = propertyInfo.DisplayName();
                    !displayName.empty())
                {
                    columnHeader = displayName;
                }

                if(auto const order = propertyInfo.DisplayOrder();
                    order)
                {
                    columnOrder = order.Value();
                }

                // Generate a single column and determine its relative order
                int32_t insertIndex = 0;
                if (columnOrder == std::numeric_limits<int32_t>::max())
                {
                    insertIndex = static_cast<int32_t>(columnOrderPairs.size());
                }
                else
                {
                    for(auto [key, args] : columnOrderPairs)
                    {
                        if (key > columnOrder)
                        {
                            break;
                        }

                        ++insertIndex;
                    }
                }

                auto columnArgs = GenerateColumn(propertyInfo.TypeName(), propertyInfo.Name(), columnHeader);
                columnOrderPairs.insert(columnOrderPairs.begin() + insertIndex, std::make_tuple(columnOrder, columnArgs));
            }

            // Add the columns to the DataGrid in the correct order
            for(auto [key, args] : columnOrderPairs)
            {
                AddGeneratedColumn(args);
            }
        }
        else if (DataConnection().DataIsPrimitive())
        {
            AddGeneratedColumn(GenerateColumn(DataConnection().DataTypeTypeName(), L""sv, DataConnection().DataTypeName()));
        }
    }

    bool DataGrid::GetColumnEffectiveReadOnlyState(MiniDumpExplorer::DataGridColumn const& dataGridColumn) const
    {
        assert(dataGridColumn);

        return IsReadOnly() || dataGridColumn.IsReadOnly() || dataGridColumn.as<DataGridColumn>()->IsDataGridFillerColumn();
    }

    // Returns the absolute coordinate of the left edge of the given column (including
    // the potential gridline - that is the left edge of the gridline is returned). Note that
    // the column does not need to be in the display area.
    double DataGrid::GetColumnXFromIndex(int32_t const index)
    {
        assert(index < ColumnsItemsInternal().size());
        assert(ColumnsItemsInternal()[index].as<DataGridColumn>()->IsVisible());

        auto x = 0.0;
        for(auto const& column : ColumnsInternal().GetVisibleColumns())
        {
            if (index == column.as<DataGridColumn>()->Index())
            {
                break;
            }

            x += GetEdgedColumnWidth(column);
        }

        return x;
    }

    double DataGrid::GetNegHorizontalOffsetFromHorizontalOffset(double horizontalOffset)
    {
        for(auto const& column : ColumnsInternal().GetVisibleScrollingColumns())
        {
            if (GetEdgedColumnWidth(column) > horizontalOffset)
            {
                break;
            }

            horizontalOffset -= GetEdgedColumnWidth(column);
        }

        return horizontalOffset;
    }

    // Increases the widths of all non-star columns with DisplayIndex >= displayIndex such that the total
    // width is increased by the given amount, if possible.  If the total desired adjustment amount
    // could not be met, the remaining amount of adjustment is returned.  The adjustment stops when
    // the column's target width has been met.
    double DataGrid::IncreaseNonStarColumnWidths(int32_t const displayIndex, std::function<double(MiniDumpExplorer::DataGridColumn const&)> const& targetWidth, double amount, bool const reverse, bool const affectNewColumns)
    {
        if (double_utils::less_than(amount, 0))
        {
            return amount;
        }

        for(auto const& column : ColumnsInternal().GetDisplayedColumns(
            reverse,
            [displayIndex, affectNewColumns](MiniDumpExplorer::DataGridColumn const& column)
            {
                auto const internalColumn = column.as<DataGridColumn>();
                return internalColumn->IsVisible() &&
                        internalColumn->WidthLength().UnitType() != DataGridLengthUnitType::Star &&
                        internalColumn->DisplayIndex() >= displayIndex &&
                        internalColumn->ActualCanUserResize() &&
                        (affectNewColumns || internalColumn->IsInitialDesiredWidthDetermined());
            }))
        {
            auto const internalColumn = column.as<DataGridColumn>();
            amount = IncreaseNonStarColumnWidth(column, std::min(internalColumn->ActualMaxWidth(), targetWidth(column)), amount);
            if (double_utils::is_zero(amount))
            {
                break;
            }
        }

        return amount;
    }

    void DataGrid::InsertDisplayedColumnHeader(MiniDumpExplorer::DataGridColumn const& dataGridColumn) const
    {
        assert(dataGridColumn);
        if (columnHeadersPresenter_)
        {
            auto const internalDataGridColumn = dataGridColumn.as<DataGridColumn>();
            internalDataGridColumn->HeaderCell().Visibility(dataGridColumn.Visibility());
            [[maybe_unused]] uint32_t index;
            assert(!columnHeadersPresenter_.Children().IndexOf(internalDataGridColumn->HeaderCell(), index));
            columnHeadersPresenter_.Children().InsertAt(internalDataGridColumn->DisplayIndexWithFiller(), internalDataGridColumn->HeaderCell());
        }
    }

    void DataGrid::RemoveAutoGeneratedColumns()
    {
        uint32_t index = 0;
        ++autoGeneratingColumnOperationCount_;
        auto const handle = dlg_help_utils::make_scope_exit([this]()
            {
                --autoGeneratingColumnOperationCount_;
            });

        while (index < ColumnsInternal().ObservableItems().Size())
        {
            // Skip over the user columns
            while (index < ColumnsInternal().ObservableItems().Size() && !ColumnsInternal().ObservableItems().GetAt(index).IsAutoGenerated())
            {
                index++;
            }

            // Remove the auto-generated columns
            while (index < ColumnsInternal().ObservableItems().Size() && ColumnsInternal().ObservableItems().GetAt(index).IsAutoGenerated())
            {
                ColumnsInternal().ObservableItems().RemoveAt(index);
            }
        }

        ColumnsInternal().AutoGeneratedColumnCount(0);
    }

    bool DataGrid::ScrollColumnIntoView(int32_t const columnIndex)
    {
        assert(columnIndex != InvalidIndex);
        assert(columnIndex < ColumnsItemsInternal().size());

        if (DisplayData().FirstDisplayedScrollingCol() != InvalidIndex &&
            !ColumnsItemsInternal()[columnIndex].IsFrozen() &&
            (columnIndex != DisplayData().FirstDisplayedScrollingCol() || negHorizontalOffset_ > 0))
        {
            if (ColumnsInternal().DisplayInOrder(columnIndex, DisplayData().FirstDisplayedScrollingCol()))
            {
                auto columnsToScroll = ColumnsInternal().GetColumnCount(true /*isVisible*/, false /*isFrozen*/, columnIndex, DisplayData().FirstDisplayedScrollingCol());
                if (negHorizontalOffset_ > 0)
                {
                    ++columnsToScroll;
                }

                ScrollColumns(-static_cast<int32_t>(columnsToScroll));
            }
            else if (columnIndex == DisplayData().FirstDisplayedScrollingCol() && negHorizontalOffset_ > 0)
            {
                ScrollColumns(-1);
            }
            else if (DisplayData().LastTotallyDisplayedScrollingCol() == InvalidIndex ||
                (DisplayData().LastTotallyDisplayedScrollingCol() != columnIndex &&
                    ColumnsInternal().DisplayInOrder(DisplayData().LastTotallyDisplayedScrollingCol(), columnIndex)))
            {
                auto const xColumnLeftEdge = GetColumnXFromIndex(columnIndex);
                auto const xColumnRightEdge = xColumnLeftEdge + GetEdgedColumnWidth(ColumnsItemsInternal()[columnIndex]);
                auto change = xColumnRightEdge - HorizontalOffset() - CellsWidth();
                auto widthRemaining = change;

                auto newFirstDisplayedScrollingCol = ColumnsItemsInternal()[DisplayData().FirstDisplayedScrollingCol()];
                auto nextColumn = ColumnsInternal().GetNextVisibleColumn(newFirstDisplayedScrollingCol);
                auto newColumnWidth = GetEdgedColumnWidth(newFirstDisplayedScrollingCol) - negHorizontalOffset_;
                while (nextColumn && widthRemaining >= newColumnWidth)
                {
                    widthRemaining -= newColumnWidth;
                    newFirstDisplayedScrollingCol = nextColumn;
                    newColumnWidth = GetEdgedColumnWidth(newFirstDisplayedScrollingCol);
                    nextColumn = ColumnsInternal().GetNextVisibleColumn(newFirstDisplayedScrollingCol);
                    negHorizontalOffset_ = 0;
                }

                negHorizontalOffset_ += widthRemaining;
                DisplayData().LastTotallyDisplayedScrollingCol(columnIndex);
                auto const internalNewFirstDisplayedScrollingCol = newFirstDisplayedScrollingCol.as<DataGridColumn>();
                if (internalNewFirstDisplayedScrollingCol->Index() == columnIndex)
                {
                    negHorizontalOffset_ = 0;
                    
                    // If the entire column cannot be displayed, we want to start showing it from its LeftEdge.
                    if (auto const frozenColumnWidth = ColumnsInternal().GetVisibleFrozenEdgedColumnsWidth();
                        newColumnWidth > CellsWidth() - frozenColumnWidth)
                    {
                        DisplayData().LastTotallyDisplayedScrollingCol(InvalidIndex);
                        change = xColumnLeftEdge - HorizontalOffset() - frozenColumnWidth;
                    }
                }

                DisplayData().FirstDisplayedScrollingCol(internalNewFirstDisplayedScrollingCol->Index());

                // At this point DisplayData.FirstDisplayedScrollingColumn and LastDisplayedScrollingColumn should be correct.
                if (change != 0)
                {
                    UpdateHorizontalOffset(HorizontalOffset() + change);
                }
            }
        }

        return true;
    }

    void DataGrid::ScrollColumns(int32_t const columns)
    {
        MiniDumpExplorer::DataGridColumn newFirstVisibleScrollingCol{nullptr};
        int32_t colCount = 0;
        if (columns > 0)
        {
            if (DisplayData().LastTotallyDisplayedScrollingCol() != InvalidIndex)
            {
                auto dataGridColumnTmp = ColumnsItemsInternal()[DisplayData().LastTotallyDisplayedScrollingCol()];
                while (colCount < columns && dataGridColumnTmp)
                {
                    dataGridColumnTmp = ColumnsInternal().GetNextVisibleColumn(dataGridColumnTmp);
                    colCount++;
                }

                if (!dataGridColumnTmp)
                {
                    // no more column to display on the right of the last totally seen column
                    return;
                }
            }

            assert(DisplayData().FirstDisplayedScrollingCol() != InvalidIndex);
            auto dataGridColumnTmp = ColumnsItemsInternal()[DisplayData().FirstDisplayedScrollingCol()];
            colCount = 0;
            while (colCount < columns && dataGridColumnTmp)
            {
                dataGridColumnTmp = ColumnsInternal().GetNextVisibleColumn(dataGridColumnTmp);
                colCount++;
            }

            newFirstVisibleScrollingCol = dataGridColumnTmp;
        }

        if (columns < 0)
        {
            assert(DisplayData().FirstDisplayedScrollingCol() != InvalidIndex);
            auto dataGridColumnTmp = ColumnsItemsInternal()[DisplayData().FirstDisplayedScrollingCol()];
            if (negHorizontalOffset_ > 0)
            {
                colCount++;
            }

            while (colCount < -columns && dataGridColumnTmp)
            {
                dataGridColumnTmp = ColumnsInternal().GetPreviousVisibleScrollingColumn(dataGridColumnTmp);
                colCount++;
            }

            newFirstVisibleScrollingCol = dataGridColumnTmp;
            if (!newFirstVisibleScrollingCol)
            {
                if (negHorizontalOffset_ == 0)
                {
                    // no more column to display on the left of the first seen column
                    return;
                }
                else
                {
                    newFirstVisibleScrollingCol = ColumnsItemsInternal()[DisplayData().FirstDisplayedScrollingCol()];
                }
            }
        }

        auto newColOffset = 0.0;
        for(auto const& dataGridColumn : ColumnsInternal().GetVisibleScrollingColumns())
        {
            if (dataGridColumn == newFirstVisibleScrollingCol)
            {
                break;
            }

            newColOffset += GetEdgedColumnWidth(dataGridColumn);
        }

        UpdateHorizontalOffset(newColOffset);
    }

    void DataGrid::UpdateDisplayedColumns()
    {
        DisplayData().FirstDisplayedScrollingCol(ComputeFirstVisibleScrollingColumn());
        ComputeDisplayedColumns();
    }
}
