#include "pch.h"
#include "DataGridSelectedItemsCollection.h"

#include "DataGrid.h"
#include "DataGridError.h"
#include "DataGridDataConnection.h"
#include "Utility/erase_value.h"
#include "Utility/IndexToValueTable.h"

using namespace std::string_view_literals;

namespace DataGridInternal
{
    struct DataGridSelectedItemsCollection::Impl
    {
        Impl(winrt::MiniDumpExplorer::implementation::DataGrid* owningGrid)
            : owningGrid_(owningGrid)
        {
        }

        winrt::MiniDumpExplorer::implementation::DataGrid* owningGrid_;
        std::vector<winrt::Windows::Foundation::IInspectable> oldSelectedItemsCache_;
        Utility::IndexToValueTable<bool> oldSelectedSlotsTable_;
        std::vector<winrt::Windows::Foundation::IInspectable> selectedItemsCache_;
        Utility::IndexToValueTable<bool> selectedSlotsTable_;
    };


    DataGridSelectedItemsCollection::DataGridSelectedItemsCollection(winrt::MiniDumpExplorer::implementation::DataGrid* owningGrid)
        : impl_{std::make_unique<Impl>(owningGrid)}
    {
    }

    DataGridSelectedItemsCollection::~DataGridSelectedItemsCollection() = default;

    winrt::Windows::Foundation::IInspectable DataGridSelectedItemsCollection::operator[](int32_t const index) const
    {
        if (index >= impl_->selectedSlotsTable_.IndexCount())
        {
            throw DataGridError::ValueMustBeBetween(L"index"sv, L"Index"sv, 0, true, impl_->selectedSlotsTable_.IndexCount(), false);
        }

        auto const slot = impl_->selectedSlotsTable_.GetNthIndex(index);
        return impl_->owningGrid_->DataConnection().GetDataItem(impl_->owningGrid_->RowIndexFromSlot(slot));
    }

    int32_t DataGridSelectedItemsCollection::Add(winrt::Windows::Foundation::IInspectable const& dataItem) const
    {
        if (impl_->owningGrid_->SelectionMode() == winrt::MiniDumpExplorer::DataGridSelectionMode::Single)
        {
            throw DataGridError::DataGridSelectedItemsCollection::CannotChangeSelectedItemsCollectionInSingleMode();
        }

        auto const itemIndex = impl_->owningGrid_->DataConnection().IndexOf(dataItem);
        if (itemIndex == InvalidIndex)
        {
            throw DataGridError::DataGrid::ItemIsNotContainedInTheItemsSource(L"dataItem"sv);
        }

        const auto slot = impl_->owningGrid_->SlotFromRowIndex(itemIndex);
        impl_->selectedSlotsTable_.RangeCount() == 0
            ? impl_->owningGrid_->SelectedItem(dataItem)
            : impl_->owningGrid_->SetRowSelection(slot, true /*isSelected*/, false /*setAnchorSlot*/);
        return impl_->selectedSlotsTable_.IndexOf(slot);
    }

    void DataGridSelectedItemsCollection::Clear() const
    {
        if (impl_->owningGrid_->SelectionMode() == winrt::MiniDumpExplorer::DataGridSelectionMode::Single)
        {
            throw DataGridError::DataGridSelectedItemsCollection::CannotChangeSelectedItemsCollectionInSingleMode();
        }

        if (impl_->selectedSlotsTable_.RangeCount() > 0)
        {
            // Clearing the selection does not reset the potential current cell.
            if (!impl_->owningGrid_->CommitEdit(winrt::MiniDumpExplorer::DataGridEditingUnit::Row, true /*exitEditing*/))
            {
                // Edited value couldn't be committed or aborted
                return;
            }

            impl_->owningGrid_->ClearRowSelection(true /*resetAnchorSlot*/);
        }
    }

    bool DataGridSelectedItemsCollection::Contains(winrt::Windows::Foundation::IInspectable const& dataItem) const
    {
        auto const itemIndex = impl_->owningGrid_->DataConnection().IndexOf(dataItem);
        if (itemIndex == InvalidIndex)
        {
            return false;
        }

        return ContainsSlot(impl_->owningGrid_->SlotFromRowIndex(itemIndex));
    }

    int32_t DataGridSelectedItemsCollection::IndexOf(winrt::Windows::Foundation::IInspectable const& dataItem) const
    {
        auto const itemIndex = impl_->owningGrid_->DataConnection().IndexOf(dataItem);
        if (itemIndex == InvalidIndex)
        {
            return InvalidIndex;
        }

        auto const slot = impl_->owningGrid_->SlotFromRowIndex(itemIndex);
        return impl_->selectedSlotsTable_.IndexOf(slot);
    }

