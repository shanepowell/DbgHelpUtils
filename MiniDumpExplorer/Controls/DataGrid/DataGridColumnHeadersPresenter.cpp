#include "pch.h"
#include "DataGridColumnHeadersPresenter.h"

#include "DataGrid.h"
#include "DataGridColumn.h"
#include "DataGridColumnCollection.h"
#include "DataGridColumnHeader.h"
#include "DataGridFillerColumn.h"

#if __has_include("DataGridColumnHeadersPresenter.g.cpp")
// ReSharper disable once CppUnusedIncludeDirective
#include "DataGridColumnHeadersPresenter.g.cpp"  // NOLINT(bugprone-suspicious-include)
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    struct DataGridColumnHeadersPresenterAutomationPeer;

    auto DataGridColumnHeadersPresenter::InternalOwningGrid()
    {
        return OwningGrid().try_as<DataGrid>();
    }

    DataGridColumnHeadersPresenter::DataGridColumnHeadersPresenter() = default;

    void DataGridColumnHeadersPresenter::DragIndicator(Controls::Control const& value)
    {
        if (value != dragIndicator_)
        {
            if (uint32_t index;
                Children().IndexOf(dragIndicator_, index))
            {
                Children().RemoveAt(index);
            }

            dragIndicator_ = value;
            if (dragIndicator_)
            {
                Children().Append(dragIndicator_);
            }
        }
    }

    void DataGridColumnHeadersPresenter::DropLocationIndicator(Controls::Control const& value)
    {
        if (value != dropLocationIndicator_)
        {
            if (uint32_t index;
                Children().IndexOf(dropLocationIndicator_, index))
            {
                Children().RemoveAt(index);
            }

            dropLocationIndicator_ = value;
            if (dropLocationIndicator_)
            {
                Children().Append(dropLocationIndicator_);
            }
        }
    }

    Windows::Foundation::Size DataGridColumnHeadersPresenter::ArrangeOverride(Windows::Foundation::Size const& finalSize)
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

        double dragIndicatorLeftEdge = 0;
        double frozenLeftEdge = 0;
        auto scrollingLeftEdge = -internalOwningGrid->HorizontalOffset();
        for(auto const& dataGridColumn : internalOwningGrid->ColumnsInternal().GetVisibleColumns())
        {
            auto const& internalDataGridColumn = dataGridColumn.try_as<DataGridColumn>();
            auto const columnHeader = internalDataGridColumn->HeaderCell();
            auto const internalColumnHeader = columnHeader.as<DataGridColumnHeader>();
            assert(internalColumnHeader->OwningColumn() == dataGridColumn);

            if (dataGridColumn.IsFrozen())
            {
                columnHeader.Arrange({static_cast<float>(frozenLeftEdge), 0, static_cast<float>(internalDataGridColumn->LayoutRoundedWidth()), finalSize.Height});
                columnHeader.Clip(nullptr); // The layout system could have clipped this because it's not aware of our render transform
                if (DragColumn() == dataGridColumn && DragIndicator())
                {
                    dragIndicatorLeftEdge = frozenLeftEdge + DragIndicatorOffset();
                }

                frozenLeftEdge += dataGridColumn.ActualWidth();
            }
            else
            {
                columnHeader.Arrange({static_cast<float>(scrollingLeftEdge), 0, static_cast<float>(internalDataGridColumn->LayoutRoundedWidth()), finalSize.Height});
                EnsureColumnHeaderClip(columnHeader, dataGridColumn.ActualWidth(), finalSize.Height, frozenLeftEdge, scrollingLeftEdge);
                if (DragColumn() == dataGridColumn && DragIndicator())
                {
                    dragIndicatorLeftEdge = scrollingLeftEdge + DragIndicatorOffset();
                }
            }

            scrollingLeftEdge += dataGridColumn.ActualWidth();
        }

        if (DragColumn())
        {
            if (DragIndicator())
            {
                EnsureColumnReorderingClip(DragIndicator(), finalSize.Height, frozenLeftEdge, dragIndicatorLeftEdge);  // NOLINT(readability-suspicious-call-argument)
                DragIndicator().Arrange({static_cast<float>(dragIndicatorLeftEdge), 0, static_cast<float>(DragIndicator().ActualWidth()), static_cast<float>(DragIndicator().ActualHeight())});
            }

            if (DropLocationIndicator())
            {
                EnsureColumnReorderingClip(DropLocationIndicator(), finalSize.Height, frozenLeftEdge, DropLocationIndicatorOffset());
                DropLocationIndicator().Arrange({static_cast<float>(DropLocationIndicatorOffset()), 0, static_cast<float>(DropLocationIndicator().ActualWidth()), static_cast<float>(DropLocationIndicator().ActualHeight())});
            }
        }

        // Arrange filler
        internalOwningGrid->OnFillerColumnWidthNeeded(finalSize.Width);
        auto const fillerColumn = internalOwningGrid->ColumnsInternal().FillerColumn();
        if (auto const internalFillerColumn = fillerColumn.as<DataGridFillerColumn>();
            internalFillerColumn->FillerWidth() > 0)
        {
            auto const headerCell = internalFillerColumn->HeaderCell();
            headerCell.Visibility(Visibility::Visible);
            headerCell.Arrange({static_cast<float>(scrollingLeftEdge), 0, static_cast<float>(internalFillerColumn->FillerWidth()), finalSize.Height});
        }
        else
        {
            internalFillerColumn->HeaderCell().Visibility(Visibility::Collapsed);
        }

        // This needs to be updated after the filler column is configured
        if (auto const lastVisibleColumn = internalOwningGrid->ColumnsInternal().LastVisibleColumn();
            lastVisibleColumn)
        {
            lastVisibleColumn.as<DataGridColumn>()->HeaderCell().as<DataGridColumnHeader>()->UpdateSeparatorVisibility(lastVisibleColumn);
        }

        return finalSize;
    }

    Windows::Foundation::Size DataGridColumnHeadersPresenter::MeasureOverride(Windows::Foundation::Size const& availableSize)
    {
        auto const internalOwningGrid = InternalOwningGrid();
        if (!internalOwningGrid)
        {
            return base_type::MeasureOverride(availableSize);
        }

        if (!internalOwningGrid->AreColumnHeadersVisible())
        {
            return {0.0, 0.0};
        }

        double height = internalOwningGrid->ColumnHeaderHeight();
        bool autoSizeHeight;
        if (std::isnan(height))
        {
            // No explicit height values were set so we can autosize
            height = 0;
            autoSizeHeight = true;
        }
        else
        {
            autoSizeHeight = false;
        }

        double totalDisplayWidth = 0;
        internalOwningGrid->ColumnsInternal().EnsureVisibleEdgedColumnsWidth();
        auto const lastVisibleColumn = internalOwningGrid->ColumnsInternal().LastVisibleColumn();
        for(auto const& column : internalOwningGrid->ColumnsInternal().GetVisibleColumns())
        {
            // Measure each column header
            auto const internalColumn = column.as<DataGridColumn>();
            auto const autoGrowWidth = column.Width().IsAuto() || column.Width().IsSizeToHeader();
            auto columnHeader = internalColumn->HeaderCell();
            if (column != lastVisibleColumn)
            {
                columnHeader.as<DataGridColumnHeader>()->UpdateSeparatorVisibility(lastVisibleColumn);
            }

            // If we're not using star sizing or the current column can't be resized,
            // then just set the display width according to the column's desired width
            if (!internalOwningGrid->UsesStarSizing() || (!internalColumn->ActualCanUserResize() && !column.Width().IsStar()))
            {
                // In the edge-case where we're given infinite width, and we have star columns, the
                // star columns grow to their predefined limit of 10,000 (or their MaxWidth)
                double const newDisplayWidth = column.Width().IsStar() ?
                    std::min(internalColumn->ActualMaxWidth(), DataGrid::MaximumStarColumnWidth) :
                    std::max(internalColumn->ActualMinWidth(), std::min(internalColumn->ActualMaxWidth(), column.Width().DesiredValue()));
                internalColumn->SetWidthDisplayValue(newDisplayWidth);
            }

            // If we're auto-growing the column based on the header content, we want to measure it at its maximum value
            if (autoGrowWidth)
            {
                columnHeader.Measure({static_cast<float>(internalColumn->ActualMaxWidth()), std::numeric_limits<float>::infinity()});
                internalOwningGrid->AutoSizeColumn(column, columnHeader.DesiredSize().Width);
                internalColumn->ComputeLayoutRoundedWidth(totalDisplayWidth);
            }
            else if (!internalOwningGrid->UsesStarSizing())
            {
                internalColumn->ComputeLayoutRoundedWidth(totalDisplayWidth);
                columnHeader.Measure({static_cast<float>(internalColumn->LayoutRoundedWidth()), std::numeric_limits<float>::infinity()});
            }

            // We need to track the largest height in order to auto-size
            if (autoSizeHeight)
            {
                height = std::max(height, static_cast<double>(columnHeader.DesiredSize().Height));
            }

            totalDisplayWidth += column.ActualWidth();
        }

        // If we're using star sizing (and we're not waiting for an auto-column to finish growing)
        // then we will resize all the columns to fit the available space.
        if (internalOwningGrid->UsesStarSizing() && !internalOwningGrid->AutoSizingColumns())
        {
            auto const adjustment = std::isinf(availableSize.Width) ? internalOwningGrid->CellsWidth() : availableSize.Width - totalDisplayWidth;
            internalOwningGrid->AdjustColumnWidths(0, adjustment, false);

            // Since we didn't know the final widths of the columns until we resized,
            // we waited until now to measure each header
            double leftEdge = 0;
            for(auto const& column : internalOwningGrid->ColumnsInternal().GetVisibleColumns())
            {
                auto const internalColumn = column.as<DataGridColumn>();
                internalColumn->ComputeLayoutRoundedWidth(leftEdge);
                internalColumn->HeaderCell().Measure({static_cast<float>(internalColumn->LayoutRoundedWidth()), std::numeric_limits<float>::infinity()});
                if (autoSizeHeight)
                {
                    height = std::max(height, static_cast<double>(internalColumn->HeaderCell().DesiredSize().Height));
                }

                leftEdge += column.ActualWidth();
            }
        }

        // Add the filler column if it's not represented.  We won't know whether we need it or not until Arrange
        auto const fillerColumn = internalOwningGrid->ColumnsInternal().FillerColumn().as<DataGridFillerColumn>();
        if (!fillerColumn->IsRepresented())
        {
            [[maybe_unused]] auto const currentSize = internalOwningGrid->ColumnsInternal().ObservableItems().Size();
            [[maybe_unused]] uint32_t index;
            assert(!Children().IndexOf(fillerColumn->HeaderCell(), index));
            fillerColumn->HeaderCell().SeparatorVisibility(Visibility::Collapsed);

            auto const size = Children().Size();
            Children().InsertAt(std::min(internalOwningGrid->ColumnsInternal().ObservableItems().Size(), size), fillerColumn->HeaderCell());
            fillerColumn->IsRepresented(true);

            // Optimize for the case where we don't need the filler cell
            fillerColumn->HeaderCell().Visibility(Visibility::Collapsed);
        }

        fillerColumn->HeaderCell().Measure({std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity()});

        if (DragIndicator())
        {
            DragIndicator().Measure({std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity()});
        }

        if (DropLocationIndicator())
        {
            DropLocationIndicator().Measure({std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity()});
        }

        internalOwningGrid->ColumnsInternal().EnsureVisibleEdgedColumnsWidth();
        return {static_cast<float>(internalOwningGrid->ColumnsInternal().VisibleEdgedColumnsWidth()), static_cast<float>(height)};
    }

    Automation::Peers::AutomationPeer DataGridColumnHeadersPresenter::OnCreateAutomationPeer() const
    {
        return MiniDumpExplorer::DataGridColumnHeadersPresenterAutomationPeer{*this};
    }

    void DataGridColumnHeadersPresenter::EnsureColumnHeaderClip(MiniDumpExplorer::DataGridColumnHeader const& columnHeader, double const width, double const height, double const frozenLeftEdge, double const columnHeaderLeftEdge)
    {
        // Clip the cell only if it's scrolled under frozen columns.  Unfortunately, we need to clip in this case
        // because cells could be transparent
        if (frozenLeftEdge > columnHeaderLeftEdge)
        {
            Media::RectangleGeometry const rg{};
            auto const xClip = std::min(width, frozenLeftEdge - columnHeaderLeftEdge);
            rg.Rect({static_cast<float>(xClip), 0, static_cast<float>(width - xClip), static_cast<float>(height)});
            columnHeader.Clip(rg);
        }
        else
        {
            columnHeader.Clip(nullptr);
        }
    }

    void DataGridColumnHeadersPresenter::EnsureColumnReorderingClip(Controls::Control const& control, double const height, double const frozenColumnsWidth, double const controlLeftEdge)
    {
        auto const internalOwningGrid = InternalOwningGrid();
        double leftEdge = 0;
        auto rightEdge = internalOwningGrid->CellsWidth();
        auto const width = control.ActualWidth();
        if (DragColumn().IsFrozen())
        {
            // If we're dragging a frozen column, we want to clip the corresponding DragIndicator control when it goes
            // into the scrolling columns region, but not the DropLocationIndicator.
            if (control == DragIndicator())
            {
                rightEdge = std::min(rightEdge, frozenColumnsWidth);
            }
        }
        else if (internalOwningGrid->FrozenColumnCount() > 0)
        {
            // If we're dragging a scrolling column, we want to clip both the DragIndicator and the DropLocationIndicator
            // controls when they go into the frozen column range.
            leftEdge = frozenColumnsWidth;
        }

        Media::RectangleGeometry rg{nullptr};
        if (leftEdge > controlLeftEdge)
        {
            rg = Media::RectangleGeometry{};
            auto const xClip = std::min(width, leftEdge - controlLeftEdge);
            rg.Rect({static_cast<float>(xClip), 0, static_cast<float>(width - xClip), static_cast<float>(height)});
        }

        if (controlLeftEdge + width >= rightEdge)
        {
            if (!rg)
            {
                rg = Media::RectangleGeometry{};
            }

            rg.Rect({rg.Rect().X, rg.Rect().Y, static_cast<float>(std::max(0.0, rightEdge - controlLeftEdge - rg.Rect().X)), static_cast<float>(height)});
        }

        control.Clip(rg);
    }
}
