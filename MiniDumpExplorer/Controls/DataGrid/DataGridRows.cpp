#include "pch.h"
#include "DataGrid.h"

#include <winrt/Microsoft.UI.Xaml.Automation.Peers.h>

#include "DataGridAutomationPeer.h"
#include "DataGridCell.h"
#include "DataGridFillerColumn.h"
#include "DataGridRow.h"
#include "DataGridRowGroupHeader.h"
#include "DbgHelpUtils/double_utils.h"
#include "DbgHelpUtils/exit_scope.h"
#include "Helpers/TypeHelper.h"
#include "Helpers/UIHelper.h"
#include "Utility/SuspendedHandlers.h"
#include "Utility/logger.h"

using namespace winrt;
using namespace Microsoft::UI::Xaml;
using namespace std::string_literals;
using namespace std::string_view_literals;

namespace winrt::MiniDumpExplorer::implementation
{
    bool DataGrid::AreRowBottomGridLinesRequired() const
    {
        return (GridLinesVisibility() == DataGridGridLinesVisibility::Horizontal || GridLinesVisibility() == DataGridGridLinesVisibility::All) && HorizontalGridLinesBrush();
    }

    int32_t DataGrid::FirstVisibleSlot() const
    {
        return SlotCount() != InvalidIndex ? GetNextVisibleSlot(InvalidIndex) : InvalidIndex;
    }

    int32_t DataGrid::FrozenColumnCountWithFiller()
    {
        auto count = FrozenColumnCount();
        if (ColumnsInternal().RowGroupSpacerColumn().as<DataGridFillerColumn>()->IsRepresented() && (AreRowGroupHeadersFrozen() || count > 0))
        {
            // Either the RowGroupHeaders are frozen by default or the user set a frozen column count.  In both cases, we need to freeze
            // one more column than what the public value says
            ++count;
        }

        return count;
    }

    int32_t DataGrid::LastVisibleSlot() const
    {
        return SlotCount() != InvalidIndex ? GetPreviousVisibleSlot(SlotCount()) : InvalidIndex;
    }

    void DataGrid::CollapseRowGroup(Data::ICollectionViewGroup const& collectionViewGroup, bool const collapseAllSubgroups)
    {
        if (WaitForLostFocus([weakPtr = get_weak(), collectionViewGroup, collapseAllSubgroups]
            {
                if (auto const self = weakPtr.get())
                {
                    self->CollapseRowGroup(collectionViewGroup, collapseAllSubgroups);
                }
            }) ||
            !collectionViewGroup || !CommitEdit())
        {
            return;
        }

        EnsureRowGroupVisibility(RowGroupInfoFromCollectionViewGroup(collectionViewGroup), Visibility::Collapsed, true);

        if (collapseAllSubgroups)
        {
            for (auto const& groupObj : collectionViewGroup.GroupItems())
            {
                if (auto const subGroup = groupObj.try_as<Data::ICollectionViewGroup>();
                    subGroup)
                {
                    CollapseRowGroup(subGroup, collapseAllSubgroups);
                }
            }
        }
    }

    // Expands slots from startSlot to endSlot inclusive and adds the amount expanded in this sub-operation to
    // the given totalHeightChanged of the entire operation
    void DataGrid::ExpandRowGroup(Data::ICollectionViewGroup const& collectionViewGroup, bool const expandAllSubgroups)
    {
        if (WaitForLostFocus([weakPtr = get_weak(), collectionViewGroup, expandAllSubgroups]
            {
                if (auto const self = weakPtr.get())
                {
                    self->ExpandRowGroup(collectionViewGroup, expandAllSubgroups);
                }
            }) ||
            !collectionViewGroup || !CommitEdit())
        {
            return;
        }

        EnsureRowGroupVisibility(RowGroupInfoFromCollectionViewGroup(collectionViewGroup), Visibility::Visible, true);

        if (expandAllSubgroups)
        {
            for (auto const& groupObj : collectionViewGroup.GroupItems())
            {
                if (auto const subGroup = groupObj.try_as<Data::ICollectionViewGroup>();
                    subGroup)
                {
                    ExpandRowGroup(subGroup, expandAllSubgroups);
                }
            }
        }
    }

    void DataGrid::ClearRowSelection(bool const resetAnchorSlot)
    {
        if (resetAnchorSlot)
        {
            anchorSlot_ = InvalidIndex;
        }

        if (selectedItems_.Count() > 0)
        {
            ++noSelectionChangeCount_;
            auto handle = dlg_help_utils::make_scope_exit([this]()
                {
                    --noSelectionChangeCount_;
                });

            // Individually deselecting displayed rows to view potential transitions
            for (auto slot = DisplayData().FirstScrollingSlot();
                slot != InvalidIndex && slot <= DisplayData().LastScrollingSlot();
                ++slot)
            {
                if (auto const row = DisplayData().GetDisplayedElement(slot).try_as<DataGridRow>();
                    row)
                {
                    if (selectedItems_.ContainsSlot(row->Slot()))
                    {
                        SelectSlot(row->Slot(), false);
                    }
                }
            }

            selectedItems_.ClearRows();
            SelectionHasChanged(true);
        }
    }

    void DataGrid::ClearRowSelection(int32_t const slotException, bool const setAnchorSlot)
    {
        ++noSelectionChangeCount_;
        auto handle = dlg_help_utils::make_scope_exit([this]()
            {
                --noSelectionChangeCount_;
            });

        auto exceptionAlreadySelected = false;
        if (selectedItems_.Count() > 0)
        {
            // Individually deselecting displayed rows to view potential transitions
            for (auto slot = DisplayData().FirstScrollingSlot();
                slot != InvalidIndex && slot <= DisplayData().LastScrollingSlot();
                ++slot)
            {
                if (slot != slotException && selectedItems_.ContainsSlot(slot))
                {
                    SelectSlot(slot, false);
                    SelectionHasChanged(true);
                }
            }

            exceptionAlreadySelected = selectedItems_.ContainsSlot(slotException);
            if (auto const selectedCount = selectedItems_.Count();
                selectedCount > 0)
            {
                if (selectedCount > 1)
                {
                    SelectionHasChanged(true);
                }
                else
                {
                    if (auto const currentlySelectedSlot = *selectedItems_.GetIndexes().begin();
                        currentlySelectedSlot != slotException)
                    {
                        SelectionHasChanged(true);
                    }
                }

                selectedItems_.ClearRows();
            }
        }

        if (exceptionAlreadySelected)
        {
            // Exception row was already selected. It just needs to be marked as selected again.
            // No transition involved.
            selectedItems_.SelectSlot(slotException, true /*select*/);
            if (setAnchorSlot)
            {
                anchorSlot_ = slotException;
            }
        }
        else
        {
            // Exception row was not selected. It needs to be selected with potential transition
            SetRowSelection(slotException, true /*isSelected*/, setAnchorSlot);
        }
    }

    int32_t DataGrid::GetCollapsedSlotCount(int32_t const startSlot, int32_t const endSlot) const
    {
        return collapsedSlotsTable_.GetIndexCount(startSlot, endSlot);
    }

    int32_t DataGrid::GetNextVisibleSlot(int32_t const slot) const
    {
        return collapsedSlotsTable_.GetNextGap(slot);
    }

    int32_t DataGrid::GetPreviousVisibleSlot(int32_t const slot) const
    {
        return collapsedSlotsTable_.GetPreviousGap(slot);
    }

    Visibility DataGrid::GetRowDetailsVisibility(int32_t const rowIndex) const
    {
        return GetRowDetailsVisibility(rowIndex, RowDetailsVisibilityMode());
    }

    Visibility DataGrid::GetRowDetailsVisibility(int32_t const rowIndex, DataGridRowDetailsVisibilityMode const gridLevelRowDetailsVisibility) const
    {
        assert(rowIndex != InvalidIndex);
        auto found = false;
        auto const value = showDetailsTable_.GetValueAt(rowIndex, found);
        if (found)
        {
            // The user explicitly set DetailsVisibility on a row, so we should respect that
            return value;
        }

        if (gridLevelRowDetailsVisibility == DataGridRowDetailsVisibilityMode::Visible ||
            (gridLevelRowDetailsVisibility == DataGridRowDetailsVisibilityMode::VisibleWhenSelected &&
                selectedItems_.ContainsSlot(SlotFromRowIndex(rowIndex))))
        {
            return Visibility::Visible;
        }

        return Visibility::Collapsed;
    }

    MiniDumpExplorer::DataGridRow DataGrid::GetRowFromItem(Windows::Foundation::IInspectable const& dataItem) const
    {
        auto const rowIndex = DataConnection().IndexOf(dataItem);
        if (rowIndex == InvalidIndex)
        {
            return nullptr;
        }

        auto const slot = SlotFromRowIndex(rowIndex);
        return IsSlotVisible(slot) ? DisplayData().GetDisplayedElement(slot).try_as<MiniDumpExplorer::DataGridRow>() : nullptr;
    }

    bool DataGrid::GetRowSelection(int32_t const slot) const
    {
        assert(slot != InvalidIndex);
        return selectedItems_.ContainsSlot(slot);
    }

    void DataGrid::InsertElementAt(int32_t const slot, int32_t const rowIndex, Windows::Foundation::IInspectable const& item, std::shared_ptr<DataGridInternal::DataGridRowGroupInfo> const& groupInfo, bool const isCollapsed)
    {
        assert(slot != InvalidIndex);
        assert(slot <= SlotCount());

        auto const isRow = rowIndex != InvalidIndex;
        if (isCollapsed || (IsReadOnly() && rowIndex == DataConnection().NewItemPlaceholderIndex()))
        {
            InsertElement(slot, nullptr /*element*/, true /*updateVerticalScrollBarOnly*/, true /*isCollapsed*/, isRow);
        }
        else if (SlotIsDisplayed(slot))
        {
            // Row at that index needs to be displayed
            if (isRow)
            {
                InsertElement(slot, GenerateRow(rowIndex, slot, item), false /*updateVerticalScrollBarOnly*/, false /*isCollapsed*/, isRow);
            }
            else
            {
                InsertElement(slot, GenerateRowGroupHeader(slot, groupInfo), false /*updateVerticalScrollBarOnly*/, false /*isCollapsed*/, isRow);
            }
        }
        else
        {
            InsertElement(slot, nullptr, !vScrollBar_ || vScrollBar_.Visibility() == Visibility::Visible /*updateVerticalScrollBarOnly*/, false /*isCollapsed*/, isRow);
        }
    }

    void DataGrid::InsertRowAt(int32_t const rowIndex)
    {
        auto const slot = SlotFromRowIndex(rowIndex);
        auto const item = DataConnection().GetDataItem(rowIndex);

        // isCollapsed below is always false because we only use the method if we're not grouping
        InsertElementAt(
            slot,
            rowIndex,
            item,
            nullptr /*DataGridRowGroupInfo*/,
            false /*isCollapsed*/);
    }

    bool DataGrid::IsColumnDisplayed(int32_t const columnIndex) const
    {
        return columnIndex >= FirstDisplayedNonFillerColumnIndex() && columnIndex <= DisplayData().LastTotallyDisplayedScrollingCol();
    }

    bool DataGrid::IsRowRecyclable(MiniDumpExplorer::DataGridRow const& row) const
    {
        return row != EditingRow() && row != focusedRow_;
    }

    bool DataGrid::IsSlotVisible(int32_t const slot) const
    {
        return slot >= DisplayData().FirstScrollingSlot() &&
            slot <= DisplayData().LastScrollingSlot() &&
            slot != InvalidIndex &&
            !collapsedSlotsTable_.Contains(slot);
    }

    void DataGrid::OnUnloadingRowDetails(MiniDumpExplorer::DataGridRow const& row, FrameworkElement const& detailsElement)
    {
        OnUnloadingRowDetails({ row, detailsElement });
    }

    void DataGrid::OnLoadingRowDetails(MiniDumpExplorer::DataGridRow const& row, FrameworkElement const& detailsElement)
    {
        OnLoadingRowDetails({ row, detailsElement });
    }

    void DataGrid::OnRowDetailsVisibilityPropertyChanged(int32_t const rowIndex, Microsoft::UI::Xaml::Visibility const visibility)
    {
        assert(rowIndex != InvalidIndex);
        assert(rowIndex < SlotCount());

        showDetailsTable_.AddValue(rowIndex, visibility);
    }

    void DataGrid::OnRowGroupHeaderToggled(MiniDumpExplorer::DataGridRowGroupHeader const& groupHeader, Microsoft::UI::Xaml::Visibility newVisibility, bool const setCurrent)
    {
        auto const internalGroupHeader = groupHeader.as<DataGridRowGroupHeader>();
        auto const& rowGroupInfo = internalGroupHeader->RowGroupInfo();
        assert(rowGroupInfo);
        assert(rowGroupInfo->CollectionViewGroup().GroupItems().Size() > 0);

        if (WaitForLostFocus([weakPtr = get_weak(), groupHeader, newVisibility, setCurrent]
            {
                if (auto const self = weakPtr.get())
                {
                    self->OnRowGroupHeaderToggled(groupHeader, newVisibility, setCurrent);
                }
            }) ||
            !CommitEdit())
        {
            return;
        }

        if (setCurrent && CurrentSlot() != rowGroupInfo->Slot())
        {
            // Most of the time this is set by the MouseLeftButtonDown handler but validation could cause that code path to fail
            UpdateSelectionAndCurrency(CurrentColumnIndex(), rowGroupInfo->Slot(), DataGridInternal::DataGridSelectionAction::SelectCurrent, false /*scrollIntoView*/);
        }

        UpdateRowGroupVisibility(*rowGroupInfo, newVisibility, true /*isHeaderDisplayed*/);

        ComputeScrollBarsLayout();

        // We need force arrange since our Scrolling Rows could update without automatically triggering layout
        InvalidateRowsArrange();
    }

    void DataGrid::OnRowsMeasure()
    {
        if (!double_utils::is_zero(DisplayData().PendingVerticalScrollHeight()))
        {
            ScrollSlotsByHeight(DisplayData().PendingVerticalScrollHeight());
            DisplayData().PendingVerticalScrollHeight(0);
        }
    }

    void DataGrid::OnSublevelIndentUpdated(MiniDumpExplorer::DataGridRowGroupHeader const& groupHeader, double const newValue)
    {
        assert(DataConnection().CollectionView());
        assert(DataConnection().CollectionView().CollectionGroups());

        uint32_t constexpr groupLevelCount = 1;

        assert(groupHeader.Level() < groupLevelCount);

        auto oldValue = rowGroupSublevelIndents_[groupHeader.Level()];
        if (groupHeader.Level() > 0)
        {
            oldValue -= rowGroupSublevelIndents_[groupHeader.Level() - 1];
        }

        // Update the affected values in our table by the amount affected
        double const change = newValue - oldValue;
        for (auto i = groupHeader.Level(); i < groupLevelCount; ++i)
        {
            rowGroupSublevelIndents_[i] += change;
            assert(RowGroupSublevelIndents()[i] >= 0);
        }

        EnsureRowGroupSpacerColumnWidth(groupLevelCount);
    }