    void DataGridSelectedItemsCollection::Remove(winrt::Windows::Foundation::IInspectable const& dataItem) const
    {
        if (impl_->owningGrid_->SelectionMode() == winrt::MiniDumpExplorer::DataGridSelectionMode::Single)
        {
            throw DataGridError::DataGridSelectedItemsCollection::CannotChangeSelectedItemsCollectionInSingleMode();
        }

        auto const itemIndex = impl_->owningGrid_->DataConnection().IndexOf(dataItem);
        if (itemIndex == InvalidIndex)
        {
            return;
        }

        if (itemIndex == impl_->owningGrid_->CurrentSlot() &&
            !impl_->owningGrid_->CommitEdit(winrt::MiniDumpExplorer::DataGridEditingUnit::Row, true /*exitEditing*/))
        {
            // Edited value couldn't be committed or aborted
            return;
        }

        impl_->owningGrid_->SetRowSelection(itemIndex, false /*isSelected*/, false /*setAnchorSlot*/);
    }

    void DataGridSelectedItemsCollection::RemoveAt(int32_t const index) const
    {
        if (impl_->owningGrid_->SelectionMode() == winrt::MiniDumpExplorer::DataGridSelectionMode::Single)
        {
            throw DataGridError::DataGridSelectedItemsCollection::CannotChangeSelectedItemsCollectionInSingleMode();
        }

        if (index < 0 || index >= impl_->selectedSlotsTable_.IndexCount())
        {
            throw DataGridError::ValueMustBeBetween(L"index"sv, L"Index"sv, 0, true, impl_->selectedSlotsTable_.IndexCount(), false);
        }

        auto const rowIndex = impl_->selectedSlotsTable_.GetNthIndex(index);

        if (rowIndex == impl_->owningGrid_->CurrentSlot() &&
            !impl_->owningGrid_->CommitEdit(winrt::MiniDumpExplorer::DataGridEditingUnit::Row, true /*exitEditing*/))
        {
            // Edited value couldn't be committed or aborted
            return;
        }

        impl_->owningGrid_->SetRowSelection(rowIndex, false /*isSelected*/, false /*setAnchorSlot*/);
    }

    int32_t DataGridSelectedItemsCollection::Count() const
    {
        return impl_->selectedSlotsTable_.IndexCount();
    }

    std::generator<winrt::Windows::Foundation::IInspectable> DataGridSelectedItemsCollection::Items() const
    {
        for(auto const slot : impl_->selectedSlotsTable_.GetIndexes())
        {
            auto const rowIndex = impl_->owningGrid_->RowIndexFromSlot(slot);
            co_yield impl_->owningGrid_->DataConnection().GetDataItem(rowIndex);
        }
    }

    winrt::MiniDumpExplorer::DataGrid DataGridSelectedItemsCollection::OwningGrid() const
    {
        return *impl_->owningGrid_;
    }

    std::vector<winrt::Windows::Foundation::IInspectable> const& DataGridSelectedItemsCollection::SelectedItemsCache() const
    {
        return impl_->selectedItemsCache_;
    }

    std::vector<winrt::Windows::Foundation::IInspectable>& DataGridSelectedItemsCollection::SelectedItemsCache()
    {
        return impl_->selectedItemsCache_;
    }

    // ReSharper disable once CppMemberFunctionMayBeConst
    void DataGridSelectedItemsCollection::ClearRows()
    {
        impl_->selectedSlotsTable_.Clear();
        impl_->selectedItemsCache_.clear();
    }

    bool DataGridSelectedItemsCollection::ContainsSlot(int32_t const slot) const
    {
        return impl_->selectedSlotsTable_.Contains(slot);
    }

    bool DataGridSelectedItemsCollection::ContainsAll(int32_t const startSlot, int32_t const endSlot) const
    {
        auto itemSlot = impl_->owningGrid_->RowGroupHeadersTable().GetNextGap(startSlot - 1);
        while (itemSlot <= endSlot)
        {
            // Skip over the RowGroupHeaderSlots
            auto const nextRowGroupHeaderSlot = impl_->owningGrid_->RowGroupHeadersTable().GetNextIndex(itemSlot);
            auto const lastItemSlot = nextRowGroupHeaderSlot == InvalidIndex ? endSlot : std::min<>(endSlot, nextRowGroupHeaderSlot - 1);
            if (!impl_->selectedSlotsTable_.ContainsAll(itemSlot, lastItemSlot))
            {
                return false;
            }

            itemSlot = impl_->owningGrid_->RowGroupHeadersTable().GetNextGap(lastItemSlot);
        }

        return true;
    }

