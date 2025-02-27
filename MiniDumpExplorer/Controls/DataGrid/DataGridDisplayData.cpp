#include "pch.h"
#include "DataGridDisplayData.h"

#include "DataGrid.h"
#include "DataGridRow.h"
#include "DataGridRowGroupHeader.h"

using namespace winrt::Microsoft::UI::Xaml;
using namespace winrt::MiniDumpExplorer;
using namespace winrt;

namespace DataGridInternal
{
    DataGridDisplayData::DataGridDisplayData(implementation::DataGrid* owner)
        : owner_{owner}
    {
        ResetSlotIndexes();
    }

    void DataGridDisplayData::AddRecylableRow(DataGridRow const& row)
    {
        assert(std::ranges::find(recyclableRows_, row) == recyclableRows_.end());

        row.as<implementation::DataGridRow>()->DetachFromDataGrid(true);
        recyclableRows_.push_back(row);
    }

    void DataGridDisplayData::AddRecylableRowGroupHeader(DataGridRowGroupHeader const& groupHeader)
    {
        assert(std::ranges::find(recyclableGroupHeaders_, groupHeader) == recyclableGroupHeaders_.end());

        auto const internalGroupHeader = groupHeader.as<implementation::DataGridRowGroupHeader>();
        internalGroupHeader->PropertyName({});
        internalGroupHeader->PropertyValue({});
        internalGroupHeader->IsRecycled( true);
        recyclableGroupHeaders_.push_back(groupHeader);
    }

    void DataGridDisplayData::ClearElements(bool const recycle)
    {
        ResetSlotIndexes();
        if (recycle)  // NOLINT(bugprone-branch-clone)
        {
            for(auto const& element : scrollingElements_)
            {
                if (auto const row = element.try_as<implementation::DataGridRow>();
                    row)
                {
                    if (row->IsRecyclable())
                    {
                        AddRecylableRow(*row);
                    }
                    else
                    {
                        row->Clip({});
                    }
                }
                else if (auto const groupHeader = element.try_as<DataGridRowGroupHeader>();
                    groupHeader)
                {
                    AddRecylableRowGroupHeader(groupHeader);
                }
            }
        }
        else
        {
            recyclableRows_.clear();
            fullyRecycledRows_.clear();
            recyclableGroupHeaders_.clear();
            fullyRecycledGroupHeaders_.clear();
        }

        scrollingElements_.clear();
    }

    void DataGridDisplayData::CorrectSlotsAfterDeletion(int32_t const slot, bool const wasCollapsed)
    {
        if (wasCollapsed)
        {
            if (slot > FirstScrollingSlot())
            {
                --lastScrollingSlot_;
            }
        }
        else if (owner_->IsSlotVisible(slot))
        {
            UnloadScrollingElement(slot, true /*updateSlotInformation*/, true /*wasDeleted*/);
        }

        // This cannot be an else condition because if there are 2 rows left, and you delete the first one
        // then these indexes need to be updated as well
        if (slot < FirstScrollingSlot())
        {
            --firstScrollingSlot_;
            --lastScrollingSlot_;
        }
    }

    void DataGridDisplayData::CorrectSlotsAfterInsertion(int32_t const slot, UIElement const& element, bool const isCollapsed)
    {
        if (slot < FirstScrollingSlot())
        {
            // The row was inserted above our viewport, just update our indexes
            ++firstScrollingSlot_;
            ++lastScrollingSlot_;
        }
        else if (isCollapsed && (slot <= LastScrollingSlot()))
        {
            ++lastScrollingSlot_;
        }
        else if (owner_->GetPreviousVisibleSlot(slot) <= LastScrollingSlot() || LastScrollingSlot() == InvalidIndex)
        {
            assert(element);

            // The row was inserted in our viewport, add it as a scrolling row
            LoadScrollingSlot(slot, element, true /*updateSlotInformation*/);
        }
    }

    void DataGridDisplayData::FullyRecycleElements()
    {
        // Fully recycle Recycleable rows and transfer them to Recycled rows
        while (recyclableRows_.size() > 0)
        {
            auto row = recyclableRows_.back();
            recyclableRows_.pop_back();
            assert(row);
            row.Visibility(Visibility::Collapsed);
            assert(std::ranges::find(fullyRecycledRows_, row) == fullyRecycledRows_.end());
            fullyRecycledRows_.push_back(row);
        }

        // Fully recycle recyclable GroupHeaders and transfer them to Recycled GroupHeaders
        while (recyclableGroupHeaders_.size() > 0)
        {
            auto groupHeader = recyclableGroupHeaders_.back();
            recyclableGroupHeaders_.pop_back();
            assert(groupHeader);
            groupHeader.Visibility(Visibility::Collapsed);
            assert(std::ranges::find(fullyRecycledGroupHeaders_, groupHeader) == fullyRecycledGroupHeaders_.end());
            fullyRecycledGroupHeaders_.push_back(groupHeader);
        }
    }

    UIElement DataGridDisplayData::GetDisplayedElement(int32_t const slot) const
    {
        assert(slot >= FirstScrollingSlot());
        assert(slot <= LastScrollingSlot());

        return scrollingElements_[GetCircularListIndex(slot, true /*wrap*/)];
    }