    void DataGrid::RefreshRows(bool const recycleRows, bool const clearRows)
    {
        if (measured_)
        {
            // _desiredCurrentColumnIndex is used in MakeFirstDisplayedCellCurrentCell to set the
            // column position back to what it was before the refresh
            desiredCurrentColumnIndex_ = CurrentColumnIndex();
            auto verticalOffset = verticalOffset_;
            if (DisplayData().PendingVerticalScrollHeight() > 0)
            {
                // Use the pending vertical scrollbar position if there is one, in the case that the collection
                // has been reset multiple times in a row.
                verticalOffset = DisplayData().PendingVerticalScrollHeight();
            }

            VerticalOffset(0);
            negVerticalOffset_ = 0;

            if (clearRows)
            {
                ClearRows(recycleRows);
                ClearRowGroupHeadersTable();
                PopulateRowGroupHeadersTable();
                RefreshSlotCounts();
            }

            RefreshRowGroupHeaders();

            // Update the CurrentSlot because it might have changed
            if (recycleRows && DataConnection().CollectionView())
            {
                CurrentSlot(DataConnection().CollectionView().CurrentPosition() == InvalidIndex
                    ? InvalidIndex : SlotFromRowIndex(DataConnection().CollectionView().CurrentPosition()));
                if (CurrentSlot() == InvalidIndex)
                {
                    SetCurrentCellCore(InvalidIndex, InvalidIndex);
                }
            }

            if (ColumnsItemsInternal().size() > 0)
            {
                auto slotCount = DataConnection().Count();
                slotCount += RowGroupHeadersTable().IndexCount();
                AddSlots(slotCount);
                InvalidateMeasure();
            }

            EnsureRowGroupSpacerColumn();

            if (VerticalScrollBar())
            {
                DisplayData().PendingVerticalScrollHeight(std::min(verticalOffset, VerticalScrollBar().Maximum()));
            }
        }
        else
        {
            if (clearRows)
            {
                ClearRows(recycleRows /*recycle*/);
            }

            ClearRowGroupHeadersTable();
            PopulateRowGroupHeadersTable();
            RefreshSlotCounts();
        }
    }

    void DataGrid::RemoveRowAt(int32_t const rowIndex, Windows::Foundation::IInspectable const& item)
    {
        RemoveElementAt(SlotFromRowIndex(rowIndex), item, true);
    }

    std::shared_ptr<DataGridInternal::DataGridRowGroupInfo> DataGrid::RowGroupInfoFromCollectionViewGroup(Data::ICollectionViewGroup const& collectionViewGroup) const
    {
        for (auto const slot : RowGroupHeadersTable().GetIndexes())
        {
            if (auto rowGroupInfo = RowGroupHeadersTable().GetValueAt(slot);
                rowGroupInfo->CollectionViewGroup() == collectionViewGroup)
            {
                return rowGroupInfo;
            }
        }

        return {};
    }

    int32_t DataGrid::RowIndexFromSlot(int32_t const slot) const
    {
        return slot - RowGroupHeadersTable().GetIndexCount(0, slot);
    }

    bool DataGrid::ScrollSlotIntoView(int32_t const slot, bool const scrolledHorizontally)
    {
        assert(collapsedSlotsTable_.Contains(slot) || !IsSlotOutOfBounds(slot));

        if (scrolledHorizontally && DisplayData().FirstScrollingSlot() <= slot && DisplayData().LastScrollingSlot() >= slot)
        {
            // If the slot is displayed, and we scrolled horizontally, column virtualisation could cause the rows to grow.
            // As a result we need to force measure on the rows we're displaying and recalculate our First and Last slots
            // , so they're accurate
            for (auto const& row : DisplayData().GetScrollingElements(true /*onlyRows*/))
            {
                row.Measure({ std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity() });
            }

            UpdateDisplayedRows(DisplayData().FirstScrollingSlot(), CellsHeight());
        }

        if (DisplayData().FirstScrollingSlot() < slot && DisplayData().LastScrollingSlot() > slot)
        {
            // The row is already displayed in its entirety
            return true;
        }

        if (DisplayData().FirstScrollingSlot() == slot && slot != InvalidIndex)
        {
            if (!double_utils::is_zero(NegVerticalOffset()))
            {
                // First displayed row is partially scrolled of. Let's scroll it so that this.NegVerticalOffset becomes 0.
                DisplayData().PendingVerticalScrollHeight(-NegVerticalOffset());
                InvalidateRowsMeasure(false /*invalidateIndividualRows*/);
            }

            return true;
        }

        auto deltaY = 0.0;
        int32_t firstFullSlot;
        if (DisplayData().FirstScrollingSlot() > slot)
        {
            // Scroll up to the new row, so it becomes the first displayed row
            firstFullSlot = DisplayData().FirstScrollingSlot() - 1;
            if (double_utils::greater_than(NegVerticalOffset(), 0))
            {
                deltaY = -NegVerticalOffset();
            }

            deltaY -= GetSlotElementsHeight(slot, firstFullSlot);  // NOLINT(readability-suspicious-call-argument)
            if (DisplayData().FirstScrollingSlot() - slot > 1)
            {
                // TODO: This will likely discard and create a small number of the same rows, so we could probably
                // optimize this.  The optimization would only affect the PageUp key.
                ResetDisplayedRows();
            }

            negVerticalOffset_ = 0;
            UpdateDisplayedRows(slot, CellsHeight());
        }
        else if (DisplayData().LastScrollingSlot() <= slot)
        {
            // Scroll down to the new row, so it's entirely displayed.  If the height of the row
            // is greater than the height of the DataGrid, then show the top of the row at the top
            // of the grid.
            firstFullSlot = DisplayData().LastScrollingSlot();

            // Figure out how much of the last row is cut off.
            auto const rowHeight = GetExactSlotElementHeight(DisplayData().LastScrollingSlot());
            if (auto const availableHeight = AvailableSlotElementRoom() + rowHeight;
                double_utils::are_close(rowHeight, availableHeight))
            {
                if (DisplayData().LastScrollingSlot() == slot)
                {
                    // We're already at the very bottom, so we don't need to scroll down further.
                    return true;
                }
                else
                {
                    // We're already showing the entire last row so don't count it as part of the delta.
                    ++firstFullSlot;
                }
            }
            else if (rowHeight > availableHeight)
            {
                ++firstFullSlot;
                deltaY += rowHeight - availableHeight;
            }

            // sum up the height of the rest of the full rows.
            if (slot >= firstFullSlot)
            {
                deltaY += GetSlotElementsHeight(firstFullSlot, slot);
            }

            // If the first row we're displaying is no longer adjacent to the rows we have
            // simply discarded the ones we have.
            if (slot - DisplayData().LastScrollingSlot() > 1)
            {
                ResetDisplayedRows();
            }

            if (double_utils::greater_than_or_close(GetExactSlotElementHeight(slot), CellsHeight()))
            {
                // The entire row won't fit in the DataGrid, so we start showing it from the top.
                negVerticalOffset_ = 0;
                UpdateDisplayedRows(slot, CellsHeight());
            }
            else
            {
                UpdateDisplayedRowsFromBottom(slot);
            }
        }

        VerticalOffset(VerticalOffset() + deltaY);
        if (verticalOffset_ < 0 || DisplayData().FirstScrollingSlot() == 0)
        {
            // We scrolled too far because a row's height was larger than its approximation.
            VerticalOffset(NegVerticalOffset());
        }

        // TODO: in certain cases (eg, variable row height), this may not be true
        assert(double_utils::less_than_or_close(NegVerticalOffset(), verticalOffset_));

        SetVerticalOffset(verticalOffset_);

        InvalidateMeasure();
        InvalidateRowsMeasure(false /*invalidateIndividualRows*/);

        return true;
    }

    void DataGrid::SetRowSelection(int32_t const slot, bool const isSelected, bool const setAnchorSlot)
    {
        assert(isSelected || !setAnchorSlot);
        assert(!IsSlotOutOfSelectionBounds(slot));
        ++noSelectionChangeCount_;
        auto handle = dlg_help_utils::make_scope_exit([this]()
            {
                --noSelectionChangeCount_;
            });

        if (SelectionMode() == DataGridSelectionMode::Single && isSelected)
        {
            assert(selectedItems_.Count() != InvalidIndex);
            if (selectedItems_.Count() > 0)
            {

                if (auto const currentlySelectedSlot = *selectedItems_.GetIndexes().begin();
                    currentlySelectedSlot != slot)
                {
                    SelectSlot(currentlySelectedSlot, false);
                    SelectionHasChanged(true);
                }
            }
        }

        if (selectedItems_.ContainsSlot(slot) != isSelected)
        {
            SelectSlot(slot, isSelected);
            SelectionHasChanged(true);
        }

        if (setAnchorSlot)
        {
            anchorSlot_ = slot;
        }
    }

    void DataGrid::SetRowsSelection(int32_t const startSlot, int32_t const endSlot, bool const isSelected)
    {
        assert(startSlot != InvalidIndex);
        assert(startSlot < SlotCount());
        assert(endSlot != InvalidIndex);
        assert(endSlot < SlotCount());
        assert(startSlot <= endSlot);

        ++noSelectionChangeCount_;
        auto handle = dlg_help_utils::make_scope_exit([this]()
            {
                --noSelectionChangeCount_;
            });
        if (isSelected && !selectedItems_.ContainsAll(startSlot, endSlot))
        {
            // At least one row gets selected
            SelectSlots(startSlot, endSlot, true);
            SelectionHasChanged(true);
        }
    }

    int32_t DataGrid::SlotFromRowIndex(int32_t const rowIndex) const
    {
        return rowIndex + RowGroupHeadersTable().GetIndexCountBeforeGap(0, rowIndex);
    }

    void DataGrid::OnRowDetailsVisibilityChanged(MiniDumpExplorer::DataGridRowDetailsEventArgs const& e)
    {
        rowDetailsVisibilityChangedHandler_(*this, e);
    }

    double DataGrid::EdgedRowsHeightCalculated()
    {
        // If we're not displaying any rows or if we have infinite space the relative height of our rows is 0
        if (DisplayData().LastScrollingSlot() == InvalidIndex || std::isinf(AvailableSlotElementRoom()))
        {
            if (oldEdgedRowsHeightCalculated_ > 0)
            {
                oldEdgedRowsHeightCalculated_ = 0;

                LoadMoreDataFromIncrementalItemsSource(0);
            }

            return 0;
        }

        assert(DisplayData().LastScrollingSlot() != InvalidIndex);
        assert(verticalOffset_ >= 0);
        assert(NegVerticalOffset() >= 0);

        // Height of all rows above the viewport
        auto totalRowsHeight = verticalOffset_ - NegVerticalOffset();

        // Add the height of all the rows currently displayed, AvailableRowRoom
        // is not always up to date enough for this
        for (auto const& element : DisplayData().GetScrollingElements())
        {

            if (auto const row = element.try_as<DataGridRow>();
                row)
            {
                totalRowsHeight += row->TargetHeight();
            }
            else
            {
                UIHelper::EnsureMeasured(element);
                totalRowsHeight += element.DesiredSize().Height;
            }
        }

        // Details up to and including viewport
        auto detailsCount = GetDetailsCountInclusive(0, DisplayData().LastScrollingSlot());

        // Subtract details that were accounted for from the totalRowsHeight
        totalRowsHeight -= static_cast<double>(detailsCount) * RowDetailsHeightEstimate();

        // Update the RowHeightEstimate if we have more row information
        if (DisplayData().LastScrollingSlot() >= lastEstimatedRow_)
        {
            lastEstimatedRow_ = DisplayData().LastScrollingSlot();
            rowHeightEstimate_ = totalRowsHeight / (lastEstimatedRow_ + 1 - collapsedSlotsTable_.GetIndexCount(0, lastEstimatedRow_));
        }

        // Calculate estimates for what's beyond the viewport
        if (VisibleSlotCount() > DisplayData().NumDisplayedScrollingElements())
        {
            auto const remainingRowCount = SlotCount() - DisplayData().LastScrollingSlot() - collapsedSlotsTable_.GetIndexCount(DisplayData().LastScrollingSlot(), SlotCount() - 1) - 1;

            // Add estimation for the cell heights of all rows beyond our viewport
            totalRowsHeight += RowHeightEstimate() * remainingRowCount;

            // Add the rest of the details beyond the viewport
            detailsCount += GetDetailsCountInclusive(DisplayData().LastScrollingSlot() + 1, SlotCount() - 1);
        }

        // TODO: Update the DetailsHeightEstimate
        auto const totalDetailsHeight = static_cast<double>(detailsCount) * RowDetailsHeightEstimate();
        auto const newEdgedRowsHeightCalculated = totalRowsHeight + totalDetailsHeight;
        auto const loadMoreDataFromIncrementalItemsSource = newEdgedRowsHeightCalculated < oldEdgedRowsHeightCalculated_;

        oldEdgedRowsHeightCalculated_ = newEdgedRowsHeightCalculated;

        if (loadMoreDataFromIncrementalItemsSource)
        {
            LoadMoreDataFromIncrementalItemsSource(newEdgedRowsHeightCalculated);
        }

        return newEdgedRowsHeightCalculated;
    }

    void DataGrid::CorrectRowAfterDeletion(MiniDumpExplorer::DataGridRow const& row, bool const rowDeleted)
    {
        auto const internalRow = row.as<DataGridRow>();
        internalRow->Slot(internalRow->Slot() - 1);
        if (rowDeleted)
        {
            internalRow->Index(internalRow->Index() - 1);
        }
    }

    void DataGrid::CorrectRowAfterInsertion(MiniDumpExplorer::DataGridRow const& row, bool const rowInserted)
    {
        auto const internalRow = row.as<DataGridRow>();
        internalRow->Slot(internalRow->Slot() + 1);
        if (rowInserted)
        {
            internalRow->Index(internalRow->Index() + 1);
        }
    }

    void DataGrid::AddSlotElement(int32_t const slot, UIElement const& element)
    {
#ifndef NDEBUG
        if (auto const row = element.try_as<DataGridRow>();
            row)
        {
            assert(row->OwningGrid() == *this);
            assert(row->Cells().Count() == ColumnsItemsInternal().size());

            int32_t columnIndex = 0;
            for (auto dataGridCell : row->Cells().GetCells())
            {
                auto const internalDataGridCell = dataGridCell.as<DataGridCell>();
                assert(internalDataGridCell->OwningRow() == *row);
                assert(internalDataGridCell->OwningColumn() == ColumnsItemsInternal()[columnIndex]);
                ++columnIndex;
            }
        }
#endif
        assert(slot == SlotCount());

        OnAddedElement_Phase1(slot, element);
        ++slotCount_;
        ++visibleSlotCount_;
        OnAddedElement_Phase2(slot, false /*updateVerticalScrollBarOnly*/);
        OnElementsChanged(true /*grew*/);
    }

    void DataGrid::AddSlots(int32_t const totalSlots)
    {
        slotCount_ = 0;
        visibleSlotCount_ = 0;
        auto nextGroupSlot = InvalidIndex;
        std::experimental::generator<int32_t>::iterator groupSlotsIt, groupSlotsItEnd;
        if (RowGroupHeadersTable().RangeCount() > 0)
        {
            auto groupSlots = RowGroupHeadersTable().GetIndexes();
            groupSlotsIt = groupSlots.begin();
            groupSlotsItEnd = groupSlots.end();
            if (groupSlotsIt != groupSlotsItEnd)
            {
                nextGroupSlot = *groupSlotsIt;
            }
        }

        int32_t slot = 0;
        int32_t addedRows = 0;
        while (slot < totalSlots && AvailableSlotElementRoom() > 0)
        {
            if (slot == nextGroupSlot)
            {
                auto groupRowInfo = RowGroupHeadersTable().GetValueAt(slot);
                AddSlotElement(slot, GenerateRowGroupHeader(slot, groupRowInfo));
                if (groupSlotsIt != groupSlotsItEnd)
                {
                    ++groupSlotsIt;
                }
                nextGroupSlot = groupSlotsIt != groupSlotsItEnd ? *groupSlotsIt : InvalidIndex;
            }
            else
            {
                AddSlotElement(slot, GenerateRow(addedRows, slot));
                ++addedRows;
            }

            ++slot;
        }

        if (slot < totalSlots)
        {
            slotCount_ += totalSlots - slot;
            visibleSlotCount_ += totalSlots - slot;
            OnAddedElement_Phase2(0, !vScrollBar_ || vScrollBar_.Visibility() == Visibility::Visible /*updateVerticalScrollBarOnly*/);
            OnElementsChanged(true /*grew*/);
        }
    }