    void DataGridSelectedItemsCollection::Delete(int32_t const slot, winrt::Windows::Foundation::IInspectable const& item)
    {
        if (impl_->oldSelectedSlotsTable_.Contains(slot))
        {
            impl_->owningGrid_->SelectionHasChanged(true);
        }

        DeleteSlot(slot);
        Utility::erase_value(impl_->selectedItemsCache_, item);
    }

    // ReSharper disable once CppMemberFunctionMayBeConst
    void DataGridSelectedItemsCollection::DeleteSlot(int32_t const slot)
    {
        impl_->selectedSlotsTable_.RemoveIndex(slot);
        impl_->oldSelectedSlotsTable_.RemoveIndex(slot);
    }

    int32_t DataGridSelectedItemsCollection::GetIndexCount(int32_t const lowerBound, int32_t const upperBound) const
    {
        return impl_->selectedSlotsTable_.GetIndexCount(lowerBound, upperBound, true);
    }

    std::generator<int32_t> DataGridSelectedItemsCollection::GetIndexes() const
    {
        return impl_->selectedSlotsTable_.GetIndexes();
    }

    std::generator<int32_t> DataGridSelectedItemsCollection::GetSlots(int32_t const startSlot) const
    {
        return impl_->selectedSlotsTable_.GetIndexes(startSlot);
    }

    // ReSharper disable once CppMemberFunctionMayBeConst
    winrt::Microsoft::UI::Xaml::Controls::SelectionChangedEventArgs DataGridSelectedItemsCollection::GetSelectionChangedEventArgs()
    {
        auto addedSelectedItems = winrt::Windows::Foundation::Collections::IVector(winrt::single_threaded_vector<winrt::Windows::Foundation::IInspectable>());
        auto removedSelectedItems = winrt::Windows::Foundation::Collections::IVector(winrt::single_threaded_vector<winrt::Windows::Foundation::IInspectable>());

        // Compare the old selected indexes with the current selection to determine which items
        // have been added and removed since the last time this method was called
        for(auto const newSlot : impl_->selectedSlotsTable_.GetIndexes())
        {
            auto newItem = impl_->owningGrid_->DataConnection().GetDataItem(impl_->owningGrid_->RowIndexFromSlot(newSlot));
            if (impl_->oldSelectedSlotsTable_.Contains(newSlot))
            {
                impl_->oldSelectedSlotsTable_.RemoveValue(newSlot);
                Utility::erase_value(impl_->oldSelectedItemsCache_, newItem);
            }
            else
            {
                addedSelectedItems.Append(newItem);
            }
        }

        for(winrt::Windows::Foundation::IInspectable const& oldItem : impl_->oldSelectedItemsCache_)
        {
            removedSelectedItems.Append(oldItem);
        }

        // The current selection becomes the old selection
        impl_->oldSelectedSlotsTable_ = impl_->selectedSlotsTable_.Copy();
        impl_->oldSelectedItemsCache_.clear();
        impl_->oldSelectedItemsCache_.insert(impl_->oldSelectedItemsCache_.end(), impl_->selectedItemsCache_.begin(), impl_->selectedItemsCache_.end());

        return winrt::Microsoft::UI::Xaml::Controls::SelectionChangedEventArgs{std::move(removedSelectedItems), std::move(addedSelectedItems)};
    }

    // ReSharper disable once CppMemberFunctionMayBeConst
    void DataGridSelectedItemsCollection::InsertIndex(int32_t const slot)
    {
        impl_->selectedSlotsTable_.InsertIndex(slot);
        impl_->oldSelectedSlotsTable_.InsertIndex(slot);

        // It's possible that we're inserting an item that was just removed.  If that's the case,
        // and the re-inserted item used to be selected, we want to update the oldSelectedSlotsTable_
        // to include the item's new index within the collection.
        if (auto const rowIndex = impl_->owningGrid_->RowIndexFromSlot(slot);
            rowIndex != InvalidIndex)
        {
            if (auto const insertedItem = impl_->owningGrid_->DataConnection().GetDataItem(rowIndex);
                insertedItem && std::ranges::find(impl_->oldSelectedItemsCache_, insertedItem) != impl_->oldSelectedItemsCache_.end())
            {
                impl_->oldSelectedSlotsTable_.AddValue(slot, true);
            }
        }
    }

    // ReSharper disable once CppMemberFunctionMayBeConst
    void DataGridSelectedItemsCollection::SelectSlot(int32_t const slot, bool const select)
    {
        if (impl_->owningGrid_->RowGroupHeadersTable().Contains(slot))
        {
            return;
        }

        if (select)  // NOLINT(bugprone-branch-clone)
        {
            if (!impl_->selectedSlotsTable_.Contains(slot))
            {
                impl_->selectedItemsCache_.push_back(impl_->owningGrid_->DataConnection().GetDataItem(impl_->owningGrid_->RowIndexFromSlot(slot)));
            }

            impl_->selectedSlotsTable_.AddValue(slot, true);
        }
        else
        {
            if (impl_->selectedSlotsTable_.Contains(slot))
            {
                Utility::erase_value(impl_->selectedItemsCache_, impl_->owningGrid_->DataConnection().GetDataItem(impl_->owningGrid_->RowIndexFromSlot(slot)));
            }

            impl_->selectedSlotsTable_.RemoveValue(slot);
        }
    }

