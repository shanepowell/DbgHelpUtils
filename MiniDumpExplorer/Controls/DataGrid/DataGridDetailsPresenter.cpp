#include "pch.h"
#include "DataGridDetailsPresenter.h"

#include <winrt/Windows.UI.Xaml.Interop.h>

#include "DataGrid.h"
#include "DataGridRow.h"
#include "DataGridCell.h"
#include "DataGridColumnCollection.h"
#include "DataGridFillerColumn.h"
#include "DbgHelpUtils/double_utils.h"

#if __has_include("DataGridDetailsPresenter.g.cpp")
// ReSharper disable once CppUnusedIncludeDirective
#include "DataGridDetailsPresenter.g.cpp"  // NOLINT(bugprone-suspicious-include)
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    auto DataGridDetailsPresenter::InternalOwningGrid() const
    {
        return OwningGrid().as<DataGrid>();
    }

    auto DataGridDetailsPresenter::InternalOwningRow() const
    {
        return owningRow_.as<DataGridRow>();
    }

    DataGridDetailsPresenter::DataGridDetailsPresenter() = default;

    DependencyProperty DataGridDetailsPresenter::ContentHeightProperty()
    {
        static DependencyProperty s_ContentHeightProperty = DependencyProperty::Register(
            L"ContentHeight",
            xaml_typename<double>(),
            xaml_typename<MiniDumpExplorer::DataGridDetailsPresenter>(),
            PropertyMetadata{ box_value(0.0), [](DependencyObject const& d, DependencyPropertyChangedEventArgs const& e) { d.as<DataGridDetailsPresenter>()->OnContentHeightPropertyChanged(e); } });

        return s_ContentHeightProperty;
    }

    double DataGridDetailsPresenter::ContentHeight() const
    {
        return unbox_value<double>(GetValue(ContentHeightProperty()));
    }

    void DataGridDetailsPresenter::ContentHeight(double const value) const
    {
        SetValue(ContentHeightProperty(), box_value(value));
    }

    void DataGridDetailsPresenter::EnsureFillerVisibility() const
    {
        auto const internalOwningGrid = InternalOwningGrid();
        auto const internalOwningRow = InternalOwningRow();
        auto const fillerCell = internalOwningRow->FillerCell();
        auto const fillerColumn = internalOwningGrid->ColumnsInternal().FillerColumn().as<DataGridFillerColumn>();
        
        if (auto const newVisibility = fillerColumn->IsActive() ? Visibility::Visible : Visibility::Collapsed;
            fillerCell.Visibility() != newVisibility)
        {
            fillerCell.Visibility(newVisibility);
            if (newVisibility == Visibility::Visible)
            {
                fillerCell.Arrange(Windows::Foundation::Rect{static_cast<float>(fillerLeftEdge_), 0.0f, static_cast<float>(fillerColumn->FillerWidth()), static_cast<float>(ActualHeight())});
            }
        }

        // This must be done after the Filler visibility is determined.  This also must be done
        // regardless of whether the filler visibility actually changed values because
        // we could scroll in a cell that didn't have EnsureGridLine called yet
        if (auto const lastVisibleColumn = internalOwningGrid->ColumnsInternal().LastVisibleColumn();
            lastVisibleColumn)
        {
            auto const cell = internalOwningRow->Cells()[lastVisibleColumn.as<DataGridColumn>()->Index()].as<DataGridCell>();
            cell->EnsureGridLine(lastVisibleColumn);
        }
    }

    void DataGridDetailsPresenter::Recycle()
    {
        // Clear out the cached desired height, so it is not reused for other rows
        desiredHeight_ = 0.0;

        if (OwningGrid() && OwningRow())
        {
            auto const internalOwningRow = InternalOwningRow();
            for(auto const& column : InternalOwningGrid()->ColumnsInternal().ObservableItems())
            {
                auto const internalColumn = column.as<DataGridColumn>();
                internalOwningRow->Cells()[internalColumn->Index()].as<DataGridCell>()->Recycle();
            }
        }
    }

    Windows::Foundation::Size DataGridDetailsPresenter::ArrangeOverride(Windows::Foundation::Size const& finalSize)
    {
        if (!OwningGrid())
        {
            return base_type::ArrangeOverride(finalSize);
        }

        auto const internalOwningGrid = InternalOwningGrid();
        if (internalOwningGrid->AutoSizingColumns())
        {
            // When we initially load an auto-column, we have to wait for all the rows to be measured
            // before we know its final desired size.  We need to trigger a new round of measures now
            // that the final sizes have been calculated.
            internalOwningGrid->AutoSizingColumns(false);
            return base_type::ArrangeOverride(finalSize);
        }

        auto frozenLeftEdge = 0.0;
        auto scrollingLeftEdge = -internalOwningGrid->HorizontalOffset();
        auto const internalOwningRow = InternalOwningRow();

        double cellLeftEdge;
        for (auto const& column : internalOwningGrid->ColumnsInternal().GetVisibleColumns())
        {
            auto const internalColumn = column.as<DataGridColumn>();
            auto const cell = internalOwningRow->Cells()[internalColumn->Index()];

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

            if (cell.Visibility() == Visibility::Visible)
            {
                cell.Arrange(Windows::Foundation::Rect{static_cast<float>(cellLeftEdge), 0, static_cast<float>(internalColumn->LayoutRoundedWidth()), finalSize.Height});
                EnsureCellClip(cell, column.ActualWidth(), finalSize.Height, frozenLeftEdge, scrollingLeftEdge);
            }

            scrollingLeftEdge += column.ActualWidth();
            internalColumn->IsInitialDesiredWidthDetermined(true);
        }

        fillerLeftEdge_ = scrollingLeftEdge;

        // FillerColumn.Width == 0 when the filler column is not active
        auto const fillerWidth = internalOwningGrid->ColumnsInternal().FillerColumn().as<DataGridFillerColumn>()->FillerWidth();
        internalOwningRow->FillerCell().Arrange(Windows::Foundation::Rect{static_cast<float>(fillerLeftEdge_), 0.0f, static_cast<float>(fillerWidth), finalSize.Height});

        return finalSize;
    }

    Windows::Foundation::Size DataGridDetailsPresenter::MeasureOverride(Windows::Foundation::Size const& availableSize)
    {
        if (!OwningGrid())
        {
            return base_type::MeasureOverride(availableSize);
        }

        auto const internalOwningGrid = InternalOwningGrid();
        bool autoSizeHeight;
        double measureHeight;
        if (std::isnan(internalOwningGrid->RowHeight()))
        {
            // No explicit height values were set, so we can auto size
            autoSizeHeight = true;
            measureHeight = std::numeric_limits<double>::infinity();
        }
        else
        {
            desiredHeight_ = internalOwningGrid->RowHeight();
            measureHeight = desiredHeight_;
            autoSizeHeight = false;
        }

        auto frozenLeftEdge = 0.0;
        auto totalDisplayWidth = 0.0;
        auto scrollingLeftEdge = -internalOwningGrid->HorizontalOffset();
        internalOwningGrid->ColumnsInternal().EnsureVisibleEdgedColumnsWidth();
        auto const lastVisibleColumn = internalOwningGrid->ColumnsInternal().LastVisibleColumn();
        auto const internalOwningRow = InternalOwningRow();
        for(auto const& column : internalOwningGrid->ColumnsInternal().GetVisibleColumns())
        {
            auto const internalColumn = column.as<DataGridColumn>();
            auto cell = internalOwningRow->Cells()[internalColumn->Index()].as<DataGridCell>();

            // Measure the entire first row to make the horizontal scrollbar more accurate
            auto const shouldDisplayCell = ShouldDisplayCell(column, frozenLeftEdge, scrollingLeftEdge) || internalOwningRow->Index() == 0;
            EnsureCellDisplay(*cell, shouldDisplayCell);
            if (shouldDisplayCell)
            {
                auto const columnWidth = column.WidthLength();
                auto const autoGrowWidth = columnWidth.IsSizeToCells() || columnWidth.IsAuto();
                if (column != lastVisibleColumn)
                {
                    cell->EnsureGridLine(lastVisibleColumn);
                }

                // If we're not using star sizing or the current column can't be resized,
                // then just set the display width according to the column's desired width
                if (!internalOwningGrid->UsesStarSizing() || (!internalColumn->ActualCanUserResize() && !column.WidthLength().IsStar()))
                {
                    // In the edge-case where we're given infinite width, and we have star columns, the
                    // star columns grow to their predefined limit of 10,000 (or their MaxWidth)
                    auto const newDisplayWidth = column.WidthLength().IsStar() ?
                        std::min(internalColumn->ActualMaxWidth(), DataGrid::MaximumStarColumnWidth) :
                        std::max(internalColumn->ActualMinWidth(), std::min(internalColumn->ActualMaxWidth(), column.WidthLength().DesiredValue()));
                    internalColumn->SetWidthDisplayValue(newDisplayWidth);
                }

                // If we're auto-growing the column based on the cell content, we want to measure it at its maximum value
                if (autoGrowWidth)
                {
                    cell->Measure(Windows::Foundation::Size{static_cast<float>(internalColumn->ActualMaxWidth()), static_cast<float>(measureHeight)});
                    internalOwningGrid->AutoSizeColumn(column, cell->DesiredSize().Width);
                    internalColumn->ComputeLayoutRoundedWidth(totalDisplayWidth);
                }
                else if (!internalOwningGrid->UsesStarSizing())
                {
                    internalColumn->ComputeLayoutRoundedWidth(scrollingLeftEdge);
                    cell->Measure(Windows::Foundation::Size{static_cast<float>(internalColumn->LayoutRoundedWidth()), static_cast<float>(measureHeight)});
                }

                // We need to track the largest height in order to auto-size
                if (autoSizeHeight)
                {
                    desiredHeight_ = std::max<double>(desiredHeight_, cell->DesiredSize().Height);
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
                auto cell = internalOwningRow->Cells()[internalColumn->Index()];
                internalColumn->ComputeLayoutRoundedWidth(leftEdge);
                cell.Measure(Windows::Foundation::Size{static_cast<float>(internalColumn->LayoutRoundedWidth()), static_cast<float>(measureHeight)});
                if (autoSizeHeight)
                {
                    desiredHeight_ = std::max<double>(desiredHeight_, cell.DesiredSize().Height);
                }

                leftEdge += column.ActualWidth();
            }
        }

        // Measure FillerCell, we're doing it unconditionally here because we don't know if we'll need the filler
        // column, and we don't want to cause another Measure if we do
        internalOwningRow->FillerCell().Measure(Windows::Foundation::Size{std::numeric_limits<float>::infinity(), static_cast<float>(desiredHeight_)});

        internalOwningGrid->ColumnsInternal().EnsureVisibleEdgedColumnsWidth();
        return Windows::Foundation::Size{static_cast<float>(internalOwningGrid->ColumnsInternal().VisibleEdgedColumnsWidth()), static_cast<float>(desiredHeight_)};
    }

    void DataGridDetailsPresenter::OnContentHeightPropertyChanged([[maybe_unused]] DependencyPropertyChangedEventArgs const& e) const
    {
        InvalidateMeasure();
    }

    MiniDumpExplorer::DataGrid DataGridDetailsPresenter::OwningGrid() const
    {
        if (owningRow_)
        {
            return owningRow_.try_as<DataGridRow>()->OwningGrid();
        }

        return nullptr;
    }

    bool DataGridDetailsPresenter::ShouldDisplayCell(MiniDumpExplorer::DataGridColumn const& column, double const frozenLeftEdge, double scrollingLeftEdge) const
    {
        assert(OwningGrid());

        auto const internalOwningGrid = InternalOwningGrid();
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

    void DataGridDetailsPresenter::EnsureCellClip(MiniDumpExplorer::DataGridCell const& cell, double const width, double const height, double const frozenLeftEdge, double const cellLeftEdge)
    {
        // Clip the cell only if it's scrolled under frozen columns.  Unfortunately, we need to clip in this case
        // because cells could be transparent
        if (auto const internalCell = cell.as<DataGridCell>();
            !internalCell->OwningColumn().IsFrozen() && frozenLeftEdge > cellLeftEdge)
        {
            Media::RectangleGeometry const rg{};
            auto const xClip = std::round(std::min(width, frozenLeftEdge - cellLeftEdge));
            rg.Rect(Windows::Foundation::Rect{static_cast<float>(xClip), 0.0f, static_cast<float>(std::max(0.0, width - xClip)), static_cast<float>(height)});
            cell.Clip(rg);
        }
        else
        {
            cell.Clip(nullptr);
        }
    }

    void DataGridDetailsPresenter::EnsureCellDisplay(MiniDumpExplorer::DataGridCell const& cell, bool const displayColumn)
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
                rg.Rect(Windows::Foundation::Rect{0, 0, 0, 0});
                cell.Clip(rg);
            }
        }
        else
        {
            cell.Visibility(displayColumn ? Visibility::Visible : Visibility::Collapsed);
        }
    }
}