    void DataGrid::ApplyDisplayedRowsState(int32_t const startSlot, int32_t const endSlot)
    {
        auto const firstSlot = std::max(DisplayData().FirstScrollingSlot(), startSlot);
        auto const lastSlot = std::min(DisplayData().LastScrollingSlot(), endSlot);

        if (firstSlot != InvalidIndex)
        {
            assert(lastSlot >= firstSlot);
            auto slot = GetNextVisibleSlot(firstSlot - 1);
            while (slot <= lastSlot)
            {
                if (auto const row = DisplayData().GetDisplayedElement(slot).try_as<DataGridRow>();
                    row)
                {
                    row->ApplyState(true /*animate*/);
                }

                slot = GetNextVisibleSlot(slot);
            }
        }
    }

    void DataGrid::ClearRowGroupHeadersTable()
    {
        // Detach existing handlers on CollectionViewGroup.Items.CollectionChanged
        for (auto const slot : RowGroupHeadersTable().GetIndexes())
        {
            if (auto const groupInfo = RowGroupHeadersTable().GetValueAt(slot);
                groupInfo->CollectionViewGroup().GroupItems())
            {
                UnhookVectorChangedListenerFromGroup(groupInfo->CollectionViewGroup().GroupItems(), false /*removeFromTable*/);
            }
        }

        if (topLevelGroup_)
        {
            UnhookVectorChangedListenerFromGroup(topLevelGroup_, false /*removeFromTable*/);
            topLevelGroup_ = nullptr;
        }

        RowGroupHeadersTable().Clear();
        collapsedSlotsTable_.Clear();

        rowGroupHeightsByLevel_.clear();
        rowGroupSublevelIndents_.clear();
    }

    void DataGrid::ClearRows(bool const recycle)
    {
        // Need to clean up recycled rows even if the RowCount is 0
        SetCurrentCellCore(InvalidIndex, InvalidIndex, false /*commitEdit*/, false /*endRowEdit*/);
        ClearRowSelection(true /*resetAnchorSlot*/);
        UnloadElements(recycle);

        ClearShowDetailsTable();
        slotCount_ = 0;
        negVerticalOffset_ = 0;
        SetVerticalOffset(0);
        ComputeScrollBarsLayout();
    }

    void DataGrid::ClearShowDetailsTable()
    {
        showDetailsTable_.Clear();
    }

    double DataGrid::CollapseSlotsInTable(int32_t const startSlot, int32_t const endSlot, int32_t& slotsExpanded, int32_t const lastDisplayedSlot, double& heightChangeBelowLastDisplayedSlot)
    {
        auto firstSlot = startSlot;
        int32_t lastSlot;
        double totalHeightChange = 0;

        // Figure out which slots actually need to be expanded since some might already be collapsed
        while (firstSlot <= endSlot)
        {
            firstSlot = collapsedSlotsTable_.GetNextGap(firstSlot - 1);

            if (auto const nextCollapsedSlot = collapsedSlotsTable_.GetNextIndex(firstSlot);
                nextCollapsedSlot != InvalidIndex)
            {
                lastSlot = std::min(endSlot, nextCollapsedSlot - 1);
            }
            else
            {
                lastSlot = endSlot;
            }

            if (firstSlot <= lastSlot)
            {
                auto const heightChange = GetHeightEstimate(firstSlot, lastSlot);
                totalHeightChange -= heightChange;
                slotsExpanded -= lastSlot - firstSlot + 1;

                if (lastSlot > lastDisplayedSlot)
                {
                    if (firstSlot > lastDisplayedSlot)
                    {
                        heightChangeBelowLastDisplayedSlot -= heightChange;
                    }
                    else
                    {
                        heightChangeBelowLastDisplayedSlot -= GetHeightEstimate(lastDisplayedSlot + 1, lastSlot);
                    }
                }

                firstSlot = lastSlot + 1;
            }
        }

        // Update _collapsedSlotsTable in one bulk operation
        collapsedSlotsTable_.AddValues(startSlot, endSlot - startSlot + 1, Visibility::Collapsed);

        return totalHeightChange;
    }

    void DataGrid::CorrectEditingRow()
    {
        if (auto const internalEditingRow = EditingRow().try_as<DataGridRow>();
            internalEditingRow)
        {
            internalEditingRow->Index(DataConnection().IndexOf(internalEditingRow->DataContext()));
            internalEditingRow->Slot(SlotFromRowIndex(internalEditingRow->Index()));
            if (internalEditingRow->Index() > 0)
            {
                // The collection actually removes and re-inserts the edited item during a commit operation.
                // We recycle the editing row in this case in order to avoid generating a new element, but we don't
                // care about refreshing its background/foreground until the item is added back (i.e. Index > 0).
                internalEditingRow->EnsureBackground();
                internalEditingRow->EnsureForeground();
            }
        }
    }

    void DataGrid::CorrectLastSubItemSlotsAfterInsertion(std::shared_ptr<DataGridInternal::DataGridRowGroupInfo> subGroupInfo)
    {
        while (subGroupInfo)
        {
            auto const subGroupLevel = subGroupInfo->Level();
            subGroupInfo->LastSubItemSlot(subGroupInfo->LastSubItemSlot() + 1);

            while (subGroupInfo && subGroupInfo->Level() >= subGroupLevel)
            {
                auto const subGroupSlot = RowGroupHeadersTable().GetPreviousIndex(subGroupInfo->Slot());
                subGroupInfo = RowGroupHeadersTable().GetValueAt(subGroupSlot);
            }
        }
    }

    void DataGrid::CorrectSlotsAfterDeletion(int32_t const slotDeleted, bool const wasRow)
    {
        assert(slotDeleted != InvalidIndex);

        // Take care of the non-visible loaded rows
        for (auto it = loadedRows_.begin(); it != loadedRows_.end();)
        {
            if (auto const dataGridRow = it->as<DataGridRow>();
                IsSlotVisible(dataGridRow->Slot()))
            {
                ++it;
            }
            else
            {
                if (dataGridRow->Slot() > slotDeleted)
                {
                    CorrectRowAfterDeletion(*dataGridRow, wasRow);
                    ++it;
                }
                else if (dataGridRow->Slot() == slotDeleted)
                {
                    it = loadedRows_.erase(it);
                }
                else
                {
                    ++it;
                }
            }
        }

        // Take care of the non-visible edited row
        CorrectEditingRow();

        // Take care of the non-visible focused row
        if (auto const focusedRow = focusedRow_.try_as<DataGridRow>();
            focusedRow_ &&
            focusedRow_ != EditingRow() &&
            !IsSlotVisible(focusedRow->Slot()) &&
            focusedRow->Slot() > slotDeleted)
        {
            CorrectRowAfterDeletion(focusedRow_, wasRow);
            focusedRow->EnsureBackground();
            focusedRow->EnsureForeground();
        }

        // Take care of the visible rows
        for (auto const row : DisplayData().GetScrollingElements(true /*onlyRows*/))
        {
            if (auto internalRow = row.as<DataGridRow>();
                internalRow->Slot() > slotDeleted)
            {
                CorrectRowAfterDeletion(*internalRow, wasRow);
                internalRow->EnsureBackground();
                internalRow->EnsureForeground();
            }
        }

        // Update the RowGroupHeaders
        for (auto const slot : RowGroupHeadersTable().GetIndexes())
        {
            auto const rowGroupInfo = RowGroupHeadersTable().GetValueAt(slot);
            if (rowGroupInfo->Slot() > slotDeleted)
            {
                rowGroupInfo->Slot(rowGroupInfo->Slot() - 1);
            }

            if (rowGroupInfo->LastSubItemSlot() >= slotDeleted)
            {
                rowGroupInfo->LastSubItemSlot(rowGroupInfo->LastSubItemSlot() - 1);
            }
        }

        // Update which row we've calculated the RowHeightEstimate up to
        if (lastEstimatedRow_ >= slotDeleted)
        {
            --lastEstimatedRow_;
        }
    }

    void DataGrid::CorrectSlotsAfterInsertion(int32_t const slotInserted, bool const isCollapsed, bool const rowInserted)
    {
        assert(slotInserted != InvalidIndex);

        // Take care of the non-visible loaded rows
        for (auto const& dataGridRow : loadedRows_)
        {
            if (auto const internalDataGridRow = dataGridRow.as<DataGridRow>();
                !IsSlotVisible(internalDataGridRow->Slot()) && internalDataGridRow->Slot() >= slotInserted)
            {
                CorrectRowAfterInsertion(dataGridRow, rowInserted);
            }
        }

        // Take care of the non-visible focused row
        if (auto const focusedRow = focusedRow_.try_as<DataGridRow>();
            focusedRow_ &&
            focusedRow_ != EditingRow() &&
            !(IsSlotVisible(focusedRow->Slot()) || (focusedRow->Slot() == slotInserted && isCollapsed)) &&
            focusedRow->Slot() >= slotInserted)
        {
            CorrectRowAfterInsertion(focusedRow_, rowInserted);
            focusedRow->EnsureBackground();
            focusedRow->EnsureForeground();
        }

        // Take care of the visible rows
        for (auto const& row : DisplayData().GetScrollingElements(true /*onlyRows*/))
        {
            if (const auto internalRow = row.as<DataGridRow>();
                internalRow->Slot() >= slotInserted)
            {
                CorrectRowAfterInsertion(*internalRow, rowInserted);
                internalRow->EnsureBackground();
                internalRow->EnsureForeground();
            }
        }

        // Re-calculate the EditingRow's Slot and Index and ensure that it is still selected.
        CorrectEditingRow();

        // Update the RowGroupHeaders
        for (auto const slot : RowGroupHeadersTable().GetIndexes(slotInserted))
        {
            auto const rowGroupInfo = RowGroupHeadersTable().GetValueAt(slot);
            if (rowGroupInfo->Slot() >= slotInserted)
            {
                rowGroupInfo->Slot(rowGroupInfo->Slot() + 1);
            }

            // We are purposefully checking GT and not GTE because the equality case is handled
            // by the CorrectLastSubItemSlotsAfterInsertion method.
            if (rowGroupInfo->LastSubItemSlot() > slotInserted)
            {
                rowGroupInfo->LastSubItemSlot(rowGroupInfo->LastSubItemSlot() + 1);
            }
        }

        // Update which row we've calculated the RowHeightEstimate up to.
        if (lastEstimatedRow_ >= slotInserted)
        {
            ++lastEstimatedRow_;
        }
    }

    int32_t DataGrid::CountAndPopulateGroupHeaders(Windows::Foundation::IInspectable const& group, int32_t const rootSlot, uint32_t const level)
    {
        int32_t treeCount = 1;

        if (auto const collectionViewGroup = group.try_as<Data::ICollectionViewGroup>();
            collectionViewGroup)
        {
            if (collectionViewGroup.GroupItems() && collectionViewGroup.GroupItems().Size() > 0)
            {
                HookupVectorChangedListenerToGroup(collectionViewGroup.GroupItems());

                if (collectionViewGroup.GroupItems().First().try_as<Data::ICollectionViewGroup>())
                {
                    for (auto const& subGroup : collectionViewGroup.GroupItems())
                    {
                        treeCount += CountAndPopulateGroupHeaders(subGroup, rootSlot + treeCount, level + 1);
                    }
                }
                else
                {
                    // Optimization: don't walk to the bottom level nodes
                    treeCount += collectionViewGroup.GroupItems().Size();
                }
            }

            RowGroupHeadersTable().AddValue(rootSlot, std::make_shared<DataGridInternal::DataGridRowGroupInfo>(collectionViewGroup, Visibility::Visible, level, rootSlot, rootSlot + treeCount - 1));
        }

        return treeCount;
    }

    void DataGrid::CollectionViewGroupItems_VectorChanged(Windows::Foundation::Collections::IObservableVector<Windows::Foundation::IInspectable> const& groupItems, Windows::Foundation::Collections::IVectorChangedEventArgs const& e)
    {
        auto const index = e.Index();
        switch (e.CollectionChange())
        {
        case Windows::Foundation::Collections::CollectionChange::ItemChanged:
            break;
        case Windows::Foundation::Collections::CollectionChange::ItemInserted:
            OnCollectionViewGroupItemInserted(groupItems, groupItems.GetAt(index), index);
            break;
        case Windows::Foundation::Collections::CollectionChange::ItemRemoved:
            OnCollectionViewGroupItemRemoved(groupItems, groupItems.GetAt(index), index);
            break;
        case Windows::Foundation::Collections::CollectionChange::Reset:
            break;
        }
    }

    void DataGrid::EnsureAncestorsExpanderButtonChecked(DataGridInternal::DataGridRowGroupInfo const& parentGroupInfo)
    {
        if (IsSlotVisible(parentGroupInfo.Slot()))
        {
            auto ancestorGroupHeader = DisplayData().GetDisplayedElement(parentGroupInfo.Slot()).try_as<DataGridRowGroupHeader>();
            while (ancestorGroupHeader)
            {
                ancestorGroupHeader->EnsureExpanderButtonIsChecked();
                if (ancestorGroupHeader->Level() > 0)
                {
                    if (auto const slot = RowGroupHeadersTable().GetPreviousIndex(ancestorGroupHeader->RowGroupInfo()->Slot());
                        IsSlotVisible(slot))
                    {
                        ancestorGroupHeader = DisplayData().GetDisplayedElement(slot).try_as<DataGridRowGroupHeader>();
                        continue;
                    }
                }

                break;
            }
        }
    }

    void DataGrid::EnsureRowDetailsVisibility(MiniDumpExplorer::DataGridRow const& row, bool const raiseNotification) const
    {
        // Show or hide RowDetails based on DataGrid settings
        row.as<DataGridRow>()->SetDetailsVisibilityInternal(
            GetRowDetailsVisibility(row.Index()),
            raiseNotification);
    }

    std::experimental::generator<MiniDumpExplorer::DataGridRow> DataGrid::GetAllRows() const
    {
        if (rowsPresenter_)
        {
            for (auto const& element : rowsPresenter_.Children())
            {
                if (auto const row = element.try_as<MiniDumpExplorer::DataGridRow>();
                    row)
                {
                    co_yield row;
                }
            }
        }
    }

    void DataGrid::OnRowDoubleClicked(MiniDumpExplorer::DataGridRow const& row, [[maybe_unused]] Input::DoubleTappedRoutedEventArgs const& e)
    {
        OnRowDoubleTapped({row, nullptr});
    }

    std::experimental::generator<MiniDumpExplorer::DataGridRowGroupHeader> DataGrid::GetAllRowGroupHeaders() const
    {
        if (rowsPresenter_)
        {
            for (auto const& element : rowsPresenter_.Children())
            {
                if (auto const rowGroupHeader = element.try_as<MiniDumpExplorer::DataGridRowGroupHeader>();
                    rowGroupHeader)
                {
                    co_yield rowGroupHeader;
                }
            }
        }
    }

    // Returns the number of rows with details visible between lowerBound and upperBound exclusive.
    // As of now, the caller needs to account for Collapsed slots.  This method assumes everything
    // is visible
    int32_t DataGrid::GetDetailsCountInclusive(int32_t lowerBound, int32_t upperBound) const
    {
        // Convert from slots to indexes.
        lowerBound = RowGroupHeadersTable().GetNextGap(lowerBound - 1);
        upperBound = RowGroupHeadersTable().GetPreviousGap(upperBound + 1);

        lowerBound = RowIndexFromSlot(lowerBound);
        upperBound = RowIndexFromSlot(upperBound);

        if (upperBound <= lowerBound + 1)
        {
            return 0;
        }

        auto const indexCount = upperBound - (lowerBound + 1);

        switch (RowDetailsVisibilityMode())
        {
        case DataGridRowDetailsVisibilityMode::Collapsed:  // NOLINT(bugprone-branch-clone)
            // Total rows with details explicitly turned on
            return showDetailsTable_.GetIndexCount(lowerBound, upperBound, Visibility::Visible);
        case DataGridRowDetailsVisibilityMode::Visible:
            // Total rows minus ones which explicitly turned details off
            return indexCount - showDetailsTable_.GetIndexCount(lowerBound, upperBound, Visibility::Collapsed);
        case DataGridRowDetailsVisibilityMode::VisibleWhenSelected:
            // Total number of remaining rows that are selected
            return selectedItems_.GetIndexCount(lowerBound, upperBound);

        default:
            assert(false);
            return 0;
        }
    }