    // ReSharper disable once CppMemberFunctionMayBeConst
    void DataGridSelectedItemsCollection::SelectSlots(int32_t const startSlot, int32_t const endSlot, bool const select)
    {
        auto itemSlot = impl_->owningGrid_->RowGroupHeadersTable().GetNextGap(startSlot - 1);
        auto const endItemSlot = impl_->owningGrid_->RowGroupHeadersTable().GetPreviousGap(endSlot + 1);

        if (select)
        {
            while (itemSlot <= endItemSlot)
            {
                // Add the newly selected item slots by skipping over the RowGroupHeaderSlots
                auto const nextRowGroupHeaderSlot = impl_->owningGrid_->RowGroupHeadersTable().GetNextIndex(itemSlot);
                auto const lastItemSlot = nextRowGroupHeaderSlot == InvalidIndex ? endItemSlot : std::min<>(endItemSlot, nextRowGroupHeaderSlot - 1);

                for (auto slot = itemSlot; slot <= lastItemSlot; slot++)
                {
                    if (!impl_->selectedSlotsTable_.Contains(slot))
                    {
                        impl_->selectedItemsCache_.push_back(impl_->owningGrid_->DataConnection().GetDataItem(impl_->owningGrid_->RowIndexFromSlot(slot)));
                    }
                }

                impl_->selectedSlotsTable_.AddValues(itemSlot, lastItemSlot - itemSlot + 1, true);
                itemSlot = impl_->owningGrid_->RowGroupHeadersTable().GetNextGap(lastItemSlot);
            }
        }
        else
        {
            while (itemSlot <= endItemSlot)
            {
                // Remove the unselected item slots by skipping over the RowGroupHeaderSlots
                auto const nextRowGroupHeaderSlot = impl_->owningGrid_->RowGroupHeadersTable().GetNextIndex(itemSlot);
                auto const lastItemSlot = nextRowGroupHeaderSlot == InvalidIndex ? endItemSlot : std::min<>(endItemSlot, nextRowGroupHeaderSlot - 1);

                for (auto slot = itemSlot; slot <= lastItemSlot; slot++)
                {
                    if (impl_->selectedSlotsTable_.Contains(slot))
                    {
                        Utility::erase_value(impl_->selectedItemsCache_, impl_->owningGrid_->DataConnection().GetDataItem(impl_->owningGrid_->RowIndexFromSlot(slot)));
                    }
                }

                impl_->selectedSlotsTable_.RemoveValues(itemSlot, lastItemSlot - itemSlot + 1);
                itemSlot = impl_->owningGrid_->RowGroupHeadersTable().GetNextGap(lastItemSlot);
            }
        }
    }

    // ReSharper disable once CppMemberFunctionMayBeConst
    void DataGridSelectedItemsCollection::UpdateIndexes()
    {
        impl_->oldSelectedSlotsTable_.Clear();
        impl_->selectedSlotsTable_.Clear();

        if (!impl_->owningGrid_->DataConnection().DataSource())
        {
            if (impl_->selectedItemsCache_.size() > 0)
            {
                impl_->owningGrid_->SelectionHasChanged(true);
                impl_->selectedItemsCache_.clear();
            }
        }
        else
        {
            std::vector<winrt::Windows::Foundation::IInspectable> tempSelectedItemsCache;
            for(auto const& item : impl_->selectedItemsCache_)
            {
                if (auto const index = impl_->owningGrid_->DataConnection().IndexOf(item);
                    index != InvalidIndex)
                {
                    tempSelectedItemsCache.push_back(item);
                    impl_->selectedSlotsTable_.AddValue(impl_->owningGrid_->SlotFromRowIndex(index), true);
                }
            }

            for(auto const& item : impl_->oldSelectedItemsCache_)
            {
                if (auto const index = impl_->owningGrid_->DataConnection().IndexOf(item);
                    index == InvalidIndex)
                {
                    impl_->owningGrid_->SelectionHasChanged(true);
                }
                else
                {
                    impl_->oldSelectedSlotsTable_.AddValue(impl_->owningGrid_->SlotFromRowIndex(index), true);
                }
            }

            impl_->selectedItemsCache_ = tempSelectedItemsCache;
        }
    }
}
