#include "pch.h"
#include "DataGridCellsPresenter.h"

#include <winrt/Microsoft.UI.Xaml.h>

#include "DataGrid.h"
#include "DataGridCell.h"
#include "DataGridColumn.h"
#include "DataGridColumnCollection.h"
#include "DataGridFillerColumn.h"
#include "DataGridRow.h"
#include "DbgHelpUtils/double_utils.h"

#if __has_include("DataGridCellsPresenter.g.cpp")
// ReSharper disable once CppUnusedIncludeDirective
#include "DataGridCellsPresenter.g.cpp"  // NOLINT(bugprone-suspicious-include)
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{

    auto DataGridCellsPresenter::InternalOwningRow() const
    {
        return owningRow_.try_as<DataGridRow>();
    }

    auto DataGridCellsPresenter::InternalOwningGrid() const
    {
        return InternalOwningRow()->OwningGrid().try_as<DataGrid>();
    }

    DataGridCellsPresenter::DataGridCellsPresenter() = default;

    Windows::Foundation::Size DataGridCellsPresenter::ArrangeOverride(Windows::Foundation::Size const& finalSize)
    {
        auto const internalOwningGrid = InternalOwningGrid();
        if (!internalOwningGrid)
        {
            return base_type::ArrangeOverride(finalSize);
        }

        if (internalOwningGrid->AutoSizingColumns())
        {
            // When we initially load an auto-column, we have to wait for all the rows to be measured
            // before we know its final desired size.  We need to trigger a new round of measures now
            // that the final sizes have been calculated.
            internalOwningGrid->AutoSizingColumns(false);
            return base_type::ArrangeOverride(finalSize);
        }

        double frozenLeftEdge = 0;
        auto scrollingLeftEdge = -internalOwningGrid->HorizontalOffset();

        auto const internalOwningRow = InternalOwningRow();

        double cellLeftEdge;
        for (auto const& column : internalOwningGrid->ColumnsInternal().GetVisibleColumns())
        {
            auto const internalColumn = column.as<DataGridColumn>();
            auto const cell = internalOwningRow->Cells()[internalColumn->Index()];
            auto const internalCell = cell.as<DataGridCell>();
            assert(internalCell->OwningColumn() == column);
            assert(internalColumn->IsVisible());

            if (column.IsFrozen())
            {
                cellLeftEdge = frozenLeftEdge;

                // This can happen before or after clipping because frozen cells aren't clipped
                frozenLeftEdge += column.ActualWidth();
            }
            else
            {
                cellLeftEdge = scrollingLeftEdge;
            }

            if (internalCell->Visibility() == Visibility::Visible)
            {
                internalCell->Arrange({static_cast<float>(cellLeftEdge), 0, static_cast<float>(internalColumn->LayoutRoundedWidth()), finalSize.Height});
                EnsureCellClip(cell, column.ActualWidth(), finalSize.Height, frozenLeftEdge, scrollingLeftEdge);
            }

            scrollingLeftEdge += column.ActualWidth();
            internalColumn->IsInitialDesiredWidthDetermined(true);
        }

        fillerLeftEdge_ = scrollingLeftEdge;

        // FillerColumn.Width == 0 when the filler column is not active
        internalOwningRow->FillerCell().Arrange({static_cast<float>(fillerLeftEdge_), 0, static_cast<float>(internalOwningGrid->ColumnsInternal().FillerColumn().as<DataGridFillerColumn>()->FillerWidth()), finalSize.Height});

        return finalSize;
    }

    Windows::Foundation::Size DataGridCellsPresenter::MeasureOverride(Windows::Foundation::Size const& availableSize)
    {
        auto const internalOwningGrid = InternalOwningGrid();
        if (!internalOwningGrid)
        {
            return base_type::MeasureOverride(availableSize);
        }

        bool autoSizeHeight;
        double measureHeight;
        if (std::isnan(internalOwningGrid->RowHeight()))
        {
            // No explicit height values were set so we can autosize
            autoSizeHeight = true;
            measureHeight = std::numeric_limits<double>::infinity();
        }
        else
        {
            desiredHeight_ = internalOwningGrid->RowHeight();
            measureHeight = desiredHeight_;
            autoSizeHeight = false;
        }

        double frozenLeftEdge = 0;
        double totalDisplayWidth = 0;
        auto scrollingLeftEdge = -internalOwningGrid->HorizontalOffset();
        internalOwningGrid->ColumnsInternal().EnsureVisibleEdgedColumnsWidth();
        auto const lastVisibleColumn = internalOwningGrid->ColumnsInternal().LastVisibleColumn();
        auto const owningRow = InternalOwningRow();
        for (auto const& column : internalOwningGrid->ColumnsInternal().GetVisibleColumns())
        {
            auto const internalColumn = column.as<DataGridColumn>();
            auto const cell = owningRow->Cells()[internalColumn->Index()];
            auto const internalCell = cell.as<DataGridCell>();

            // Measure the entire first row to make the horizontal scrollbar more accurate
            auto const shouldDisplayCell = ShouldDisplayCell(column, frozenLeftEdge, scrollingLeftEdge) || owningRow->Index() == 0;
            EnsureCellDisplay(cell, shouldDisplayCell);
            if (shouldDisplayCell)
            {
                auto columnWidth = column.WidthLength();
                auto const autoGrowWidth = columnWidth.IsSizeToCells() || columnWidth.IsAuto();
                if (column != lastVisibleColumn)
                {
                    internalCell->EnsureGridLine(lastVisibleColumn);
                }

                // If we're not using star sizing or the current column can't be resized,
                // then just set the display width according to the column's desired width
                if (!internalOwningGrid->UsesStarSizing() || (!internalColumn->ActualCanUserResize() && !column.WidthLength().IsStar()))
                {
                    // In the edge-case where we're given infinite width, and we have star columns, the
                    // star columns grow to their predefined limit of 10,000 (or their MaxWidth)
                    double const newDisplayWidth = column.WidthLength().IsStar() ?
                        std::min(internalColumn->ActualMaxWidth(), DataGrid::MaximumStarColumnWidth) :
                        std::max(internalColumn->ActualMinWidth(), std::min(internalColumn->ActualMaxWidth(), column.WidthLength().DesiredValue()));
                    internalColumn->SetWidthDisplayValue(newDisplayWidth);
                }

                // If we're auto-growing the column based on the cell content, we want to measure it at its maximum value
                if (autoGrowWidth)
                {
                    cell.Measure({static_cast<float>(internalColumn->ActualMaxWidth()), static_cast<float>(measureHeight)});
                    internalOwningGrid->AutoSizeColumn(column, cell.DesiredSize().Width);
                    internalColumn->ComputeLayoutRoundedWidth(totalDisplayWidth);
                }
                else if (!internalOwningGrid->UsesStarSizing())
                {
                    internalColumn->ComputeLayoutRoundedWidth(scrollingLeftEdge);
                    cell.Measure({static_cast<float>(internalColumn->LayoutRoundedWidth()), static_cast<float>(measureHeight)});
                }

                // We need to track the largest height in order to auto-size
                if (autoSizeHeight)
                {
                    desiredHeight_ = std::max(desiredHeight_, static_cast<double>(cell.DesiredSize().Height));
                }
            }

            if (column.IsFrozen())
            {
                frozenLeftEdge += column.ActualWidth();
            }

            scrollingLeftEdge += column.ActualWidth();
            totalDisplayWidth += column.ActualWidth();
        }

        // If we're using star sizing (and we're not waiting for an auto-column to finish growing)
        // then we will resize all the columns to fit the available space.
        if (internalOwningGrid->UsesStarSizing() && !internalOwningGrid->AutoSizingColumns())
        {
            auto const adjustment = internalOwningGrid->CellsWidth() - totalDisplayWidth;
            internalOwningGrid->AdjustColumnWidths(0, adjustment, false);

            // Since we didn't know the final widths of the columns until we resized,
            // we waited until now to measure each cell
            double leftEdge = 0;
            for(auto const& column : internalOwningGrid->ColumnsInternal().GetVisibleColumns())
            {
                auto const internalColumn = column.as<DataGridColumn>();
                auto cell = owningRow->Cells()[internalColumn->Index()];
                internalColumn->ComputeLayoutRoundedWidth(leftEdge);
                cell.Measure({static_cast<float>(internalColumn->LayoutRoundedWidth()), static_cast<float>(measureHeight)});
                if (autoSizeHeight)
                {
                    desiredHeight_ = std::max(desiredHeight_, static_cast<double>(cell.DesiredSize().Height));
                }

                leftEdge += column.ActualWidth();
            }
        }

        // Measure FillerCell, we're doing it unconditionally here because we don't know if we'll need the filler
        // column, and we don't want to cause another Measure if we do
        owningRow->FillerCell().Measure({std::numeric_limits<float>::infinity(), static_cast<float>(desiredHeight_)});

        internalOwningGrid->ColumnsInternal().EnsureVisibleEdgedColumnsWidth();
        return {static_cast<float>(internalOwningGrid->ColumnsInternal().VisibleEdgedColumnsWidth()), static_cast<float>(desiredHeight_)};
    }

    void DataGridCellsPresenter::EnsureFillerVisibility() const
    {
        auto const internalOwningGrid = InternalOwningGrid();
        auto const internalOwningRow = InternalOwningRow();
        auto const fillerColumn = internalOwningGrid->ColumnsInternal().FillerColumn().as<DataGridFillerColumn>();
        auto const newVisibility = fillerColumn->IsActive() ? Visibility::Visible : Visibility::Collapsed;
        
        if (auto const fillerCell = internalOwningRow->FillerCell();
            fillerCell.Visibility() != newVisibility)
        {
            fillerCell.Visibility(newVisibility);
            if (newVisibility == Visibility::Visible)
            {
                fillerCell.Arrange({static_cast<float>(fillerLeftEdge_), 0, static_cast<float>(fillerColumn->FillerWidth()), static_cast<float>(ActualHeight())});
            }
        }

        // This must be done after the Filler visibility is determined.  This also must be done
        // regardless of whether the filler visibility actually changed values because
        // we could scroll in a cell that didn't have EnsureGridLine called yet
        if (auto const lastVisibleColumn = internalOwningGrid->ColumnsInternal().LastVisibleColumn();
            lastVisibleColumn)
        {
            auto const internalLastVisibleColumn = lastVisibleColumn.as<DataGridColumn>();
            auto const cell = internalOwningRow->Cells()[internalLastVisibleColumn->Index()].as<DataGridCell>();
            cell->EnsureGridLine(lastVisibleColumn);
        }
    }

    void DataGridCellsPresenter::Recycle()
    {
        // Clear out the cached desired height, so it is not reused for other rows
        desiredHeight_ = 0;

        auto const internalOwningGrid = InternalOwningGrid();

        if (auto const internalOwningRow = InternalOwningRow();
            internalOwningGrid && internalOwningRow)
        {
            for(auto const& column : internalOwningGrid->ColumnsInternal().ObservableItems())
            {
                auto const internalColumn = column.as<DataGridColumn>();
                internalOwningRow->Cells()[internalColumn->Index()].as<DataGridCell>()->Recycle();
            }
        }
    }

    MiniDumpExplorer::DataGrid DataGridCellsPresenter::OwningGrid() const
    {
        if(auto const internalOwningRow = InternalOwningRow();
            internalOwningRow)
        {
            return internalOwningRow->OwningGrid();
        }

        return nullptr;
    }

    bool DataGridCellsPresenter::ShouldDisplayCell(MiniDumpExplorer::DataGridColumn const& column, double const frozenLeftEdge, double scrollingLeftEdge) const
    {
        auto const internalOwningGrid = InternalOwningGrid();
        assert(internalOwningGrid);
        assert(internalOwningGrid->HorizontalAdjustment() >= 0);
        assert(internalOwningGrid->HorizontalAdjustment() <= internalOwningGrid->HorizontalOffset());

        if (column.Visibility() != Visibility::Visible)
        {
            return false;
        }

        scrollingLeftEdge += internalOwningGrid->HorizontalAdjustment();
        auto const leftEdge = column.IsFrozen() ? frozenLeftEdge : scrollingLeftEdge;
        auto const rightEdge = leftEdge + column.ActualWidth();
        return double_utils::greater_than(rightEdge, 0) &&
            double_utils::less_than_or_close(leftEdge, internalOwningGrid->CellsWidth()) &&
            double_utils::greater_than(rightEdge, frozenLeftEdge); // scrolling column covered up by frozen column(s)
    }

    void DataGridCellsPresenter::EnsureCellClip(MiniDumpExplorer::DataGridCell const& cell, double const width, double const height, double const frozenLeftEdge, double const cellLeftEdge)
    {
        // Clip the cell only if it's scrolled under frozen columns.  Unfortunately, we need to clip in this case
        // because cells could be transparent
        if (!cell.as<DataGridCell>()->OwningColumn().as<DataGridColumn>()->IsFrozen() && frozenLeftEdge > cellLeftEdge)
        {
            Media::RectangleGeometry const rg{};
            auto const xClip = std::round(std::min(width, frozenLeftEdge - cellLeftEdge));
            rg.Rect({static_cast<float>(xClip), 0, static_cast<float>(std::max<double>(0, width - xClip)), static_cast<float>(height)});
            cell.Clip(rg);
        }
        else
        {
            cell.Clip(nullptr);
        }
    }

    void DataGridCellsPresenter::EnsureCellDisplay(MiniDumpExplorer::DataGridCell const& cell, bool const displayColumn)
    {
        if (cell.as<DataGridCell>()->IsCurrent())
        {
            if (displayColumn)
            {
                cell.Visibility(Visibility::Visible);
                cell.Clip(nullptr);
            }
            else
            {
                // Clip
                Media::RectangleGeometry const rg{};
                rg.Rect({});
                cell.Clip(rg);
            }
        }
        else
        {
            cell.Visibility(displayColumn ? Visibility::Visible : Visibility::Collapsed);
        }
    }
}