    void DataGrid::EnsureRowGroupSpacerColumn()
    {
        if (auto const spacerColumnChanged = ColumnsInternal().EnsureRowGrouping(!RowGroupHeadersTable().IsEmpty());
            spacerColumnChanged)
        {
            if (ColumnsInternal().RowGroupSpacerColumn().as<DataGridFillerColumn>()->IsRepresented() && CurrentColumnIndex() == 0)
            {
                CurrentColumn(ColumnsInternal().FirstVisibleNonFillerColumn());
            }

            ProcessFrozenColumnCount();
        }
    }

    void DataGrid::EnsureRowGroupSpacerColumnWidth(uint32_t const groupLevelCount)
    {
        if (groupLevelCount == 0)
        {
            ColumnsInternal().RowGroupSpacerColumn().Width(MiniDumpExplorer::DataGridLength{ 0.0 });
        }
        else
        {
            ColumnsInternal().RowGroupSpacerColumn().Width(MiniDumpExplorer::DataGridLength{ RowGroupSublevelIndents()[groupLevelCount - 1] });
        }
    }

    void DataGrid::EnsureRowGroupVisibility(std::shared_ptr<DataGridInternal::DataGridRowGroupInfo> const& rowGroupInfo, Microsoft::UI::Xaml::Visibility const visibility, bool const setCurrent)
    {
        if (!rowGroupInfo)
        {
            return;
        }

        if (rowGroupInfo->Visibility() != visibility)
        {
            if (IsSlotVisible(rowGroupInfo->Slot()))
            {
                auto const rowGroupHeader = DisplayData().GetDisplayedElement(rowGroupInfo->Slot()).try_as<DataGridRowGroupHeader>();
                assert(rowGroupHeader);
                rowGroupHeader->ToggleExpandCollapse(visibility, setCurrent);
            }
            else
            {
                if (collapsedSlotsTable_.Contains(rowGroupInfo->Slot()))
                {
                    // Somewhere up the parent chain, there's a collapsed header so all the slots remain the same, and
                    // we just need to mark this header with the new visibility
                    rowGroupInfo->Visibility(visibility);
                }
                else
                {
                    if (rowGroupInfo->Slot() < DisplayData().FirstScrollingSlot())
                    {
                        auto const heightChange = UpdateRowGroupVisibility(*rowGroupInfo, visibility, false /*isHeaderDisplayed*/);

                        // Use epsilon instead of 0 here so that in the off chance that our estimates put the vertical offset negative
                        // the user can still scroll to the top since the offset is non-zero
                        SetVerticalOffset(std::max(std::numeric_limits<double>::epsilon(), verticalOffset_ + heightChange));

                        DisplayData().FullyRecycleElements();
                    }
                    else
                    {
                        UpdateRowGroupVisibility(*rowGroupInfo, visibility, false /*isHeaderDisplayed*/);
                    }

                    UpdateVerticalScrollBar();
                }
            }
        }
    }

    // Expands slots from startSlot to endSlot inclusive and adds the amount expanded in this sub-operation to
    // the given totalHeightChanged of the entire operation
    void DataGrid::ExpandSlots(int32_t const startSlot, int32_t const endSlot, bool const isHeaderDisplayed, int32_t& slotsExpanded, double& totalHeightChange)
    {
        auto heightAboveStartSlot = 0.0;
        if (isHeaderDisplayed)
        {
            auto slot = DisplayData().FirstScrollingSlot();
            while (slot < startSlot)
            {
                heightAboveStartSlot += GetExactSlotElementHeight(slot);
                slot = GetNextVisibleSlot(slot);
            }

            // First make the bottom rows available for recycling, so we minimize element creation when expanding
            for (int32_t i = 0; i < endSlot - startSlot + 1 && DisplayData().LastScrollingSlot() > endSlot; ++i)
            {
                RemoveDisplayedElement(DisplayData().LastScrollingSlot(), false /*wasDeleted*/, true /*updateSlotInformation*/);
            }
        }

        // Figure out which slots actually need to be expanded since some might already be collapsed
        auto currentHeightChange = 0.0;
        auto firstSlot = startSlot;
        while (firstSlot <= endSlot)
        {
            firstSlot = collapsedSlotsTable_.GetNextIndex(firstSlot - 1);
            if (firstSlot == InvalidIndex)
            {
                break;
            }

            if (auto const lastSlot = std::min(endSlot, collapsedSlotsTable_.GetNextGap(firstSlot) - 1);
                firstSlot <= lastSlot)
            {
                if (!isHeaderDisplayed)
                {
                    // Estimate the height change if the slots aren't displayed.  If they are displayed, we can add real values
                    auto rowCount = lastSlot - firstSlot + 1;
                    auto headerHeight = 0.0;
                    rowCount -= GetRowGroupHeaderCount(firstSlot, lastSlot, Visibility::Collapsed, headerHeight);
                    auto const detailsCount = GetDetailsCountInclusive(firstSlot, lastSlot);
                    currentHeightChange += headerHeight + detailsCount * RowDetailsHeightEstimate() + rowCount * RowHeightEstimate();
                }

                slotsExpanded += lastSlot - firstSlot + 1;
                firstSlot = lastSlot + 1;
            }
        }

        // Update _collapsedSlotsTable in one bulk operation
        collapsedSlotsTable_.RemoveValues(startSlot, endSlot - startSlot + 1);

        if (isHeaderDisplayed)
        {
            auto const availableHeight = CellsHeight() - heightAboveStartSlot;

            // Actually expand the displayed slots up to what we can display
            auto lastExpandedSlot = InvalidIndex;
            for (auto i = startSlot; i <= endSlot && currentHeightChange < availableHeight; ++i)
            {
                auto const insertedElement = InsertDisplayedElement(i, false /*updateSlotInformation*/);
                lastExpandedSlot = i;
                UIHelper::EnsureMeasured(insertedElement);
                currentHeightChange += insertedElement.DesiredSize().Height;
                if (i > DisplayData().LastScrollingSlot())
                {
                    DisplayData().LastScrollingSlot(i);
                }
            }

            // We were unable to expand the slots from (lastExpandedSlot + 1) to endSlot because we ran out of space;
            // however, we also have extra visible elements below endSlot.  In this case, we need to remove the
            // extra elements.  While we remove these, we need to mark (lastExpandedSlot + 1) to endSlot collapsed
            // because that is a temporary gap that is not accounted for.
            if (lastExpandedSlot != InvalidIndex && lastExpandedSlot < endSlot && DisplayData().LastScrollingSlot() > endSlot)
            {
                // Temporarily account for the slots we couldn't expand by marking them collapsed
                collapsedSlotsTable_.AddValues(lastExpandedSlot + 1, endSlot - lastExpandedSlot, Visibility::Collapsed);

                // Remove the extra elements below our lastExpandedSlot
                RemoveNonDisplayedRows(DisplayData().FirstScrollingSlot(), lastExpandedSlot);

                // Remove the temporarily marked collapsed rows
                collapsedSlotsTable_.RemoveValues(lastExpandedSlot + 1, endSlot - lastExpandedSlot);
            }
        }

        // Update the total height for the entire Expand operation
        totalHeightChange += currentHeightChange;
    }

    void DataGrid::GenerateEditingElements()
    {
        if (auto const internalEditingRow = EditingRow().try_as<DataGridRow>();
            internalEditingRow)
        {
            assert(internalEditingRow->Cells().Count() == ColumnsItemsInternal().size());
            for (auto const& column : ColumnsInternal().GetDisplayedColumns([](MiniDumpExplorer::DataGridColumn const& c) { return c.as<DataGridColumn>()->IsVisible() && !c.IsReadOnly(); }))
            {
                auto const internalColumn = column.as<DataGridColumn>();
                internalColumn->GenerateEditingElementInternal(internalEditingRow->Cells()[internalColumn->Index()], internalEditingRow->DataContext());
            }
        }
    }

    MiniDumpExplorer::DataGridRow DataGrid::GenerateRow(int32_t const rowIndex, int32_t const slot)
    {
        return GenerateRow(rowIndex, slot, DataConnection().GetDataItem(rowIndex));
    }

    MiniDumpExplorer::DataGridRow DataGrid::GenerateRow(int32_t const rowIndex, int32_t const slot, Windows::Foundation::IInspectable const& dataContext)
    {
        assert(rowIndex != InvalidIndex);
        auto dataGridRow = GetGeneratedRow(dataContext);
        if (!dataGridRow)
        {
            dataGridRow = DisplayData().GetUsedRow();
            if (!dataGridRow)
            {
                dataGridRow = MiniDumpExplorer::DataGridRow{};
            }

            auto const internalDataGridRow = dataGridRow.as<DataGridRow>();
            internalDataGridRow->Index(rowIndex);
            internalDataGridRow->Slot(slot);
            internalDataGridRow->OwningGrid(*this);
            internalDataGridRow->DataContext(dataContext);
            CompleteCellsCollection(dataGridRow);

            OnLoadingRow(MiniDumpExplorer::DataGridRowEventArgs{ dataGridRow });

            if (auto const peer = Automation::Peers::FrameworkElementAutomationPeer::FromElement(*this).try_as<DataGridAutomationPeer>();
                peer)
            {
                peer->UpdateRowPeerEventsSource(dataGridRow);
            }
        }

        return dataGridRow;
    }

    MiniDumpExplorer::DataGridRowGroupHeader DataGrid::GenerateRowGroupHeader(int32_t const slot, std::shared_ptr<DataGridInternal::DataGridRowGroupInfo> const& rowGroupInfo)
    {
        assert(slot != InvalidIndex);
        assert(rowGroupInfo);

        auto groupHeader = DisplayData().GetUsedGroupHeader();
        if (!groupHeader)
        {
            groupHeader = MiniDumpExplorer::DataGridRowGroupHeader{};
        }
        auto const internalGroupHeader = groupHeader.as<DataGridRowGroupHeader>();
        internalGroupHeader->OwningGrid(*this);
        internalGroupHeader->RowGroupInfo(rowGroupInfo);
        internalGroupHeader->DataContext(rowGroupInfo->CollectionViewGroup());
        internalGroupHeader->Level(static_cast<uint32_t>(rowGroupInfo->Level()));

        OnLoadingRowGroup(MiniDumpExplorer::DataGridRowGroupHeaderEventArgs{ groupHeader });

        if (!groupHeader.PropertyName().empty() &&
            groupHeader.PropertyValue().empty() &&
            rowGroupInfo->CollectionViewGroup().GroupItems() &&
            rowGroupInfo->CollectionViewGroup().GroupItems().Size() > 0)
        {
            if (auto const propertyValue = TypeHelper::GetDependencyProperty(rowGroupInfo->CollectionViewGroup().GroupItems().GetAt(0), groupHeader.PropertyName());
                propertyValue)
            {
                groupHeader.PropertyValue(UIHelper::ObjectToString(propertyValue));
            }
        }

        groupHeader.as<DataGridRowGroupHeader>()->UpdateTitleElements();

        if (auto const peer = Automation::Peers::FrameworkElementAutomationPeer::FromElement(*this).try_as<DataGridAutomationPeer>();
            peer)
        {
            peer->UpdateRowGroupHeaderPeerEventsSource(groupHeader);
        }

        return groupHeader;
    }

    double DataGrid::GetExactSlotElementHeight(int32_t const slot)
    {
        assert(slot != InvalidIndex);
        assert(slot < SlotCount());

        if (IsSlotVisible(slot))
        {
            auto const element = DisplayData().GetDisplayedElement(slot);
            assert(element);
            UIHelper::EnsureMeasured(element);
            return element.DesiredSize().Height;
        }

        // InsertDisplayedElement automatically measures the element
        auto const slotElement = InsertDisplayedElement(slot, true /*updateSlotInformation*/);
        assert(slotElement);
        return slotElement.DesiredSize().Height;
    }

    double DataGrid::GetHeightEstimate(int32_t const fromSlot, int32_t const toSlot) const
    {
        auto rowCount = toSlot - fromSlot + 1;
        auto headerHeight = 0.0;
        rowCount -= GetRowGroupHeaderCount(fromSlot, toSlot, Visibility::Visible, headerHeight);
        auto const detailsCount = GetDetailsCountInclusive(fromSlot, toSlot);

        return headerHeight + static_cast<double>(detailsCount) * RowDetailsHeightEstimate() + static_cast<double>(rowCount) * RowHeightEstimate();
    }

    std::shared_ptr<DataGridInternal::DataGridRowGroupInfo> DataGrid::GetParentGroupInfo(Windows::Foundation::IInspectable const& collection) const
    {
        if (collection == DataConnection().CollectionView().CollectionGroups())
        {
            // If the new item is a root level element, it has no parent group, so create an empty RowGroupInfo
            return std::make_shared<DataGridInternal::DataGridRowGroupInfo>(nullptr, Visibility::Visible, InvalidIndex, InvalidIndex, InvalidIndex);
        }

        for (auto const slot : RowGroupHeadersTable().GetIndexes())
        {
            if (auto const groupInfo = RowGroupHeadersTable().GetValueAt(slot);
                groupInfo->CollectionViewGroup().GroupItems() == collection)
            {
                return groupInfo;
            }
        }

        return {};
    }

    int32_t DataGrid::GetRowGroupHeaderCount(int32_t const startSlot, int32_t const endSlot, std::optional<Microsoft::UI::Xaml::Visibility> const visibility, double& headersHeight) const
    {
        int32_t count = 0;
        headersHeight = 0;
        for (auto const slot : RowGroupHeadersTable().GetIndexes(startSlot))
        {
            if (slot > endSlot)
            {
                return count;
            }

            auto const rowGroupInfo = RowGroupHeadersTable().GetValueAt(slot);
            if (!visibility.has_value() ||
                (visibility == Visibility::Visible && !collapsedSlotsTable_.Contains(slot)) ||
                (visibility == Visibility::Collapsed && collapsedSlotsTable_.Contains(slot)))
            {
                ++count;
                headersHeight += rowGroupHeightsByLevel_[rowGroupInfo->Level()];
            }
        }

        return count;
    }

    // If the provided slot is displayed, returns the exact height.
    // If the slot is not displayed, returns a default height.
    double DataGrid::GetSlotElementHeight(int32_t const slot) const
    {
        assert(slot != InvalidIndex);
        assert(slot < SlotCount());
        if (IsSlotVisible(slot))
        {
            assert(DisplayData().GetDisplayedElement(slot));
            auto const element = DisplayData().GetDisplayedElement(slot);
            UIHelper::EnsureMeasured(element);
            return element.DesiredSize().Height;
        }

        if (auto const rowGroupInfo = RowGroupHeadersTable().GetValueAt(slot);
            rowGroupInfo)
        {
            return rowGroupHeightsByLevel_[rowGroupInfo->Level()];
        }

        // Assume it's a row since we're either not grouping or it wasn't a RowGroupHeader.
        return RowHeightEstimate() + (GetRowDetailsVisibility(RowIndexFromSlot(slot)) == Visibility::Visible ? RowDetailsHeightEstimate() : 0);
    }

    // Accumulates the approximate height of the non-collapsed slots from fromSlot to toSlot inclusive.
    // Including the potential gridline thickness.
    // Returns accumulated approximate height of the non-collapsed slots from fromSlot to toSlot inclusive including the potential gridline thickness.
    double DataGrid::GetSlotElementsHeight(int32_t const fromSlot, int32_t const toSlot) const
    {
        assert(toSlot >= fromSlot);

        auto height = 0.0;
        for (auto slot = GetNextVisibleSlot(fromSlot - 1); slot <= toSlot; slot = GetNextVisibleSlot(slot))
        {
            height += GetSlotElementHeight(slot);
        }

        return height;
    }

