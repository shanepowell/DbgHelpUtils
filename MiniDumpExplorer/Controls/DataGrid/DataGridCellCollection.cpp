#include "pch.h"
#include "DataGridCellCollection.h"

#include "DataGridCell.h"
#include "DataGridRow.h"

using namespace winrt::MiniDumpExplorer;
using namespace winrt;

namespace DataGridInternal
{
    struct DataGridCellCollection::Impl
    {
        Impl(implementation::DataGridRow* owningRow)
            : owningRow_{owningRow}
        {
        }

        std::vector<DataGridCell> cells_;
        implementation::DataGridRow* owningRow_;

        event<Windows::Foundation::EventHandler<DataGridCellEventArgs>> cellAddedHandler_;
        event<Windows::Foundation::EventHandler<DataGridCellEventArgs>> cellRemovedHandler_;
    };


    DataGridCellCollection::DataGridCellCollection(implementation::DataGridRow* owningRow)
        : impl_{std::make_unique<Impl>(owningRow)}
    {
    }

    DataGridCellCollection::~DataGridCellCollection() = default;

    // ReSharper disable once CppMemberFunctionMayBeConst
    event_token DataGridCellCollection::CellAdded(Windows::Foundation::EventHandler<DataGridCellEventArgs> const& value)
    {
        return impl_->cellAddedHandler_.add(value);
    }

    // ReSharper disable once CppMemberFunctionMayBeConst
    void DataGridCellCollection::CellAdded(event_token const& value)
    {
        impl_->cellAddedHandler_.remove(value);
    }

    // ReSharper disable once CppMemberFunctionMayBeConst
    event_token DataGridCellCollection::CellRemoved(Windows::Foundation::EventHandler<DataGridCellEventArgs> const& value)
    {
        return impl_->cellRemovedHandler_.add(value);
    }

    // ReSharper disable once CppMemberFunctionMayBeConst
    void DataGridCellCollection::CellRemoved(event_token const& value)
    {
        impl_->cellRemovedHandler_.remove(value);
    }

    size_t DataGridCellCollection::Count() const
    {
        return impl_->cells_.size();
    }

    std::vector<DataGridCell> const& DataGridCellCollection::GetCells() const
    {
        return impl_->cells_;
    }

    // ReSharper disable once CppMemberFunctionMayBeConst
    void DataGridCellCollection::Insert(size_t const cellIndex, DataGridCell const& cell)
    {
        cell.try_as<implementation::DataGridCell>()->OwningRow(*impl_->owningRow_);
        impl_->cells_.insert(impl_->cells_.begin() + cellIndex, cell);

        impl_->cellAddedHandler_(*impl_->owningRow_, DataGridCellEventArgs{cell});
    }

    // ReSharper disable once CppMemberFunctionMayBeConst
    void DataGridCellCollection::RemoveAt(size_t const cellIndex)
    {
        auto cell = impl_->cells_.at(cellIndex);
        impl_->cells_.erase(impl_->cells_.begin() + cellIndex);
        cell.try_as<implementation::DataGridCell>()->OwningRow(DataGridRow{nullptr});

        impl_->cellRemovedHandler_(*impl_->owningRow_, DataGridCellEventArgs{std::move(cell)});
    }

    DataGridCell DataGridCellCollection::operator[](size_t const index) const
    {
        return impl_->cells_.at(index);
    }
}