    DataGridRow DataGridDisplayData::GetDisplayedRow(int32_t const rowIndex) const
    {
        return GetDisplayedElement(owner_->SlotFromRowIndex(rowIndex)).try_as<DataGridRow>();
    }

    dlg_help_utils::generator<UIElement> DataGridDisplayData::GetScrollingElements(bool const onlyRows) const
    {
        for (size_t i = 0; i < scrollingElements_.size(); i++)
        {
            if (auto element = scrollingElements_[(headScrollingElements_ + i) % scrollingElements_.size()];
                !onlyRows || element.try_as<DataGridRow>())
            {
                // _scrollingRows is a circular list that wraps
                co_yield element;
            }
        }
    }

    DataGridRowGroupHeader DataGridDisplayData::GetUsedGroupHeader()
    {
        if (!recyclableGroupHeaders_.empty())
        {
            auto header = recyclableGroupHeaders_.back();
            recyclableGroupHeaders_.pop_back();
            return header;
        }

        if (!fullyRecycledGroupHeaders_.empty())
        {
            // For fully recycled rows, we need to set the Visibility back to Visible
            auto groupHeader = fullyRecycledGroupHeaders_.back();
            fullyRecycledGroupHeaders_.pop_back();
            groupHeader.Visibility(Visibility::Visible);
            return groupHeader;
        }

        return nullptr;
    }

    DataGridRow DataGridDisplayData::GetUsedRow()
    {
        if (!recyclableRows_.empty())
        {
            auto row = recyclableRows_.back();
            recyclableRows_.pop_back();
            return row;
        }

        if (!fullyRecycledRows_.empty())
        {
            // For fully recycled rows, we need to set the Visibility back to Visible
            auto row = fullyRecycledRows_.back();
            fullyRecycledRows_.pop_back();
            row.Visibility(Visibility::Visible);
            return row;
        }

        return nullptr;
    }

    void DataGridDisplayData::LoadScrollingSlot(int32_t const slot, UIElement const& element, bool const updateSlotInformation)
    {
        if (scrollingElements_.empty())
        {
            SetScrollingSlots(slot);
            scrollingElements_.push_back(element);
        }
        else
        {
            // The slot should be adjacent to the other slots being displayed
            assert(slot >= owner_->GetPreviousVisibleSlot(FirstScrollingSlot()));
            assert(slot <= owner_->GetNextVisibleSlot(LastScrollingSlot()));
            if (updateSlotInformation)
            {
                if (slot < FirstScrollingSlot())
                {
                    firstScrollingSlot_ = slot;
                }
                else
                {
                    lastScrollingSlot_ = owner_->GetNextVisibleSlot(LastScrollingSlot());
                }
            }

            auto insertIndex = GetCircularListIndex(slot, false /*wrap*/);
            if (insertIndex > scrollingElements_.size())
            {
                // We need to wrap around from the bottom to the top of our circular list; as a result the head of the list moves forward
                insertIndex -= static_cast<int32_t>(scrollingElements_.size());
                headScrollingElements_++;
            }

            scrollingElements_.insert(scrollingElements_.begin() + insertIndex, element);
        }
    }

    void DataGridDisplayData::UnloadScrollingElement(int32_t const slot, bool const updateSlotInformation, bool const wasDeleted)
    {
        assert(owner_->IsSlotVisible(slot));

        auto elementIndex = GetCircularListIndex(slot, false /*wrap*/);
        if (elementIndex > scrollingElements_.size())
        {
            // We need to wrap around from the top to the bottom of our circular list
            elementIndex -= static_cast<int32_t>(scrollingElements_.size());
            headScrollingElements_--;
        }

        scrollingElements_.erase(scrollingElements_.begin() + elementIndex);

        if (updateSlotInformation)
        {
            if (slot == FirstScrollingSlot() && !wasDeleted)
            {
                firstScrollingSlot_ = owner_->GetNextVisibleSlot(FirstScrollingSlot());
            }
            else
            {
                lastScrollingSlot_ = owner_->GetPreviousVisibleSlot(LastScrollingSlot());
            }

            if (lastScrollingSlot_ < FirstScrollingSlot())
            {
                ResetSlotIndexes();
            }
        }
    }

    int32_t DataGridDisplayData::GetCircularListIndex(int32_t const slot, bool const wrap) const
    {
        auto const index = slot - FirstScrollingSlot() - headScrollingElements_ - owner_->GetCollapsedSlotCount(FirstScrollingSlot(), slot);
        return wrap ? index % static_cast<int32_t>(scrollingElements_.size()) : index;
    }

    void DataGridDisplayData::ResetSlotIndexes()
    {
        SetScrollingSlots(InvalidIndex);
        numTotallyDisplayedScrollingElements_ = 0;
        headScrollingElements_ = 0;
    }

    void DataGridDisplayData::SetScrollingSlots(int32_t const newValue)
    {
        firstScrollingSlot_ = newValue;
        lastScrollingSlot_ = newValue;
    }
}