    // Checks if the row for the provided dataContext has been generated and is present
    // in either the loaded rows, pre-fetched rows, or editing row.
    // The displayed rows are *not* searched. Returns null if the row does not belong to those 3 categories.
    MiniDumpExplorer::DataGridRow DataGrid::GetGeneratedRow(Windows::Foundation::IInspectable const& dataContext) const
    {
        // Check the list of rows being loaded via the LoadingRow event.
        if (auto dataGridRow = GetLoadedRow(dataContext);
            dataGridRow)
        {
            return dataGridRow;
        }

        // Check the potential editing row.
        if (EditingRow() && dataContext == EditingRow().DataContext())
        {
            return EditingRow();
        }

        // Check the potential focused row.
        if (focusedRow_ && dataContext == focusedRow_.DataContext())
        {
            return focusedRow_;
        }

        return nullptr;
    }

    MiniDumpExplorer::DataGridRow DataGrid::GetLoadedRow(Windows::Foundation::IInspectable const& dataContext) const
    {
        for (auto const& dataGridRow : loadedRows_)
        {
            if (dataGridRow.DataContext() == dataContext)
            {
                return dataGridRow;
            }
        }

        return nullptr;
    }

    FrameworkElement DataGrid::InsertDisplayedElement(int32_t const slot, bool const updateSlotInformation)
    {
        FrameworkElement slotElement{ nullptr };
        if (RowGroupHeadersTable().Contains(slot))
        {
            slotElement = GenerateRowGroupHeader(slot, RowGroupHeadersTable().GetValueAt(slot) /*rowGroupInfo*/);
        }
        else
        {
            // If we're grouping, the GroupLevel needs to be fixed later by methods calling this
            // which end up inserting rows. We don't do it here because elements could be inserted
            // from top to bottom or bottom to up, so it's better to do in one pass
            slotElement = GenerateRow(RowIndexFromSlot(slot), slot);
        }

        InsertDisplayedElement(slot, slotElement, false /*wasNewlyAdded*/, updateSlotInformation);
        return slotElement;
    }

    void DataGrid::InsertDisplayedElement(int32_t const slot, UIElement const& element, bool const wasNewlyAdded, bool const updateSlotInformation)
    {
        // We can only support creating new rows that are adjacent to the currently visible rows
        // since they need to be added to the visual tree for us to Measure them.
        assert(DisplayData().FirstScrollingSlot() == InvalidIndex || (slot >= GetPreviousVisibleSlot(DisplayData().FirstScrollingSlot()) && slot <= GetNextVisibleSlot(DisplayData().LastScrollingSlot())));
        assert(element);

        if (rowsPresenter_)
        {
            MiniDumpExplorer::DataGridRowGroupHeader groupHeader{ nullptr };
            auto const row = element.try_as<DataGridRow>();
            if (row)
            {
                LoadRowVisualsForDisplay(*row);
                [[maybe_unused]] uint32_t index;

                if (IsRowRecyclable(*row))
                {
                    if (!row->IsRecycled())
                    {
                        assert(!rowsPresenter_.Children().IndexOf(element, index));
                        rowsPresenter_.Children().Append(*row);
                    }
                }
                else
                {
                    element.Clip(nullptr);
                    assert(row->Index() == RowIndexFromSlot(slot));
                    if (!rowsPresenter_.Children().IndexOf(*row, index))
                    {
                        rowsPresenter_.Children().Append(*row);
                    }
                }
            }
            else
            {
                groupHeader = element.try_as<MiniDumpExplorer::DataGridRowGroupHeader>();
                assert(groupHeader);
                if (groupHeader)
                {
                    auto const internalGroupHeader = groupHeader.as<DataGridRowGroupHeader>();
                    internalGroupHeader->TotalIndent(groupHeader.Level() == 0 ? 0 : RowGroupSublevelIndents()[groupHeader.Level() - 1]);
                    if (!internalGroupHeader->IsRecycled())
                    {
                        rowsPresenter_.Children().Append(element);
                    }

                    internalGroupHeader->LoadVisualsForDisplay();

                    auto const lastStyle = rowGroupHeaderStyles_.Size() > 0 ? rowGroupHeaderStyles_.GetAt(rowGroupHeaderStyles_.Size() - 1) : nullptr;
                    EnsureElementStyle(groupHeader, groupHeader.Style(), groupHeader.Level() < rowGroupHeaderStyles_.Size() ? rowGroupHeaderStyles_.GetAt(groupHeader.Level()) : lastStyle);
                }
            }

            // Measure the element and update AvailableRowRoom
            element.Measure({ std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity() });
            AvailableSlotElementRoom(AvailableSlotElementRoom() - element.DesiredSize().Height);

            if (groupHeader)
            {
                rowGroupHeightsByLevel_[groupHeader.Level()] = groupHeader.DesiredSize().Height;
            }

            if (row && RowHeightEstimate() == DefaultRowHeight && std::isnan(row->Height()))
            {
                rowHeightEstimate_ = row->DesiredSize().Height - row->DetailsContentHeight();
            }
        }

        if (wasNewlyAdded)
        {
            DisplayData().CorrectSlotsAfterInsertion(slot, element, false /*isCollapsed*/);
        }
        else
        {
            DisplayData().LoadScrollingSlot(slot, element, updateSlotInformation);
        }
    }

    void DataGrid::InsertElement(int32_t const slot, UIElement const& element, bool const updateVerticalScrollBarOnly, bool const isCollapsed, bool const isRow)
    {
        assert(slot != InvalidIndex);
        assert(slot <= SlotCount());

        OnInsertingElement(slot, true /*firstInsertion*/, isCollapsed, isRow);   // will throw an exception if the insertion is illegal

        OnInsertedElement_Phase1(slot, element, isCollapsed, isRow);
        ++slotCount_;;
        if (!isCollapsed)
        {
            ++visibleSlotCount_;
        }

        OnInsertedElement_Phase2(slot, updateVerticalScrollBarOnly, isCollapsed);
    }

    void DataGrid::InvalidateRowHeightEstimate()
    {
        // Start from scratch and assume that we haven't estimated any rows
        lastEstimatedRow_ = InvalidIndex;
    }

    void DataGrid::OnAddedElement_Phase1(int32_t const slot, UIElement const& element)
    {
        assert(slot != InvalidIndex);

        // Row needs to be potentially added to the displayed rows
        if (SlotIsDisplayed(slot))
        {
            InsertDisplayedElement(slot, element, true /*wasNewlyAdded*/, true);
        }
    }

    void DataGrid::OnAddedElement_Phase2(int32_t const slot, bool const updateVerticalScrollBarOnly)
    {
        if (slot < DisplayData().FirstScrollingSlot() - 1)
        {
            // The element was added above our viewport, so it pushes the VerticalOffset down
            auto const elementHeight = RowGroupHeadersTable().Contains(slot) ? RowGroupHeaderHeightEstimate() : RowHeightEstimate();

            SetVerticalOffset(verticalOffset_ + elementHeight);
        }

        if (updateVerticalScrollBarOnly)
        {
            UpdateVerticalScrollBar();
        }
        else
        {
            ComputeScrollBarsLayout();

            // Reposition rows in case we use a recycled one
            InvalidateRowsArrange();
        }
    }

    void DataGrid::OnCollectionViewGroupItemInserted(Windows::Foundation::IInspectable const& groupItems, Windows::Foundation::IInspectable const& insertedItem, int32_t const insertedIndex)
    {
        // We need to figure out the CollectionViewGroup that the sender belongs to.  We could cache
        // it by tagging the collections ahead of time, but I think the extra storage might not be worth
        // it since this lookup should be performant enough
        auto insertSlot = InvalidIndex;
        auto const parentGroupInfo = GetParentGroupInfo(groupItems);
        auto group = insertedItem.try_as<Data::ICollectionViewGroup>();

        if (parentGroupInfo)
        {
            if (group || parentGroupInfo->Level() == InvalidIndex)
            {
                insertSlot = parentGroupInfo->Slot() + 1;

                // For groups, we need to skip over subgroups to find the correct slot
                std::shared_ptr<DataGridInternal::DataGridRowGroupInfo> groupInfo;
                for (int32_t i = 0; i < insertedIndex; ++i)
                {
                    do
                    {
                        insertSlot = RowGroupHeadersTable().GetNextIndex(insertSlot);
                        groupInfo = RowGroupHeadersTable().GetValueAt(insertSlot);
                    } while (groupInfo && groupInfo->Level() > parentGroupInfo->Level() + 1);

                    if (!groupInfo)
                    {
                        insertSlot = SlotCount();
                    }
                }
            }
            else
            {
                // For items the slot is a simple calculation
                insertSlot = parentGroupInfo->Slot() + insertedIndex + 1;
            }
        }

        if (insertSlot != InvalidIndex)
        {
            auto const isCollapsed = parentGroupInfo && (parentGroupInfo->Visibility() == Visibility::Collapsed || collapsedSlotsTable_.Contains(parentGroupInfo->Slot()));
            if (group)
            {
                if (group.GroupItems())
                {
                    HookupVectorChangedListenerToGroup(group.GroupItems());
                }

                auto const newGroupInfo = std::make_shared<DataGridInternal::DataGridRowGroupInfo>(group, Visibility::Visible, parentGroupInfo->Level() + 1, insertSlot, insertSlot);
                InsertElementAt(insertSlot, InvalidIndex /*rowIndex*/, nullptr /*item*/, newGroupInfo, isCollapsed);
                RowGroupHeadersTable().AddValue(insertSlot, newGroupInfo);
            }
            else
            {
                // Assume we're adding a new row
                auto const rowIndex = DataConnection().IndexOf(insertedItem);
                assert(rowIndex != InvalidIndex);
                if (SlotCount() == 0 && DataConnection().ShouldAutoGenerateColumns())
                {
                    AutoGenerateColumnsPrivate();
                }

                InsertElementAt(insertSlot, rowIndex, insertedItem /*item*/, nullptr /*rowGroupInfo*/, isCollapsed);
            }

            CorrectLastSubItemSlotsAfterInsertion(parentGroupInfo);
            if (parentGroupInfo && parentGroupInfo->LastSubItemSlot() - parentGroupInfo->Slot() == 1)
            {
                // We just added the first item to a RowGroup so the header should transition from Empty to either Expanded or Collapsed
                EnsureAncestorsExpanderButtonChecked(*parentGroupInfo);
            }
        }
    }

    void DataGrid::OnCollectionViewGroupItemRemoved(Windows::Foundation::IInspectable const& groupItems, Windows::Foundation::IInspectable const& removedItem, int32_t const removedIndex)
    {
        if (auto const removedGroup = removedItem.try_as<Data::ICollectionViewGroup>();
            removedGroup)
        {
            if (removedGroup.GroupItems())
            {
                UnhookVectorChangedListenerFromGroup(removedGroup.GroupItems(), true /*removeFromTable*/);
            }

            auto const groupInfo = RowGroupInfoFromCollectionViewGroup(removedGroup);
            assert(groupInfo);
            if (auto const removedGroupItems = removedGroup.GroupItems();
                groupInfo->Level() == rowGroupHeightsByLevel_.size() - 1 && removedGroupItems && removedGroupItems.Size() > 0)
            {
                assert(groupInfo->LastSubItemSlot() - groupInfo->Slot() == static_cast<int32_t>(removedGroupItems.Size()));

                // If we're removing a leaf Group then remove all of its items before removing the Group.
                for (uint32_t i = 0; i < removedGroupItems.Size(); ++i)
                {
                    RemoveElementAt(groupInfo->Slot() + 1, removedGroupItems.GetAt(i) /*item*/, true /*isRow*/);
                }
            }

            RemoveElementAt(groupInfo->Slot(), nullptr /*item*/, false /*isRow*/);
        }
        else
        {
            // A single item was removed from a leaf group
            if (auto const parentGroupInfo = GetParentGroupInfo(groupItems);
                parentGroupInfo)
            {
                int32_t slot;
                if (!parentGroupInfo->CollectionViewGroup() && RowGroupHeadersTable().IndexCount() > 0)
                {
                    slot = SlotCount() - 1;
                }
                else
                {
                    slot = parentGroupInfo->Slot() + removedIndex + 1;
                }

                RemoveElementAt(slot, removedItem, true /*isRow*/);
            }
        }
    }

    void DataGrid::OnElementsChanged(bool const grew)
    {
        if (grew &&
            !ColumnsItemsInternal().empty() &&
            CurrentColumnIndex() == InvalidIndex)
        {
            MakeFirstDisplayedCellCurrentCell();
        }
    }

    void DataGrid::OnInsertedElement_Phase1(int32_t const slot, UIElement const& element, bool const isCollapsed, bool const isRow)
    {
        assert(slot != InvalidIndex);

        // Fix the Index of all following rows
        CorrectSlotsAfterInsertion(slot, isCollapsed, isRow);

        // Next, same effect as adding a row
        if (element)
        {
#ifndef NDEBUG
            if (auto const dataGridRow = element.try_as<DataGridRow>();
                dataGridRow)
            {
                assert(dataGridRow->Cells().Count() == ColumnsItemsInternal().size());

                int32_t columnIndex = 0;
                for (auto const& dataGridCell : dataGridRow->Cells().GetCells())
                {
                    auto const internalDataGridCell = dataGridCell.as<DataGridCell>();
                    assert(internalDataGridCell->OwningRow() == *dataGridRow);
                    assert(internalDataGridCell->OwningColumn() == ColumnsItemsInternal()[columnIndex]);
                    ++columnIndex;
                }
            }
#endif
            assert(!isCollapsed);
            OnAddedElement_Phase1(slot, element);
        }
        else if (slot <= DisplayData().FirstScrollingSlot() || (isCollapsed && slot <= DisplayData().LastScrollingSlot()))
        {
            DisplayData().CorrectSlotsAfterInsertion(slot, nullptr /*row*/, isCollapsed);
        }
    }

    void DataGrid::OnInsertedElement_Phase2(int32_t const slot, bool const updateVerticalScrollBarOnly, bool const isCollapsed)
    {
        assert(slot != InvalidIndex);

        if (!isCollapsed)
        {
            // Same effect as adding a row
            OnAddedElement_Phase2(slot, updateVerticalScrollBarOnly);
        }
    }

    void DataGrid::OnInsertingElement(int32_t const slotInserted, bool const firstInsertion, bool const isCollapsed, bool const isRow)
    {
        // Reset the current cell's address if it's after the inserted row.
        if (firstInsertion)
        {
            if (CurrentSlot() != InvalidIndex && slotInserted <= CurrentSlot())
            {
                // The underlying data was already added, therefore we need to avoid accessing any back-end data since we might be off by 1 row.
                temporarilyResetCurrentCell_ = true;
                [[maybe_unused]] auto const success = SetCurrentCellCore(InvalidIndex, InvalidIndex);
                assert(success);
            }
        }

        // Update the slot ranges for the RowGroupHeaders before updating the _selectedItems table,
        // because it's dependent on the slots being correct in regard to grouping.
        RowGroupHeadersTable().InsertIndex(slotInserted);

        selectedItems_.InsertIndex(slotInserted);
        if (isRow)
        {
            // Since details are only visible for rows, the showDetailsTable only contains row indexes.
            if (auto const rowIndex = RowIndexFromSlot(slotInserted);  // NOLINT(bugprone-branch-clone)
                rowIndex == DataConnection().NewItemPlaceholderIndex())
            {
                showDetailsTable_.InsertIndexAndValue(rowIndex, Visibility::Collapsed);
            }
            else
            {
                showDetailsTable_.InsertIndex(rowIndex);
            }
        }

        if (isCollapsed)  // NOLINT(bugprone-branch-clone)
        {
            collapsedSlotsTable_.InsertIndexAndValue(slotInserted, Visibility::Collapsed);
        }
        else
        {
            collapsedSlotsTable_.InsertIndex(slotInserted);
        }

        // If we've inserted rows before the current selected item, update its index
        if (slotInserted <= SelectedIndex())
        {
            SuspendedHandlers::SetValueNoCallback(*this, SelectedIndexProperty(), box_value(SelectedIndex() + 1));
        }
    }

