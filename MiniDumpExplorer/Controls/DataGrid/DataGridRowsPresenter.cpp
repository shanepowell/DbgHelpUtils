#include "pch.h"
#include "DataGridRowsPresenter.h"

#include <winrt/Microsoft.UI.XAML.Input.h>

#include "DataGrid.h"
#include "DataGridColumnCollection.h"
#include "DataGridDisplayData.h"
#include "DataGridFillerColumn.h"
#include "DataGridRow.h"
#include "DataGridRowGroupHeader.h"
// ReSharper disable once CppUnusedIncludeDirective
#include "DataGridRowsPresenterAutomationPeer.h"

#include "Utility/logger.h"

#if __has_include("DataGridRowsPresenter.g.cpp")
// ReSharper disable once CppUnusedIncludeDirective
#include "DataGridRowsPresenter.g.cpp"  // NOLINT(bugprone-suspicious-include)
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    auto DataGridRowsPresenter::InternalOwningGrid() const
    {
        return owningGrid_.try_as<DataGrid>();
    }


    DataGridRowsPresenter::DataGridRowsPresenter()
    {
        [[maybe_unused]] auto eventManipulationStarting = ManipulationStarting({this, &DataGridRowsPresenter::DataGridRowsPresenter_ManipulationStarting});
        [[maybe_unused]] auto eventManipulationStarted = ManipulationStarted({&DataGridRowsPresenter::DataGridRowsPresenter_ManipulationStarted});
        [[maybe_unused]] auto eventManipulationDelta = ManipulationDelta({this, &DataGridRowsPresenter::DataGridRowsPresenter_ManipulationDelta});
    }

    Windows::Foundation::Size DataGridRowsPresenter::ArrangeOverride(Windows::Foundation::Size const& finalSize) const
    {
        auto const internalOwningGrid = InternalOwningGrid();
        if (finalSize.Height == 0 || !internalOwningGrid)
        {
            return base_type::ArrangeOverride(finalSize);
        }

        internalOwningGrid->OnFillerColumnWidthNeeded(finalSize.Width);

        auto const rowDesiredWidth = internalOwningGrid->ColumnsInternal().VisibleEdgedColumnsWidth() + internalOwningGrid->ColumnsInternal().FillerColumn().as<DataGridFillerColumn>()->FillerWidth();
        auto topEdge = -internalOwningGrid->NegVerticalOffset();
        for(auto const& element : internalOwningGrid->DisplayData().GetScrollingElements())
        {
            if (auto const row = element.try_as<DataGridRow>();
                row)
            {
                assert(row->Index() != DataGridRow::InvalidIndex); // A displayed row should always have its index

                // Visibility for all filler cells needs to be set in one place.  Setting it individually in
                // each CellsPresenter causes an NxN layout cycle (see DevDiv Bugs 211557)
                row->EnsureFillerVisibility();
                row->Arrange({static_cast<float>(-internalOwningGrid->HorizontalOffset()), static_cast<float>(topEdge), static_cast<float>(rowDesiredWidth), element.DesiredSize().Height});
            }
            else if (auto const groupHeader = element.try_as<DataGridRowGroupHeader>();
                    groupHeader)
            {
                auto const leftEdge = internalOwningGrid->AreRowGroupHeadersFrozen() ? 0.0 : -internalOwningGrid->HorizontalOffset();
                groupHeader->Arrange({static_cast<float>(leftEdge), static_cast<float>(topEdge), static_cast<float>(rowDesiredWidth - leftEdge), element.DesiredSize().Height});
            }

            topEdge += element.DesiredSize().Height;
        }

        auto const finalHeight = std::max<double>(topEdge + internalOwningGrid->NegVerticalOffset(), finalSize.Height);

        // Clip the RowsPresenter so rows cannot overlap other elements in certain styling scenarios
        Media::RectangleGeometry const rg{};
        rg.Rect({0, 0, finalSize.Width, static_cast<float>(finalHeight)});
        Clip(rg);

        return {finalSize.Width, static_cast<float>(finalHeight)};
    }

    Windows::Foundation::Size DataGridRowsPresenter::MeasureOverride(Windows::Foundation::Size const& availableSize) const
    {
        auto const internalOwningGrid = InternalOwningGrid();
        if (availableSize.Height == 0 || !internalOwningGrid)
        {
            return base_type::MeasureOverride(availableSize);
        }

        // If the Width of our RowsPresenter changed then we need to invalidate our rows
        auto const invalidateRows =
            (!internalOwningGrid->RowsPresenterAvailableSize().has_value() || availableSize.Width != internalOwningGrid->RowsPresenterAvailableSize()->Width) &&
            !std::isinf(availableSize.Width);

        // The DataGrid uses the RowsPresenter available size in order to autogrow
        // and calculate the scrollbars
        internalOwningGrid->RowsPresenterAvailableSize(availableSize);

        internalOwningGrid->OnRowsMeasure();

        auto totalHeight = -internalOwningGrid->NegVerticalOffset();
        auto const totalCellsWidth = internalOwningGrid->ColumnsInternal().VisibleEdgedColumnsWidth();

        double headerWidth = 0;
        for(auto const& element : internalOwningGrid->DisplayData().GetScrollingElements())
        {
            auto const row = element.try_as<DataGridRow>();
            if (row)
            {
                if (invalidateRows)
                {
                    row->InvalidateMeasure();
                }
            }

            element.Measure({std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity()});

            if (row && row->HeaderCell())
            {
                headerWidth = std::max<double>(headerWidth, row->HeaderCell().DesiredSize().Width);
            }
            else
            {
                
                if (auto const groupHeader = element.try_as<DataGridRowGroupHeader>();
                    groupHeader && groupHeader->HeaderCell())
                {
                    headerWidth = std::max<double>(headerWidth, groupHeader->HeaderCell().DesiredSize().Width);
                }
            }

            totalHeight += element.DesiredSize().Height;
        }

        internalOwningGrid->RowHeadersDesiredWidth(headerWidth);

        // Could be positive infinity depending on the DataGrid's bounds
        internalOwningGrid->AvailableSlotElementRoom(availableSize.Height - totalHeight);

        // TODO: totalHeight can be negative if we've just collapsed details.  This is a workaround,
        // the real fix is to correct NegVerticalOffset.
        totalHeight = std::max(0.0, totalHeight);

        return {static_cast<float>(totalCellsWidth + headerWidth), static_cast<float>(totalHeight)};
    }

    Automation::Peers::AutomationPeer DataGridRowsPresenter::OnCreateAutomationPeer() const
    {
        return MiniDumpExplorer::DataGridRowsPresenterAutomationPeer{ *this };
    }

    void DataGridRowsPresenter::DataGridRowsPresenter_ManipulationStarting([[maybe_unused]] Windows::Foundation::IInspectable const& sender, [[maybe_unused]] Input::ManipulationStartingRoutedEventArgs const& e)
    {
        if (auto const internalOwningGrid = InternalOwningGrid();
            internalOwningGrid)
        {
            assert(internalOwningGrid->IsEnabled());

            preManipulationHorizontalOffset_ = internalOwningGrid->HorizontalOffset();
            preManipulationVerticalOffset_ = internalOwningGrid->VerticalOffset();
        }
    }

    void DataGridRowsPresenter::DataGridRowsPresenter_ManipulationStarted([[maybe_unused]] Windows::Foundation::IInspectable const& sender, Input::ManipulationStartedRoutedEventArgs const& e)
    {
        if (e.PointerDeviceType() != Microsoft::UI::Input::PointerDeviceType::Touch)
        {
            e.Complete();
        }
    }

    void DataGridRowsPresenter::DataGridRowsPresenter_ManipulationDelta([[maybe_unused]] Windows::Foundation::IInspectable const& sender, Input::ManipulationDeltaRoutedEventArgs const& e) const
    {
        if (auto const internalOwningGrid = InternalOwningGrid();
            internalOwningGrid)
        {
            e.Handled(internalOwningGrid->ProcessScrollOffsetDelta(preManipulationHorizontalOffset_ - e.Cumulative().Translation.X - internalOwningGrid->HorizontalOffset(), true /*isForHorizontalScroll*/) ||
                internalOwningGrid->ProcessScrollOffsetDelta(preManipulationVerticalOffset_ - e.Cumulative().Translation.Y - internalOwningGrid->VerticalOffset(), false /*isForHorizontalScroll*/));
        }
    }
}