    void DataGrid::OnRemovedElement(int32_t const slotDeleted, Windows::Foundation::IInspectable const& itemDeleted, bool const isRow)
    {
        --slotCount_;
        const auto wasCollapsed = collapsedSlotsTable_.Contains(slotDeleted);
        if (!wasCollapsed)
        {
            --visibleSlotCount_;
        }

        // If we're deleting the focused row, we need to clear the cached value
        if (focusedRow_ && focusedRow_.DataContext() == itemDeleted)
        {
            ResetFocusedRow();
        }

        // The element needs to be potentially removed from the displayed elements
        UIElement elementDeleted{ nullptr };
        if (slotDeleted <= DisplayData().LastScrollingSlot())
        {
            if (slotDeleted >= DisplayData().FirstScrollingSlot() && !wasCollapsed)
            {
                elementDeleted = DisplayData().GetDisplayedElement(slotDeleted);

                // Make sure we have the correct height for the calculation below
                UIHelper::EnsureMeasured(elementDeleted);

                // We need to retrieve the Element before updating the tables, but we need
                // to update the tables before updating DisplayData in RemoveDisplayedElement
                UpdateTablesForRemoval(slotDeleted, itemDeleted);

                // Displayed row is removed
                RemoveDisplayedElement(elementDeleted, slotDeleted, true /*wasDeleted*/, true /*updateSlotInformation*/);
            }
            else
            {
                UpdateTablesForRemoval(slotDeleted, itemDeleted);

                // Removed row is not in view, just update the DisplayData
                DisplayData().CorrectSlotsAfterDeletion(slotDeleted, wasCollapsed);
            }
        }
        else
        {
            // The element was removed beyond the viewport, so we just need to update the tables
            UpdateTablesForRemoval(slotDeleted, itemDeleted);
        }

        // If a row was removed before the currently selected row, update its index
        if (slotDeleted < SelectedIndex())
        {
            SuspendedHandlers::SetValueNoCallback(*this, SelectedIndexProperty(), box_value(SelectedIndex() - 1));
        }

        if (!wasCollapsed)
        {
            if (slotDeleted >= DisplayData().LastScrollingSlot() && !elementDeleted)
            {
                // Deleted Row is below our Viewport, we just need to adjust the scrollbar
                UpdateVerticalScrollBar();
            }
            else
            {
                auto verticalOffsetAdjustment = 0.0;
                if (elementDeleted)
                {
                    // Deleted Row is within our Viewport, update the AvailableRowRoom
                    availableSlotElementRoom_ += elementDeleted.DesiredSize().Height;

                    // When we delete a row in view, we also need to adjust the verticalOffset
                    // in the cases where the deletion causes us to be scrolled further down than
                    // what is possible.
                    if (auto const newVerticalScrollBarMax = vScrollBar_.Maximum() - elementDeleted.DesiredSize().Height;
                        verticalOffset_ > newVerticalScrollBarMax)
                    {
                        verticalOffsetAdjustment = elementDeleted.DesiredSize().Height;
                    }
                }
                else
                {
                    // Deleted element is above our Viewport, update the vertical offset
                    verticalOffsetAdjustment = isRow ? RowHeightEstimate() : RowGroupHeaderHeightEstimate();
                }

                if (verticalOffsetAdjustment > 0)
                {
                    SetVerticalOffset(std::max(0.0, verticalOffset_ - verticalOffsetAdjustment));

                    // If we've adjusted the vertical offset so that is less than the amount that the first element
                    // is covered up, we need to uncover the first element appropriately.
                    if (NegVerticalOffset() > verticalOffset_)
                    {
                        negVerticalOffset_ = verticalOffset_;
                    }
                }

                ComputeScrollBarsLayout();

                // Reposition rows in case we use a recycled one
                InvalidateRowsArrange();
            }
        }
    }

    void DataGrid::OnRemovingElement(int32_t const slotDeleted)
    {
        // Note that the row needs to be deleted no matter what. The underlying data row was already deleted.
        assert(slotDeleted != InvalidIndex);
        assert(slotDeleted < SlotCount());
        temporarilyResetCurrentCell_ = false;

        // Reset the current cell's address if it's on the deleted row, or after it.
        if (CurrentSlot() != InvalidIndex && slotDeleted <= CurrentSlot())
        {
            desiredCurrentColumnIndex_ = CurrentColumnIndex();
            if (slotDeleted == CurrentSlot())
            {
                // No editing is committed since the underlying entity was already deleted.
                [[maybe_unused]] auto const success = SetCurrentCellCore(InvalidIndex, InvalidIndex, false /*commitEdit*/, false /*endRowEdit*/);
                assert(success);
            }
            else
            {
                // Underlying data of deleted row is gone. It cannot be accessed any more. Skip the commit of the editing.
                temporarilyResetCurrentCell_ = true;
                [[maybe_unused]] auto const success = SetCurrentCellCore(InvalidIndex, InvalidIndex);
                assert(success);
            }
        }
    }

    // Makes sure the row shows the proper visuals for selection, currency, details, etc.
    void DataGrid::LoadRowVisualsForDisplay(MiniDumpExplorer::DataGridRow const& row) const
    {
        auto const internalRow = row.as<DataGridRow>();

        // If the row has been recycled, reapply the BackgroundBrush
        if (internalRow->IsRecycled())
        {
            internalRow->EnsureBackground();
            internalRow->EnsureForeground();
            internalRow->ApplyCellsState(false /*animate*/);
        }
        else if (row == EditingRow())
        {
            internalRow->ApplyCellsState(false /*animate*/);
        }

        // Set the Row's Style if we one's defined at the DataGrid level and the user didn't
        // set one at the row level
        EnsureElementStyle(row, nullptr, RowStyle());
        internalRow->EnsureHeaderStyleAndVisibility(nullptr);

        // Check to see if the row contains the CurrentCell, apply its state.
        if (CurrentColumnIndex() != InvalidIndex &&
            CurrentSlot() != InvalidIndex &&
            row.Index() == CurrentSlot())
        {
            internalRow->Cells().GetCells()[CurrentColumnIndex()].as<DataGridCell>()->ApplyCellState(false /*animate*/);
        }

        if (internalRow->IsSelected() || internalRow->IsRecycled())
        {
            internalRow->ApplyState(false);
        }

        // Show or hide RowDetails based on DataGrid settings
        EnsureRowDetailsVisibility(
            row,
            false /*raiseNotification*/);
    }

    void DataGrid::PopulateRowGroupHeadersTable()
    {
        if (DataConnection().CollectionView() &&
            DataConnection().CollectionView().CollectionGroups())
        {
            int32_t totalSlots = 0;
            topLevelGroup_ = DataConnection().CollectionView().CollectionGroups();
            HookupVectorChangedListenerToGroup(topLevelGroup_);
            for (auto const& group : DataConnection().CollectionView().CollectionGroups())
            {
                totalSlots += CountAndPopulateGroupHeaders(group, totalSlots, 0);
            }
        }
    }

    void DataGrid::HookupVectorChangedListenerToGroup(Windows::Foundation::Collections::IObservableVector<Windows::Foundation::IInspectable> const& groupItems)
    {
        if (groupItems && !groupsVectorChangedListenersTable_.contains(groupItems))
        {
            auto weakVectorChangedListener = std::make_shared<Utility::WeakEventListener<MiniDumpExplorer::DataGrid, Windows::Foundation::IInspectable, Windows::Foundation::Collections::IVectorChangedEventArgs>>(*this);
            weakVectorChangedListener->OnEventAction([](MiniDumpExplorer::DataGrid const& instance, Windows::Foundation::IInspectable const& source, Windows::Foundation::Collections::IVectorChangedEventArgs const& eventArgs)
                {
                    instance.as<DataGrid>()->CollectionViewGroupItems_VectorChanged(source.try_as<Windows::Foundation::Collections::IObservableVector<Windows::Foundation::IInspectable>>(), eventArgs);
                });
            auto event = groupItems.VectorChanged({ weakVectorChangedListener.get(), &Utility::WeakEventListener<MiniDumpExplorer::DataGrid, Windows::Foundation::IInspectable, Windows::Foundation::Collections::IVectorChangedEventArgs>::OnEvent });
            weakVectorChangedListener->OnDetachAction([event, groupItems]([[maybe_unused]] auto const& weakEventListener) { groupItems.VectorChanged(event); });

            groupsVectorChangedListenersTable_.insert(std::make_pair(groupItems, weakVectorChangedListener));
        }
    }

    void DataGrid::UnhookVectorChangedListenerFromGroup(Windows::Foundation::Collections::IObservableVector<Windows::Foundation::IInspectable> const& groupItems, bool const removeFromTable)
    {
        if (!groupItems)
        {
            return;
        }

        if (auto const it = groupsVectorChangedListenersTable_.find(groupItems);
            it != groupsVectorChangedListenersTable_.end())
        {
            it->second->Detach();
            if (removeFromTable)
            {
                groupsVectorChangedListenersTable_.erase(it);
            }
        }
    }

    void DataGrid::RefreshRowGroupHeaders()
    {
        if (DataConnection().CollectionView() &&
            DataConnection().CollectionView().CollectionGroups())
        {
            // Initialize our array for the height of the RowGroupHeaders by Level.
            // If the Length is the same, we can reuse the old array.
            constexpr int32_t groupLevelCount = 1;

            if (rowGroupHeightsByLevel_.size() != groupLevelCount)
            {
                rowGroupHeightsByLevel_.resize(groupLevelCount);
                for (int32_t i = 0; i < groupLevelCount; ++i)
                {
                    // Default height for now, the actual heights are updated as the RowGroupHeaders
                    // are added and measured
                    rowGroupHeightsByLevel_[i] = DefaultRowHeight;
                }
            }

            if (rowGroupSublevelIndents_.size() != groupLevelCount)
            {
                rowGroupSublevelIndents_.resize(groupLevelCount);
                for (uint32_t i = 0; i < groupLevelCount; ++i)
                {
                    MiniDumpExplorer::DataGridRowGroupHeader rowGroupHeader{ nullptr };
                    auto indent = DefaultRowGroupSublevelIndent;
                    if (i < RowGroupHeaderStyles().Size() && RowGroupHeaderStyles().GetAt(i))
                    {
                        if (!rowGroupHeader)
                        {
                            rowGroupHeader = MiniDumpExplorer::DataGridRowGroupHeader{};
                        }

                        rowGroupHeader.Style(RowGroupHeaderStyles().GetAt(i));
                        if (rowGroupHeader.SublevelIndent() != DefaultRowGroupSublevelIndent)
                        {
                            indent = rowGroupHeader.SublevelIndent();
                        }
                    }

                    rowGroupSublevelIndents_[i] = indent;
                    if (i > 0)
                    {
                        rowGroupSublevelIndents_[i] += rowGroupSublevelIndents_[i - 1];
                    }
                }
            }

            EnsureRowGroupSpacerColumnWidth(groupLevelCount);
        }
    }

    void DataGrid::RefreshSlotCounts()
    {
        slotCount_ = DataConnection().Count();
        slotCount_ += RowGroupHeadersTable().IndexCount();
        visibleSlotCount_ = SlotCount() - collapsedSlotsTable_.GetIndexCount(0, SlotCount() - 1);
    }

    void DataGrid::RemoveDisplayedElement(int32_t const slot, bool const wasDeleted, bool const updateSlotInformation)
    {
        assert(slot >= DisplayData().FirstScrollingSlot());
        assert(slot <= DisplayData().LastScrollingSlot());

        RemoveDisplayedElement(DisplayData().GetDisplayedElement(slot), slot, wasDeleted, updateSlotInformation);
    }

    // Removes an element from display either because it was deleted or it was scrolled out of view.
    // If the element was provided, it will be the element removed; otherwise, the element will be
    // retrieved from the slot information
    void DataGrid::RemoveDisplayedElement(UIElement const& element, int32_t const slot, bool const wasDeleted, bool const updateSlotInformation)
    {
        if (auto const dataGridRow = element.try_as<DataGridRow>();
            dataGridRow)
        {
            if (IsRowRecyclable(*dataGridRow))
            {
                UnloadRow(*dataGridRow);
            }
            else
            {
                dataGridRow->Clip({});
            }
        }
        else
        {
            if (auto const groupHeader = element.try_as<MiniDumpExplorer::DataGridRowGroupHeader>();
                groupHeader)
            {
                OnUnloadingRowGroup(MiniDumpExplorer::DataGridRowGroupHeaderEventArgs{groupHeader});
                DisplayData().AddRecylableRowGroupHeader(groupHeader);
            }
            else if (uint32_t index = 0; rowsPresenter_ && rowsPresenter_.Children().IndexOf(element, index))
            {
                rowsPresenter_.Children().RemoveAt(index);
            }
        }

        // Update DisplayData
        if (wasDeleted)
        {
            DisplayData().CorrectSlotsAfterDeletion(slot, false /*wasCollapsed*/);
        }
        else
        {
            DisplayData().UnloadScrollingElement(slot, updateSlotInformation, false /*wasDeleted*/);
        }
    }

    // Removes all the editing elements for the row that is just leaving editing mode.
    void DataGrid::RemoveEditingElements() const
    {
        if (EditingRow())
        {
            assert(EditingRow().as<DataGridRow>()->Cells().Count() == ColumnsItemsInternal().size());
            for(auto const& column : Columns())
            {
                column.as<DataGridColumn>()->RemoveEditingElement();
            }
        }
    }

    void DataGrid::RemoveElementAt(int32_t const slot, Windows::Foundation::IInspectable const& item, bool const isRow)
    {
        assert(slot != InvalidIndex);
        assert(slot < SlotCount());

        OnRemovingElement(slot);

        CorrectSlotsAfterDeletion(slot, isRow);

        OnRemovedElement(slot, item, isRow);

        // Synchronize CurrentCellCoordinates, CurrentColumn, CurrentColumnIndex, CurrentItem
        // and CurrentSlot with the currently edited cell, since OnRemovingElement called
        // SetCurrentCellCore(InvalidIndex, InvalidIndex) to temporarily reset the current cell.
        if (auto const internalEditingRow = EditingRow().try_as<DataGridRow>();
            temporarilyResetCurrentCell_ &&
            editingColumnIndex_ != InvalidIndex &&
            previousCurrentItem_ &&
            EditingRow() &&
            internalEditingRow->Slot() != InvalidIndex)
        {
            ProcessSelectionAndCurrency(
                editingColumnIndex_,
                previousCurrentItem_,
                internalEditingRow->Slot(),
                DataGridInternal::DataGridSelectionAction::None,
                false);
        }
    }

    void DataGrid::RemoveNonDisplayedRows(int32_t const newFirstDisplayedSlot, int32_t const newLastDisplayedSlot)
    {
        while (DisplayData().FirstScrollingSlot() < newFirstDisplayedSlot)
        {
            // Need to add rows above the lastDisplayedScrollingRow
            RemoveDisplayedElement(DisplayData().FirstScrollingSlot(), false /*wasDeleted*/, true /*updateSlotInformation*/);
        }

        while (DisplayData().LastScrollingSlot() > newLastDisplayedSlot)
        {
            // Need to remove rows below the lastDisplayedScrollingRow
            RemoveDisplayedElement(DisplayData().LastScrollingSlot(), false /*wasDeleted*/, true /*updateSlotInformation*/);
        }
    }

    void DataGrid::ResetDisplayedRows()
    {
        if (unloadingRowHandler_ || unloadingRowGroupHandler_)
        {
            for(auto const& element : DisplayData().GetScrollingElements())
            {
                // Raise Unloading Row for all the rows we're displaying
                if (auto row = element.try_as<MiniDumpExplorer::DataGridRow>();
                    row)
                {
                    if (IsRowRecyclable(row))
                    {
                        OnUnloadingRow(MiniDumpExplorer::DataGridRowEventArgs{row});
                    }
                }
                else
                {
                    // Raise Unloading Row for all the RowGroupHeaders we're displaying
                    if (auto const groupHeader = element.try_as<MiniDumpExplorer::DataGridRowGroupHeader>();
                        groupHeader)
                    {
                        OnUnloadingRowGroup(MiniDumpExplorer::DataGridRowGroupHeaderEventArgs{groupHeader});
                    }
                }
            }
        }

        DisplayData().ClearElements(true /*recycleRows*/);
        AvailableSlotElementRoom(CellsHeight());
    }

    // Determines whether the row at the provided index must be displayed or not.
    // Returns True when the slot is displayed.
    bool DataGrid::SlotIsDisplayed(int32_t const slot) const
    {
        assert(slot != InvalidIndex);

        if (slot >= DisplayData().FirstScrollingSlot() &&
            slot <= DisplayData().LastScrollingSlot())
        {
            // Additional row takes the spot of a displayed row - it is necessarily displayed
            return true;
        }

        if (DisplayData().FirstScrollingSlot() == InvalidIndex &&
            CellsHeight() > 0 &&
            CellsWidth() > 0)
        {
            return true;
        }

        if (slot == GetNextVisibleSlot(DisplayData().LastScrollingSlot()))
        {
            if (AvailableSlotElementRoom() > 0)
            {
                // There is room for this additional row
                return true;
            }
        }

        return false;
    }

    void DataGrid::ScrollSlotsByHeight(double const height)
    {
        assert(DisplayData().FirstScrollingSlot() != InvalidIndex);
        assert(!double_utils::is_zero(height));

        scrollingByHeight_ = true;
        auto handle = dlg_help_utils::make_scope_exit([this]()
            {
                scrollingByHeight_ = false;
            });

        auto newFirstScrollingSlot = DisplayData().FirstScrollingSlot();
        auto const newVerticalOffset = verticalOffset_ + height;
        if (height > 0)
        {
            // Scrolling Down
            auto const lastVisibleSlot = GetPreviousVisibleSlot(SlotCount());
            if (vScrollBar_ && double_utils::are_close(vScrollBar_.Maximum(), newVerticalOffset))
            {
                // We've scrolled to the bottom of the ScrollBar, automatically place the user at the very bottom
                // of the DataGrid.  If this produces very odd behavior, evaluate the coping strategy used by
                // OnRowMeasure(Size).  For most data, this should be unnoticeable.
                ResetDisplayedRows();
                UpdateDisplayedRowsFromBottom(lastVisibleSlot);
                newFirstScrollingSlot = DisplayData().FirstScrollingSlot();
            }
            else
            {
                if (auto deltaY = GetSlotElementHeight(newFirstScrollingSlot) - NegVerticalOffset();
                    double_utils::less_than(height, deltaY))
                {
                    // We've merely covered up more of the same row we're on
                    negVerticalOffset_ += height;
                }
                else
                {
                    // Figure out what row we've scrolled down to and update the value for this.NegVerticalOffset
                    negVerticalOffset_ = 0;

                    if (height > 2 * CellsHeight() &&
                        (RowDetailsVisibilityMode() != DataGridRowDetailsVisibilityMode::VisibleWhenSelected || !RowDetailsTemplate()))
                    {
                        // Very large scroll occurred. Instead of determining the exact number of scrolled off rows,
                        // let's estimate the number based on this.RowHeight.
                        ResetDisplayedRows();
                        auto const singleRowHeightEstimate = RowHeightEstimate() + (RowDetailsVisibilityMode() == DataGridRowDetailsVisibilityMode::Visible ? RowDetailsHeightEstimate() : 0.0);
                        auto scrolledToSlot = newFirstScrollingSlot + static_cast<int32_t>(height / singleRowHeightEstimate);
                        scrolledToSlot += collapsedSlotsTable_.GetIndexCount(newFirstScrollingSlot, newFirstScrollingSlot + scrolledToSlot);
                        newFirstScrollingSlot = std::min(GetNextVisibleSlot(scrolledToSlot), lastVisibleSlot);
                    }
                    else
                    {
                        while (double_utils::less_than_or_close(deltaY, height))
                        {
                            if (newFirstScrollingSlot < lastVisibleSlot)
                            {
                                if (IsSlotVisible(newFirstScrollingSlot))
                                {
                                    // Make the top row available for reuse
                                    RemoveDisplayedElement(newFirstScrollingSlot, false /*wasDeleted*/, true /*updateSlotInformation*/);
                                }

                                newFirstScrollingSlot = GetNextVisibleSlot(newFirstScrollingSlot);
                            }
                            else
                            {
                                // We're being told to scroll beyond the last row, ignore the extra
                                negVerticalOffset_ = 0;
                                break;
                            }

                            auto const rowHeight = GetExactSlotElementHeight(newFirstScrollingSlot);
                            if (auto const remainingHeight = height - deltaY;
                                double_utils::less_than_or_close(rowHeight, remainingHeight))
                            {
                                deltaY += rowHeight;
                            }
                            else
                            {
                                negVerticalOffset_ = remainingHeight;
                                break;
                            }
                        }
                    }
                }
            }
        }
        else
        {
            // Scrolling Up
            if (double_utils::greater_than_or_close(height + NegVerticalOffset(), 0))
            {
                // We've merely exposing more of the row we're on
                negVerticalOffset_ += height;
            }
            else
            {
                // Figure out what row we've scrolled up to and update the value for this.NegVerticalOffset
                auto deltaY = -NegVerticalOffset();
                negVerticalOffset_ = 0.0;

                if (height < -2 * CellsHeight() &&
                    (RowDetailsVisibilityMode() != DataGridRowDetailsVisibilityMode::VisibleWhenSelected || !RowDetailsTemplate()))
                {
                    // Very large scroll occurred. Instead of determining the exact number of scrolled off rows,
                    // let's estimate the number based on this.RowHeight.
                    if (newVerticalOffset == 0)
                    {
                        newFirstScrollingSlot = 0;
                    }
                    else
                    {
                        auto const singleRowHeightEstimate = RowHeightEstimate() + (RowDetailsVisibilityMode() == DataGridRowDetailsVisibilityMode::Visible ? RowDetailsHeightEstimate() : 0.0);
                        auto scrolledToSlot = newFirstScrollingSlot + static_cast<int32_t>(height / singleRowHeightEstimate);
                        scrolledToSlot -= collapsedSlotsTable_.GetIndexCount(scrolledToSlot, newFirstScrollingSlot);

                        newFirstScrollingSlot = std::max(0, GetPreviousVisibleSlot(scrolledToSlot + 1));
                    }

                    ResetDisplayedRows();
                }
                else
                {
                    auto lastScrollingSlot = DisplayData().LastScrollingSlot();
                    while (double_utils::greater_than(deltaY, height))
                    {
                        if (newFirstScrollingSlot > 0)
                        {
                            if (IsSlotVisible(lastScrollingSlot))
                            {
                                // Make the bottom row available for reuse
                                RemoveDisplayedElement(lastScrollingSlot, false /*wasDeleted*/, true /*updateSlotInformation*/);
                                lastScrollingSlot = GetPreviousVisibleSlot(lastScrollingSlot);
                            }

                            newFirstScrollingSlot = GetPreviousVisibleSlot(newFirstScrollingSlot);
                        }
                        else
                        {
                            negVerticalOffset_ = 0.0;
                            break;
                        }

                        auto const rowHeight = GetExactSlotElementHeight(newFirstScrollingSlot);
                        if (auto const remainingHeight = height - deltaY;
                            double_utils::less_than_or_close(rowHeight + remainingHeight, 0))
                        {
                            deltaY -= rowHeight;
                        }
                        else
                        {
                            negVerticalOffset_ = rowHeight + remainingHeight;
                            break;
                        }
                    }
                }
            }

            if (double_utils::greater_than_or_close(0, newVerticalOffset) && newFirstScrollingSlot != 0)
            {
                // We've scrolled to the top of the ScrollBar, automatically place the user at the very top
                // of the DataGrid.  If this produces very odd behavior, evaluate the RowHeight estimate.
                // strategy. For most data, this should be unnoticeable.
                ResetDisplayedRows();
                negVerticalOffset_ = 0;
                UpdateDisplayedRows(0, CellsHeight());
                newFirstScrollingSlot = 0;
            }
        }

        if (auto const firstRowHeight = GetExactSlotElementHeight(newFirstScrollingSlot);
            double_utils::less_than(firstRowHeight, NegVerticalOffset()))
        {
            // We've scrolled off more of the first row than what's possible.  This can happen
            // if the first row got shorter (Ex: Collapsing RowDetails) or if the user has a recycling
            // cleanup issue.  In this case, simply try to display the next row as the first row instead
            if (newFirstScrollingSlot < SlotCount() - 1)
            {
                newFirstScrollingSlot = GetNextVisibleSlot(newFirstScrollingSlot);
                assert(newFirstScrollingSlot != InvalidIndex);
            }

            negVerticalOffset_ = 0.0;
        }

        UpdateDisplayedRows(newFirstScrollingSlot, CellsHeight());

        if (auto firstElementHeight = GetExactSlotElementHeight(DisplayData().FirstScrollingSlot());
            double_utils::greater_than(NegVerticalOffset(), firstElementHeight))
        {
            auto firstElementSlot = DisplayData().FirstScrollingSlot();

            // We filled in some rows at the top, and now we have a NegVerticalOffset that's greater than the first element
            while (newFirstScrollingSlot > 0 && double_utils::greater_than(NegVerticalOffset(), firstElementHeight))
            {
                if (auto const previousSlot = GetPreviousVisibleSlot(firstElementSlot);
                    previousSlot == InvalidIndex)
                {
                    negVerticalOffset_ = 0.0;
                    VerticalOffset(0);
                }
                else
                {
                    negVerticalOffset_ -= firstElementHeight;
                    VerticalOffset(std::max(0.0, verticalOffset_ - firstElementHeight));
                    firstElementSlot = previousSlot;
                    firstElementHeight = GetExactSlotElementHeight(firstElementSlot);
                }
            }

            // We could be smarter about this, but it's not common, so we wouldn't gain much from optimizing here
            if (firstElementSlot != DisplayData().FirstScrollingSlot())
            {
                UpdateDisplayedRows(firstElementSlot, CellsHeight());
            }
        }

        assert(DisplayData().FirstScrollingSlot() != InvalidIndex);
        assert(GetExactSlotElementHeight(DisplayData().FirstScrollingSlot()) > NegVerticalOffset());

        if (DisplayData().FirstScrollingSlot() == 0)
        {
            VerticalOffset(NegVerticalOffset());
        }
        else if (double_utils::greater_than(NegVerticalOffset(), newVerticalOffset))
        {
            // The scrolled-in row was larger than anticipated. Adjust the DataGrid so the ScrollBar thumb
            // can stay in the same place
            negVerticalOffset_ = newVerticalOffset;
            VerticalOffset(newVerticalOffset);
        }
        else
        {
            VerticalOffset(newVerticalOffset);
        }

        assert(verticalOffset_ != 0 || NegVerticalOffset() != 0 || DisplayData().FirstScrollingSlot() <= 0);

        SetVerticalOffset(verticalOffset_);

        DisplayData().FullyRecycleElements();

        assert(double_utils::greater_than_or_close(NegVerticalOffset(), 0));
        assert(double_utils::greater_than_or_close(verticalOffset_, NegVerticalOffset()));

        if (auto const peer = Automation::Peers::FrameworkElementAutomationPeer::FromElement(*this).try_as<DataGridAutomationPeer>();
            peer)
        {
            peer->RaiseAutomationScrollEvents();
        }
    }

    void DataGrid::SelectDisplayedElement(int32_t const slot)
    {
        assert(IsSlotVisible(slot));
        auto const element = DisplayData().GetDisplayedElement(slot).try_as<FrameworkElement>();
        if (auto const row = DisplayData().GetDisplayedElement(slot).try_as<DataGridRow>();
            row)
        {
            row->ApplyState(true /*animate*/);
            EnsureRowDetailsVisibility(
                *row,
                true /*raiseNotification*/);
        }
        else
        {
            // Assume it's a RowGroupHeader.
            auto const groupHeader = element.try_as<DataGridRowGroupHeader>();
            groupHeader->ApplyState(true /*useTransitions*/);
        }
    }

    void DataGrid::SelectSlot(int32_t const slot, bool const isSelected)
    {
        selectedItems_.SelectSlot(slot, isSelected);
        if (IsSlotVisible(slot))
        {
            SelectDisplayedElement(slot);
        }
    }

    void DataGrid::SelectSlots(int32_t const startSlot, int32_t const endSlot, bool const isSelected)
    {
        selectedItems_.SelectSlots(startSlot, endSlot, isSelected);

        // Apply the correct row state for display rows and also expand or collapse detail accordingly
        auto const firstSlot = std::max(DisplayData().FirstScrollingSlot(), startSlot);
        auto const lastSlot = std::min(DisplayData().LastScrollingSlot(), endSlot);

        for (int32_t slot = firstSlot; slot <= lastSlot; ++slot)
        {
            if (IsSlotVisible(slot))
            {
                SelectDisplayedElement(slot);
            }
        }
    }

    bool DataGrid::ToggleRowGroup()
    {
        if (!ColumnHeaderHasFocus() && FirstVisibleSlot() != InvalidIndex && RowGroupHeadersTable().Contains(CurrentSlot()))
        {
            if (auto const collectionViewGroup = RowGroupHeadersTable().GetValueAt(CurrentSlot())->CollectionViewGroup();
                collectionViewGroup)
            {
                if (auto const dataGridRowGroupInfo = RowGroupInfoFromCollectionViewGroup(collectionViewGroup);
                    dataGridRowGroupInfo)
                {
                    if (dataGridRowGroupInfo->Visibility() == Visibility::Collapsed)
                    {
                        ExpandRowGroup(collectionViewGroup, false /*expandAllSubgroups*/);
                    }
                    else
                    {
                        CollapseRowGroup(collectionViewGroup, false /*collapseAllSubgroups*/);
                    }

                    return true;
                }
            }
        }

        return false;
    }

    void DataGrid::UnloadElements(bool const recycle)
    {
        // Since we're unloading all the elements, we can't be in editing mode any more,
        // so commit if we can, otherwise force cancel.
        if (!CommitEdit())
        {
            CancelEdit(DataGridEditingUnit::Row, false);
        }

        ResetEditingRow();

        // Make sure to clear the focused row (because it's no longer relevant).
        if (focusedRow_)
        {
            ResetFocusedRow();
            [[maybe_unused]] auto focusResult = Focus(FocusState::Programmatic);
        }

        if (rowsPresenter_)
        {
            for(auto const& element : rowsPresenter_.Children())
            {
                if (auto const row = element.try_as<DataGridRow>();
                    row)
                 {
                    // Raise UnloadingRow for any row that was visible
                    if (IsSlotVisible(row->Slot()))
                    {
                        OnUnloadingRow(MiniDumpExplorer::DataGridRowEventArgs{*row});
                    }

                    row->DetachFromDataGrid(recycle && row->IsRecyclable() /*recycle*/);
                }
                else if (auto const groupHeader = element.try_as<DataGridRowGroupHeader>();
                        groupHeader && IsSlotVisible(groupHeader->RowGroupInfo()->Slot()))
                {
                    OnUnloadingRowGroup(MiniDumpExplorer::DataGridRowGroupHeaderEventArgs{*groupHeader});
                }
            }

            if (!recycle)
            {
                rowsPresenter_.Children().Clear();
            }
        }

        DisplayData().ClearElements(recycle);

        // Update the AvailableRowRoom since we're displaying 0 rows now
        AvailableSlotElementRoom(CellsHeight());
        visibleSlotCount_ = 0;
    }

    void DataGrid::UnloadRow(MiniDumpExplorer::DataGridRow const& dataGridRow)
    {
        assert(dataGridRow);
        assert(rowsPresenter_);
        uint32_t index{0};
        [[maybe_unused]] auto const contains = rowsPresenter_.Children().IndexOf(dataGridRow, index);
        assert(contains);

        if (std::ranges::find(loadedRows_, dataGridRow) != loadedRows_.end())
        {
            return; // The row is still referenced, we can't release it.
        }

        // Raise UnloadingRow regardless of whether the row will be recycled
        OnUnloadingRow(MiniDumpExplorer::DataGridRowEventArgs{dataGridRow});

        // TODO: Be able to recycle the current row
        auto recycleRow = CurrentSlot() != dataGridRow.Index();

        // Don't recycle if the row has a custom Style set
        if(!recycleRow)
        {
            auto const style = dataGridRow.Style();
            recycleRow = !style || style == RowStyle();
        }

        if (recycleRow)
        {
            DisplayData().AddRecylableRow(dataGridRow);
        }
        else
        {
            // TODO: Should we raise Unloading for rows that are not going to be recycled as well
            rowsPresenter_.Children().RemoveAt(index);
            dataGridRow.as<DataGridRow>()->DetachFromDataGrid(false);
        }
    }

    void DataGrid::UpdateDisplayedRows(int32_t const newFirstDisplayedSlot, double const displayHeight)
    {
        assert(!collapsedSlotsTable_.Contains(newFirstDisplayedSlot));

        auto firstDisplayedScrollingSlot = newFirstDisplayedSlot;
        auto lastDisplayedScrollingSlot = InvalidIndex;
        auto deltaY = -NegVerticalOffset();
        int32_t visibleScrollingRows = 0;

        if (double_utils::less_than_or_close(displayHeight, 0) || SlotCount() == 0 || ColumnsItemsInternal().size() == 0)
        {
            return;
        }

        if (firstDisplayedScrollingSlot == InvalidIndex)
        {
            // 0 is fine because the element in the first slot cannot be collapsed
            firstDisplayedScrollingSlot = 0;
        }

        auto slot = firstDisplayedScrollingSlot;
        while (slot < SlotCount() && !double_utils::greater_than_or_close(deltaY, displayHeight))
        {
            deltaY += GetExactSlotElementHeight(slot);
            ++visibleScrollingRows;
            lastDisplayedScrollingSlot = slot;
            slot = GetNextVisibleSlot(slot);
        }

        while (double_utils::less_than(deltaY, displayHeight) && slot != InvalidIndex)
        {
            slot = GetPreviousVisibleSlot(firstDisplayedScrollingSlot);
            if (slot != InvalidIndex)
            {
                deltaY += GetExactSlotElementHeight(slot);
                firstDisplayedScrollingSlot = slot;
                ++visibleScrollingRows;
            }
        }

        // If we're up to the first row, and we still have room left, uncover as much of the first row as we can
        if (firstDisplayedScrollingSlot == 0 && double_utils::less_than(deltaY, displayHeight))
        {
            auto const newNegVerticalOffset = std::max(0.0, NegVerticalOffset() - displayHeight + deltaY);
            deltaY += NegVerticalOffset() - newNegVerticalOffset;
            negVerticalOffset_ = newNegVerticalOffset;
        }

        if (double_utils::greater_than(deltaY, displayHeight) || (double_utils::are_close(deltaY, displayHeight) && double_utils::greater_than(NegVerticalOffset(), 0)))
        {
            DisplayData().NumTotallyDisplayedScrollingElements(visibleScrollingRows - 1);
        }
        else
        {
            DisplayData().NumTotallyDisplayedScrollingElements(visibleScrollingRows);
        }

        if (visibleScrollingRows == 0)
        {
            firstDisplayedScrollingSlot = InvalidIndex;
            assert(lastDisplayedScrollingSlot == InvalidIndex);
        }

        assert(lastDisplayedScrollingSlot < SlotCount());

        RemoveNonDisplayedRows(firstDisplayedScrollingSlot, lastDisplayedScrollingSlot);

        assert(DisplayData().NumDisplayedScrollingElements() != InvalidIndex);
        assert(DisplayData().NumTotallyDisplayedScrollingElements() != InvalidIndex);
        assert(DisplayData().FirstScrollingSlot() < SlotCount());
        assert(DisplayData().FirstScrollingSlot() == firstDisplayedScrollingSlot);
        assert(DisplayData().LastScrollingSlot() == lastDisplayedScrollingSlot);
    }

    // Similar to UpdateDisplayedRows except that it starts with the LastDisplayedScrollingRow
    // and computes the FirstDisplayScrollingRow instead of doing it the other way around.  We use this
    // when scrolling down to a full row
    void DataGrid::UpdateDisplayedRowsFromBottom(int32_t const newLastDisplayedScrollingRow)
    {
        assert(!collapsedSlotsTable_.Contains(newLastDisplayedScrollingRow));

        auto lastDisplayedScrollingRow = newLastDisplayedScrollingRow;
        auto firstDisplayedScrollingRow = InvalidIndex;
        auto const displayHeight = CellsHeight();
        auto deltaY = 0.0;
        int32_t visibleScrollingRows = 0;

        if (double_utils::less_than_or_close(displayHeight, 0) || SlotCount() == 0 || ColumnsItemsInternal().empty())
        {
            ResetDisplayedRows();
            return;
        }

        if (lastDisplayedScrollingRow == InvalidIndex)
        {
            lastDisplayedScrollingRow = 0;
        }

        auto slot = lastDisplayedScrollingRow;
        while (double_utils::less_than(deltaY, displayHeight) && slot != InvalidIndex)
        {
            deltaY += GetExactSlotElementHeight(slot);
            ++visibleScrollingRows;
            firstDisplayedScrollingRow = slot;
            slot = GetPreviousVisibleSlot(slot);
        }

        DisplayData().NumTotallyDisplayedScrollingElements(deltaY > displayHeight ? visibleScrollingRows - 1 : visibleScrollingRows);

        assert(lastDisplayedScrollingRow < SlotCount());

        negVerticalOffset_ = std::max(0.0, deltaY - displayHeight);

        RemoveNonDisplayedRows(firstDisplayedScrollingRow, lastDisplayedScrollingRow);

        assert(DisplayData().FirstScrollingSlot() < SlotCount());
    }

    void DataGrid::UpdateRowDetailsHeightEstimate()
    {
        if (rowsPresenter_ && measured_ && RowDetailsTemplate())
        {
            if (auto const detailsContent = RowDetailsTemplate().LoadContent().try_as<FrameworkElement>();
                detailsContent)
            {
                if (VisibleSlotCount() > 0)
                {
                    detailsContent.DataContext(DataConnection().GetDataItem(0));
                }

                rowsPresenter_.Children().Append(detailsContent);

                detailsContent.Measure({ std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity() });
                rowDetailsHeightEstimate_ = detailsContent.DesiredSize().Height;
                if(uint32_t index = 0; rowsPresenter_.Children().IndexOf(detailsContent, index))
                {
                    rowsPresenter_.Children().RemoveAt(index);
                }
            }
        }
    }

    // This method does not check the state of the parent RowGroupHeaders, it assumes they're ready for this newVisibility to
    // be applied this header.
    // Returns the number of pixels that were expanded or (collapsed); however, if we're expanding displayed rows, we only expand up
    // to what we can display.
    double DataGrid::UpdateRowGroupVisibility(DataGridInternal::DataGridRowGroupInfo& targetRowGroupInfo, Microsoft::UI::Xaml::Visibility const newVisibility, bool const isHeaderDisplayed)
    {
        auto heightChange = 0.0;
        int32_t slotsExpanded = 0;
        auto startSlot = targetRowGroupInfo.Slot() + 1;
        int32_t endSlot;

        targetRowGroupInfo.Visibility(newVisibility);
        if (newVisibility == Visibility::Visible)
        {
            // Expand
            for(auto const slot : RowGroupHeadersTable().GetIndexes(targetRowGroupInfo.Slot() + 1))
            {
                if (slot >= startSlot)
                {
                    auto const rowGroupInfo = RowGroupHeadersTable().GetValueAt(slot);
                    if (rowGroupInfo->Level() <= targetRowGroupInfo.Level())
                    {
                        break;
                    }

                    if (rowGroupInfo->Visibility() == Visibility::Collapsed)
                    {
                        // Skip over the items in collapsed subgroups
                        endSlot = rowGroupInfo->Slot();
                        ExpandSlots(startSlot, endSlot, isHeaderDisplayed, slotsExpanded, heightChange);
                        startSlot = rowGroupInfo->LastSubItemSlot() + 1;
                    }
                }
            }

            if (targetRowGroupInfo.LastSubItemSlot() >= startSlot)
            {
                ExpandSlots(startSlot, targetRowGroupInfo.LastSubItemSlot(), isHeaderDisplayed, slotsExpanded, heightChange);
            }

            if (isHeaderDisplayed)
            {
                UpdateDisplayedRows(DisplayData().FirstScrollingSlot(), CellsHeight());
            }
        }
        else
        {
            // Collapse
            endSlot = SlotCount() - 1;

            if (DataConnection().IsAddingNew())
            {
                --endSlot;
            }

            for(auto const slot : RowGroupHeadersTable().GetIndexes(targetRowGroupInfo.Slot() + 1))
            {
                if (auto const rowGroupInfo = RowGroupHeadersTable().GetValueAt(slot);
                    rowGroupInfo->Level() <= targetRowGroupInfo.Level())
                {
                    endSlot = slot - 1;
                    break;
                }
            }

            auto const oldLastDisplayedSlot = DisplayData().LastScrollingSlot();

            // onlyChildrenDisplayed is true if the RowGroupHeader is not displayed but some of its children are
            auto const onlyChildrenDisplayed = !isHeaderDisplayed && endSlot >= DisplayData().FirstScrollingSlot();
            auto partialGroupDisplayedHeight = 0.0;
            if (isHeaderDisplayed || onlyChildrenDisplayed)
            {
                // If the RowGroupHeader is displayed or its children are partially displayed,
                // we need to remove all the displayed slots that aren't already collapsed
                auto startDisplayedSlot = std::max(startSlot, DisplayData().FirstScrollingSlot());
                auto const endDisplayedSlot = std::min(endSlot, DisplayData().LastScrollingSlot());

                auto const elementsToRemove = endDisplayedSlot - startDisplayedSlot + 1 - collapsedSlotsTable_.GetIndexCount(startDisplayedSlot, endDisplayedSlot);
                if (auto const internalFocusedRow = focusedRow_.try_as<DataGridRow>();
                    internalFocusedRow && internalFocusedRow->Slot() >= startSlot && internalFocusedRow->Slot() <= endSlot)
                {
                    assert(!EditingRow());

                    // Don't call ResetFocusedRow here because we're already cleaning it up below, and we don't want to FullyRecycle yet
                    focusedRow_ = nullptr;
                }

                for (int32_t i = 0; i < elementsToRemove; ++i)
                {
                    auto const displayedElement = DisplayData().GetDisplayedElement(startDisplayedSlot);
                    UIHelper::EnsureMeasured(displayedElement);

                    // For partially displayed groups, we need to update the slot information right away.  For groups
                    // where the RowGroupHeader is displayed, we can just mark them collapsed later.
                    RemoveDisplayedElement(displayedElement, startDisplayedSlot, false /*wasDeleted*/, onlyChildrenDisplayed /*updateSlotInformation*/);
                    if (onlyChildrenDisplayed)
                    {
                        startDisplayedSlot = DisplayData().FirstScrollingSlot();
                        partialGroupDisplayedHeight += displayedElement.DesiredSize().Height;
                    }
                }

                if (onlyChildrenDisplayed)
                {
                    partialGroupDisplayedHeight -= NegVerticalOffset();
                }
            }

            // If part of the group we collapsed was partially displayed, we only collapsed the amount that was not displayed.
            heightChange += partialGroupDisplayedHeight;

            auto heightChangeBelowLastDisplayedSlot = 0.0;
            if (DisplayData().FirstScrollingSlot() >= startSlot && DisplayData().FirstScrollingSlot() <= endSlot)
            {
                // Our first visible slot was collapsed, find the replacement
                auto collapsedSlotsAbove = DisplayData().FirstScrollingSlot() - startSlot - collapsedSlotsTable_.GetIndexCount(startSlot, DisplayData().FirstScrollingSlot());
                assert(collapsedSlotsAbove > 0);
                auto newFirstScrollingSlot = GetNextVisibleSlot(DisplayData().FirstScrollingSlot());
                while (collapsedSlotsAbove > 1 && newFirstScrollingSlot < SlotCount())
                {
                    --collapsedSlotsAbove;
                    newFirstScrollingSlot = GetNextVisibleSlot(newFirstScrollingSlot);
                }

                heightChange += CollapseSlotsInTable(startSlot, endSlot, slotsExpanded, oldLastDisplayedSlot, heightChangeBelowLastDisplayedSlot);
                if (isHeaderDisplayed || onlyChildrenDisplayed)
                {
                    if (newFirstScrollingSlot >= SlotCount())
                    {
                        // No visible slots below, look up
                        UpdateDisplayedRowsFromBottom(targetRowGroupInfo.Slot());
                    }
                    else
                    {
                        UpdateDisplayedRows(newFirstScrollingSlot, CellsHeight());
                    }
                }
            }
            else
            {
                heightChange += CollapseSlotsInTable(startSlot, endSlot, slotsExpanded, oldLastDisplayedSlot, heightChangeBelowLastDisplayedSlot);
            }

            if (DisplayData().LastScrollingSlot() >= startSlot && DisplayData().LastScrollingSlot() <= endSlot)
            {
                // Collapsed the last scrolling row, we need to update it
                DisplayData().LastScrollingSlot(GetPreviousVisibleSlot(DisplayData().LastScrollingSlot()));
            }

            // Collapsing could cause the vertical offset to move up if we collapsed a lot of slots
            // near the bottom of the DataGrid.  To do this, we compare the height we collapsed to
            // the distance to the last visible row and adjust the scrollbar if we collapsed more
            if (isHeaderDisplayed && verticalOffset_ > 0)
            {
                auto const lastVisibleSlot = GetPreviousVisibleSlot(SlotCount());
                auto slot = GetNextVisibleSlot(oldLastDisplayedSlot);

                // AvailableSlotElementRoom ends up being the amount of the last slot that is partially scrolled off
                // as a negative value, heightChangeBelowLastDisplayed slot is also a negative value since we're collapsing
                auto heightToLastVisibleSlot = AvailableSlotElementRoom() + heightChangeBelowLastDisplayedSlot;
                while (heightToLastVisibleSlot > heightChange && slot < lastVisibleSlot)
                {
                    heightToLastVisibleSlot -= GetSlotElementHeight(slot);
                    slot = GetNextVisibleSlot(slot);
                }

                if (heightToLastVisibleSlot > heightChange)
                {
                    if (auto const newVerticalOffset = verticalOffset_ + heightChange - heightToLastVisibleSlot;
                        newVerticalOffset > 0)
                    {
                        SetVerticalOffset(newVerticalOffset);
                    }
                    else
                    {
                        // Collapsing causes the vertical offset to go to 0, so we should go back to the first row.
                        ResetDisplayedRows();
                        negVerticalOffset_ = 0;
                        SetVerticalOffset(0);
                        auto const firstDisplayedRow = GetNextVisibleSlot(InvalidIndex);
                        UpdateDisplayedRows(firstDisplayedRow, CellsHeight());
                    }
                }
            }
        }

        // Update VisibleSlotCount
        visibleSlotCount_ += slotsExpanded;

        return heightChange;
    }

    void DataGrid::UpdateTablesForRemoval(int32_t const slotDeleted, Windows::Foundation::IInspectable const& itemDeleted)
    {
        if (RowGroupHeadersTable().Contains(slotDeleted))
        {
            // A RowGroupHeader was removed
            RowGroupHeadersTable().RemoveIndexAndValue(slotDeleted);
            collapsedSlotsTable_.RemoveIndexAndValue(slotDeleted);
            selectedItems_.DeleteSlot(slotDeleted);
        }
        else
        {
            // Update the ranges of selected rows
            if (selectedItems_.ContainsSlot(slotDeleted))
            {
                SelectionHasChanged(true);
            }

            selectedItems_.Delete(slotDeleted, itemDeleted);
            showDetailsTable_.RemoveIndex(RowIndexFromSlot(slotDeleted));
            RowGroupHeadersTable().RemoveIndex(slotDeleted);
            collapsedSlotsTable_.RemoveIndex(slotDeleted);
        }
    }
}
