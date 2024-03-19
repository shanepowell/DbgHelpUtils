#include "pch.h"
#include "DataGridColumnHeader.h"

#include <winrt/Microsoft.UI.Input.h>
#include <winrt/Microsoft.UI.Xaml.Input.h>
#include <winrt/Windows.UI.Xaml.Interop.h>
// ReSharper disable once CppUnusedIncludeDirective
#include <winrt/Windows.UI.Core.h>

#include "DataGrid.h"
#include "DataGridAutomationPeer.h"
#include "DataGridColumn.h"
#include "DataGridColumnCollection.h"
// ReSharper disable once CppUnusedIncludeDirective
#include "DataGridColumnInternal.h"
#include "DataGridColumnHeadersPresenter.h"
// ReSharper disable once CppUnusedIncludeDirective
#include "DataGridColumnReorderingEventArgs.h"
#include "DataGridError.h"
#include "DataGridFillerColumn.h"
#include "DbgHelpUtils/double_utils.h"
#include "Helpers/UIHelper.h"
#include "Helpers/VisualStates.h"
#include "Utility/logger.h"
#include "Utility/SuspendedHandlers.h"

#if __has_include("DataGridColumnHeader.g.cpp")
// ReSharper disable once CppUnusedIncludeDirective
#include "DataGridColumnHeader.g.cpp"  // NOLINT(bugprone-suspicious-include)
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    struct DataGridColumnHeaderAutomationPeer;

    using drag_mode = DataGridInternal::DataGridColumnHeaderInteractionInfo::drag_mode;

    namespace
    {
        constexpr int DragThreshold = 2;
        constexpr int ResizeRegionWidthStrict = 6;
        constexpr int ResizeRegionWidthLoose = 9;
    }

    auto DataGridColumnHeader::InternalOwningGrid() const
    {
        return OwningGrid().try_as<DataGrid>();
    }

    auto DataGridColumnHeader::InternalOwningColumn() const
    {
        return owningColumn_.try_as<DataGridColumn>();
    }

    DataGridColumnHeader::DataGridColumnHeader()
    {
        AddHandler(UIElement::DoubleTappedEvent(), winrt::box_value<Input::DoubleTappedEventHandler>({ [](Windows::Foundation::IInspectable const& sender, Input::DoubleTappedRoutedEventArgs const& e) { sender.as<DataGridColumnHeader>()->DataGridColumnHeader_DoubleTapped(e); } }), true /*handledEventsToo*/);

        pointerCanceledEvent_ = PointerCanceled(Input::PointerEventHandler{ this, &DataGridColumnHeader::DataGridColumnHeader_PointerCanceled });
        pointerCaptureLostEvent_ = PointerCaptureLost(Input::PointerEventHandler{ this, &DataGridColumnHeader::DataGridColumnHeader_PointerCaptureLost });
        pointerPressedEvent_ = PointerPressed(Input::PointerEventHandler{ this, &DataGridColumnHeader::DataGridColumnHeader_PointerPressed });
        pointerReleasedEvent_ = PointerReleased(Input::PointerEventHandler{ this, &DataGridColumnHeader::DataGridColumnHeader_PointerReleased });
        pointerMovedEvent_ = PointerMoved(Input::PointerEventHandler{ this, &DataGridColumnHeader::DataGridColumnHeader_PointerMoved });
        pointerEnteredEvent_ = PointerEntered(Input::PointerEventHandler{ this, &DataGridColumnHeader::DataGridColumnHeader_PointerEntered });
        pointerExitedEvent_ = PointerExited(Input::PointerEventHandler{ this, &DataGridColumnHeader::DataGridColumnHeader_PointerExited });
        isEnabledChangedEvent_ = IsEnabledChanged(DependencyPropertyChangedEventHandler{ this, &DataGridColumnHeader::DataGridColumnHeader_IsEnabledChanged });

        DefaultStyleKey(box_value(L"MiniDumpExplorer.DataGridColumnHeader"));
    }

    DependencyProperty DataGridColumnHeader::SeparatorBrushProperty()
    {
        static DependencyProperty property = DependencyProperty::Register(
            L"SeparatorBrush",
            xaml_typename<Media::Brush>(),
            xaml_typename<MiniDumpExplorer::DataGridColumnHeader>(),
            PropertyMetadata{ nullptr });

        return property;
    }

    DependencyProperty DataGridColumnHeader::SeparatorVisibilityProperty()
    {
        static DependencyProperty property = DependencyProperty::Register(
            L"SeparatorVisibility",
            xaml_typename<Microsoft::UI::Xaml::Visibility>(),
            xaml_typename<MiniDumpExplorer::DataGridColumnHeader>(),
            PropertyMetadata{ box_value(Visibility::Visible), [](auto const& d, auto const& e) { d.template as<DataGridColumnHeader>()->OnSeparatorVisibilityPropertyChanged(d, e); } });

        return property;
    }

    Media::Brush DataGridColumnHeader::SeparatorBrush() const
    {
        return GetValue(SeparatorBrushProperty()).as<Media::Brush>();
    }

    void DataGridColumnHeader::SeparatorBrush(Media::Brush const& value) const
    {
        SetValue(SeparatorBrushProperty(), value);
    }

    Visibility DataGridColumnHeader::SeparatorVisibility() const
    {
        return unbox_value<Microsoft::UI::Xaml::Visibility>(GetValue(SeparatorVisibilityProperty()));
    }

    void DataGridColumnHeader::SeparatorVisibility(Microsoft::UI::Xaml::Visibility const value) const
    {
        SetValue(SeparatorVisibilityProperty(), box_value(value));
    }

    int32_t DataGridColumnHeader::ColumnIndex() const
    {
        if(auto const owningColumn = InternalOwningColumn();
            owningColumn)
        {
            return owningColumn->Index();
        }
        return DataGridColumn::InvalidIndex;
    }

    MiniDumpExplorer::DataGrid DataGridColumnHeader::OwningGrid() const
    {
        if(auto const internalOwningColumn = InternalOwningColumn();
            internalOwningColumn)
        {
            return internalOwningColumn->OwningGrid();
        }

        return nullptr;
    }

    bool DataGridColumnHeader::HasFocus() const
    {
        auto const internalOwningGrid = InternalOwningGrid();
        return internalOwningGrid &&
            OwningColumn() == internalOwningGrid->FocusedColumn() &&
            internalOwningGrid->ColumnHeaderHasFocus();
    }

    void DataGridColumnHeader::OnApplyTemplate() const
    {
        base_type::OnApplyTemplate();
        ApplyState(false /*useTransitions*/);
    }

    void DataGridColumnHeader::OnContentChanged(Windows::Foundation::IInspectable const& oldContent, Windows::Foundation::IInspectable const& newContent) const
    {
        if(newContent.try_as<UIElement>())
        {
            throw DataGridError::DataGridColumnHeader::ContentDoesNotSupportUIElements();
        }

        base_type::OnContentChanged(oldContent, newContent);
    }

    Automation::Peers::AutomationPeer DataGridColumnHeader::OnCreateAutomationPeer() const
    {
        if (auto const internalOwningGrid = InternalOwningGrid();
            internalOwningGrid && OwningColumn() != internalOwningGrid->ColumnsInternal().FillerColumn())
        {
            return MiniDumpExplorer::DataGridColumnHeaderAutomationPeer{*this};
        }

        return base_type::OnCreateAutomationPeer();
    }

    void DataGridColumnHeader::ApplyState(bool const useTransitions) const
    {
        auto const internalOwningGrid = InternalOwningGrid();

        // Common States
        if (auto const dragMode = !internalOwningGrid ? drag_mode::None : internalOwningGrid->ColumnHeaderInteractionInfo().DragMode;
            isPressed_ && dragMode != drag_mode::Resize)
        {
            VisualStates::GoToState(*this, useTransitions, VisualStates::StatePressed, VisualStates::StatePointerOver, VisualStates::StateNormal);
        }
        else if (isPointerOver_ && dragMode != drag_mode::Resize)
        {
            VisualStates::GoToState(*this, useTransitions, VisualStates::StatePointerOver, VisualStates::StateNormal);
        }
        else
        {
            VisualStates::GoToState(*this, useTransitions, VisualStates::StateNormal);
        }

        // Focus States
        if (HasFocus())
        {
            VisualStates::GoToState(*this, useTransitions, VisualStates::StateFocused, VisualStates::StateRegular);
        }
        else
        {
            VisualStates::GoToState(*this, useTransitions, VisualStates::StateUnfocused);
        }

        // Sort States
        if (auto const internalOwningColumn = InternalOwningColumn();
            internalOwningColumn)
        {
            if(internalOwningColumn->SortDirection())
            {
                switch (internalOwningColumn->SortDirection().Value())
                {
                case DataGridSortDirection::Ascending:
                    VisualStates::GoToState(*this, useTransitions, VisualStates::StateSortAscending, VisualStates::StateUnsorted);
                    break;
                case DataGridSortDirection::Descending:
                    VisualStates::GoToState(*this, useTransitions, VisualStates::StateSortDescending, VisualStates::StateUnsorted);
                    break;
                }
            }
            else
            {
                VisualStates::GoToState(*this, useTransitions, VisualStates::StateUnsorted);
            }
        }
    }

    void DataGridColumnHeader::EnsureStyle(Microsoft::UI::Xaml::Style const& previousStyle) const
    {
        auto const internalOwningColumn = InternalOwningColumn();
        auto const internalOwningGrid = InternalOwningGrid();
        if (auto const currentStyle = Style();
            currentStyle &&
            currentStyle != previousStyle &&
            (!internalOwningColumn || currentStyle != internalOwningColumn->HeaderStyle()) &&
            (!internalOwningGrid || currentStyle != internalOwningGrid->ColumnHeaderStyle()))
        {
            return;
        }

        Microsoft::UI::Xaml::Style style{nullptr};
        if (internalOwningColumn)
        {
            style = internalOwningColumn->HeaderStyle();
        }

        if (!style && internalOwningGrid)
        {
            style = internalOwningGrid->ColumnHeaderStyle();
        }

        UIHelper::SetStyleWithType(*this, style);
    }

    void DataGridColumnHeader::InvokeProcessSort()
    {
        auto const internalOwningGrid = InternalOwningGrid();
        assert(internalOwningGrid);

        if (internalOwningGrid->WaitForLostFocus([weakPtr = get_weak()] 
            {
                if(auto const instance = weakPtr.get())
                {
                    instance->InvokeProcessSort();
                }
            }))
        {
            return;
        }

        if (internalOwningGrid->CommitEdit(DataGridEditingUnit::Row, true /*exitEditingMode*/))
        {
            ProcessSort();
        }
    }

    void DataGridColumnHeader::UpdateSeparatorVisibility(MiniDumpExplorer::DataGridColumn const& lastVisibleColumn) const
    {
        auto newVisibility = desiredSeparatorVisibility_;

        // Collapse separator for the last column if there is no filler column
        if (auto const internalOwningGrid = InternalOwningGrid();
            OwningColumn() &&
            internalOwningGrid &&
            desiredSeparatorVisibility_ == Visibility::Visible &&
            OwningColumn() == lastVisibleColumn &&
            !internalOwningGrid->ColumnsInternal().FillerColumn().as<DataGridFillerColumn>()->IsActive())
        {
            newVisibility = Visibility::Collapsed;
        }

        // Update the public property if it has changed
        if (SeparatorVisibility() != newVisibility)
        {
            SuspendedHandlers::SetValueNoCallback(*this, SeparatorVisibilityProperty(), box_value(newVisibility));
        }
    }

    bool DataGridColumnHeader::TrySetResizeColumn(uint32_t const pointerId, MiniDumpExplorer::DataGridColumn const& column) const
    {
        // If DataGrid.CanUserResizeColumns == false, then the column can still override it
        if (auto const internalOwningGrid = InternalOwningGrid();
            internalOwningGrid && CanResizeColumn(column))
        {
            auto& interactionInfo = internalOwningGrid->ColumnHeaderInteractionInfo();

            assert(interactionInfo.DragMode != drag_mode::None);

            interactionInfo.DragColumn = column;
            interactionInfo.DragMode = drag_mode::Resize;
            interactionInfo.DragPointerId = pointerId;

            return true;
        }

        return false;
    }

    bool DataGridColumnHeader::CanReorderColumn(MiniDumpExplorer::DataGridColumn const& column)
    {
        auto const internalColumn = column.as<DataGridColumn>();
        if (auto const internalOwningGrid = internalColumn->InternalOwningGrid();
            internalOwningGrid && internalOwningGrid->UsesStarSizing() &&
            (internalOwningGrid->ColumnsInternal().LastVisibleColumn() == column || !double_utils::are_close(internalOwningGrid->ColumnsInternal().VisibleEdgedColumnsWidth(), internalOwningGrid->CellsWidth())))
        {
            return false;
        }

        return internalColumn->ActualCanUserResize();
    }

    void DataGridColumnHeader::CancelPointer(Input::PointerRoutedEventArgs const& e)
    {
        // When the user stops interacting with the column headers, the drag mode needs to be reset and any open popups closed.
        auto const internalOwningGrid = InternalOwningGrid();
        if (!internalOwningGrid)
        {
            return;
        }

        isPressed_ = false;
        isPointerOver_ = false;

        auto const& interactionInfo = internalOwningGrid->ColumnHeaderInteractionInfo();
        auto setResizeCursor = false;

        if (auto const& columnHeaders = internalOwningGrid->ColumnHeaders();
            columnHeaders)
        {
            auto const pointerPositionHeaders = e.GetCurrentPoint(columnHeaders).Position();
            setResizeCursor = interactionInfo.DragMode == drag_mode::Resize && pointerPositionHeaders.X > 0 && pointerPositionHeaders.X < internalOwningGrid->ActualWidth();
        }

        if (!setResizeCursor)
        {
            SetOriginalCursor();
        }

        if (interactionInfo.DragPointerId == e.Pointer().PointerId())
        {
            internalOwningGrid->ResetColumnHeaderInteractionInfo();
        }

        if (setResizeCursor)
        {
            SetResizeCursor(e.Pointer(), e.GetCurrentPoint(*this).Position());
        }

        ApplyState(false /*useTransitions*/);
    }

    MiniDumpExplorer::DataGridColumn DataGridColumnHeader::GetReorderingTargetColumn(Windows::Foundation::Point& pointerPositionHeaders, bool const scroll, double& scrollAmount) const
    {
        auto const internalOwningGrid = InternalOwningGrid();
        assert(internalOwningGrid);

        scrollAmount = 0;
        double leftEdge = 0;

        if (internalOwningGrid->ColumnsInternal().RowGroupSpacerColumn().as<DataGridFillerColumn>()->IsRepresented())
        {
            leftEdge = internalOwningGrid->ColumnsInternal().RowGroupSpacerColumn().ActualWidth();
        }

        auto const& interactionInfo = internalOwningGrid->ColumnHeaderInteractionInfo();
        double rightEdge = internalOwningGrid->CellsWidth();
        if (OwningColumn().IsFrozen())
        {
            rightEdge = std::min(rightEdge, interactionInfo.FrozenColumnsWidth);
        }
        else if (internalOwningGrid->FrozenColumnCount() > 0)
        {
            leftEdge = interactionInfo.FrozenColumnsWidth;
        }

        if (pointerPositionHeaders.X < leftEdge)
        {
            if (auto const scrollBar = internalOwningGrid->HorizontalScrollBar();
                scroll &&
                scrollBar &&
                scrollBar.Visibility() == Visibility::Visible &&
                scrollBar.Value() > 0)
            {
                auto const newVal = pointerPositionHeaders.X - leftEdge;
                scrollAmount = std::min(newVal, scrollBar.Value());
                internalOwningGrid->UpdateHorizontalOffset(scrollAmount + scrollBar.Value());
            }

            pointerPositionHeaders.X = static_cast<float>(leftEdge);
        }
        else if (pointerPositionHeaders.X >= rightEdge)
        {
            if (auto const scrollBar = internalOwningGrid->HorizontalScrollBar();
                scroll &&
                scrollBar &&
                scrollBar.Visibility() == Visibility::Visible &&
                scrollBar.Value() < scrollBar.Maximum())
            {
                auto const newVal = pointerPositionHeaders.X - rightEdge;
                scrollAmount = std::min(newVal, scrollBar.Maximum() - scrollBar.Value());
                internalOwningGrid->UpdateHorizontalOffset(scrollAmount + scrollBar.Value());
            }

            pointerPositionHeaders.X = static_cast<float>(rightEdge - 1);
        }

        for(auto const& column : internalOwningGrid->ColumnsInternal().GetDisplayedColumns())
        {
            auto const internalColumn = column.as<DataGridColumn>();
            auto const headerCell = internalColumn->HeaderCell();
            auto const pointerPosition = UIHelper::Translate(internalOwningGrid->ColumnHeaders(), headerCell, pointerPositionHeaders);
            auto const columnMiddle = headerCell.ActualWidth() / 2;
            if (pointerPosition.X >= 0 && pointerPosition.X <= columnMiddle)
            {
                return column;
            }

            if (pointerPosition.X > columnMiddle && pointerPosition.X < headerCell.ActualWidth())
            {
                return internalOwningGrid->ColumnsInternal().GetNextVisibleColumn(column);
            }
        }

        return nullptr;
    }

    int32_t DataGridColumnHeader::GetReorderingTargetDisplayIndex(Windows::Foundation::Point& pointerPositionHeaders) const
    {
        auto const internalOwningGrid = InternalOwningGrid();
        assert(internalOwningGrid);

        double scrollAmount;
        
        if (auto const targetColumn = GetReorderingTargetColumn(pointerPositionHeaders, false /*scroll*/, scrollAmount);
            targetColumn)
        {
            return targetColumn.DisplayIndex() > OwningColumn().DisplayIndex() ? targetColumn.DisplayIndex() - 1 : targetColumn.DisplayIndex();
        }
        else
        {
            return internalOwningGrid->Columns().Size() - 1;
        }
    }

    void DataGridColumnHeader::DataGridColumnHeader_PointerCanceled([[maybe_unused]] Windows::Foundation::IInspectable const& sender, Input::PointerRoutedEventArgs const& e)
    {
        CancelPointer(e);
    }

    void DataGridColumnHeader::DataGridColumnHeader_PointerCaptureLost([[maybe_unused]] Windows::Foundation::IInspectable const& sender, Input::PointerRoutedEventArgs const& e)
    {
        CancelPointer(e);
    }

    void DataGridColumnHeader::DataGridColumnHeader_PointerPressed([[maybe_unused]] Windows::Foundation::IInspectable const& sender, Input::PointerRoutedEventArgs const& e)
    {
        auto const internalOwningGrid = InternalOwningGrid();
        if (!internalOwningGrid || !OwningColumn() || e.Handled() || !IsEnabled() || internalOwningGrid->ColumnHeaderInteractionInfo().DragMode != drag_mode::None)
        {
            return;
        }

        auto const pointerPoint = e.GetCurrentPoint(*this);
        auto& interactionInfo = internalOwningGrid->ColumnHeaderInteractionInfo();

        if (e.Pointer().PointerDeviceType() == Microsoft::UI::Input::PointerDeviceType::Mouse && !pointerPoint.Properties().IsLeftButtonPressed())
        {
            return;
        }

        assert(interactionInfo.DragPointerId == 0);

        auto handled = e.Handled();

        isPressed_ = true;

        if (internalOwningGrid->ColumnHeaders())
        {
            auto const pointerPosition = pointerPoint.Position();

            if (CapturePointer(e.Pointer()))
            {
                interactionInfo.CapturedPointer = e.Pointer();
            }
            else
            {
                interactionInfo.CapturedPointer = nullptr;
            }

            assert(interactionInfo.DragMode == drag_mode::None);
            assert(!interactionInfo.DragColumn);
            interactionInfo.DragMode = drag_mode::PointerPressed;
            interactionInfo.DragPointerId = e.Pointer().PointerId();
            interactionInfo.FrozenColumnsWidth = internalOwningGrid->ColumnsInternal().GetVisibleFrozenEdgedColumnsWidth();
            interactionInfo.PressedPointerPositionHeaders = interactionInfo.LastPointerPositionHeaders = UIHelper::Translate(*this, internalOwningGrid->ColumnHeaders(), pointerPosition);

            const auto distanceFromLeft = pointerPosition.X;
            const auto distanceFromRight = ActualWidth() - distanceFromLeft;
            const auto currentColumn = OwningColumn();
            MiniDumpExplorer::DataGridColumn previousColumn{nullptr};
            if (!OwningColumn().try_as<MiniDumpExplorer::DataGridFillerColumn>())
            {
                previousColumn = internalOwningGrid->ColumnsInternal().GetPreviousVisibleNonFillerColumn(currentColumn);
            }

            if (auto const resizeRegionWidth = e.Pointer().PointerDeviceType() == Microsoft::UI::Input::PointerDeviceType::Touch ? ResizeRegionWidthLoose : ResizeRegionWidthStrict;
                distanceFromRight <= resizeRegionWidth)
            {
                handled = TrySetResizeColumn(e.Pointer().PointerId(), currentColumn);
            }
            else if (distanceFromLeft <= resizeRegionWidth && previousColumn)
            {
                handled = TrySetResizeColumn(e.Pointer().PointerId(), previousColumn);
            }
            else if(interactionInfo.ResizePointerId != 0)
            {
                logger::Log().LogMessage(log_level::warning, std::format(L"Failed to setup resize dragging: [{0}/{1}]", distanceFromRight, distanceFromLeft));
            }

            if (interactionInfo.DragMode == drag_mode::Resize && interactionInfo.DragColumn)
            {
                interactionInfo.DragStart = interactionInfo.LastPointerPositionHeaders;
                interactionInfo.OriginalWidth = interactionInfo.DragColumn.ActualWidth();
                interactionInfo.OriginalHorizontalOffset = internalOwningGrid->HorizontalOffset();

                handled = true;
            }
        }

        e.Handled(handled);

        ApplyState(true /*useTransitions*/);
    }

    void DataGridColumnHeader::DataGridColumnHeader_PointerReleased([[maybe_unused]] Windows::Foundation::IInspectable const& sender, Input::PointerRoutedEventArgs const& e)
    {
        auto const internalOwningGrid = InternalOwningGrid();
        if (!internalOwningGrid || !OwningColumn() || e.Handled() || !IsEnabled())
        {
            return;
        }

        auto const pointerPoint = e.GetCurrentPoint(*this);

        if (e.Pointer().PointerDeviceType() == Microsoft::UI::Input::PointerDeviceType::Mouse && pointerPoint.Properties().IsLeftButtonPressed())
        {
            return;
        }

        auto const& interactionInfo = internalOwningGrid->ColumnHeaderInteractionInfo();

        if (interactionInfo.DragPointerId != 0 && interactionInfo.DragPointerId != e.Pointer().PointerId())
        {
            return;
        }

        auto const pointerPosition = pointerPoint.Position();
        auto pointerPositionHeaders = e.GetCurrentPoint(internalOwningGrid->ColumnHeaders()).Position();
        bool handled = e.Handled();

        isPressed_ = false;

        if (internalOwningGrid->ColumnHeaders())
        {
            switch (interactionInfo.DragMode)
            {
            case drag_mode::PointerPressed:
            {
                // Completed a click or tap without dragging, so raise the DataGrid.Sorting event.
                InvokeProcessSort();
                break;
            }

            case drag_mode::Reorder:
            {
                // Find header hovered over
                if (auto const targetIndex = GetReorderingTargetDisplayIndex(pointerPositionHeaders);
                    (!OwningColumn().IsFrozen() && targetIndex >= internalOwningGrid->FrozenColumnCount()) ||
                    (OwningColumn().IsFrozen() && targetIndex < internalOwningGrid->FrozenColumnCount()))
                {
                    OwningColumn().DisplayIndex(targetIndex);

                    internalOwningGrid->OnColumnReordered(MiniDumpExplorer::DataGridColumnEventArgs{OwningColumn()});
                }

                internalOwningGrid->OnColumnHeaderDragCompleted(Controls::Primitives::DragCompletedEventArgs{pointerPosition.X - interactionInfo.DragStart.value().X, pointerPosition.Y - interactionInfo.DragStart.value().Y, false});
                break;
            }

            case drag_mode::Drag:
            {
                internalOwningGrid->OnColumnHeaderDragCompleted(Controls::Primitives::DragCompletedEventArgs{0, 0, false});
                break;
            }

            default:
                break;
            }

            SetResizeCursor(e.Pointer(), pointerPosition);

            // Variables that track drag mode states get reset in DataGridColumnHeader_LostPointerCapture
            if (interactionInfo.CapturedPointer)
            {
                ReleasePointerCapture(interactionInfo.CapturedPointer);
            }

            internalOwningGrid->ResetColumnHeaderInteractionInfo();
            handled = true;
        }

        e.Handled(handled);

        ApplyState(true /*useTransitions*/);
    }

    void DataGridColumnHeader::DataGridColumnHeader_PointerMoved([[maybe_unused]] Windows::Foundation::IInspectable const& sender, Input::PointerRoutedEventArgs const& e)
    {
        auto const internalOwningGrid = InternalOwningGrid();
        if (!OwningColumn() || !internalOwningGrid || !internalOwningGrid->ColumnHeaders() || !IsEnabled())
        {
            return;
        }

        auto const pointerPoint = e.GetCurrentPoint(*this);
        auto const pointerPosition = pointerPoint.Position();

        if (auto& interactionInfo = internalOwningGrid->ColumnHeaderInteractionInfo();
            pointerPoint.IsInContact() && (interactionInfo.DragPointerId == 0 || interactionInfo.DragPointerId == e.Pointer().PointerId()))
        {
            auto pointerPositionHeaders = e.GetCurrentPoint(internalOwningGrid->ColumnHeaders()).Position();
            auto handled = false;

            assert(internalOwningGrid->Parent().try_as<UIElement>());

            auto const distanceFromLeft = pointerPosition.X;
            auto const distanceFromRight = ActualWidth() - distanceFromLeft;

            OnPointerMove_Resize(handled, pointerPositionHeaders);
            OnPointerMove_Reorder(handled, e.Pointer(), pointerPosition, pointerPositionHeaders, distanceFromLeft, distanceFromRight);

            // If nothing was done about moving the pointer while the pointer is down, remember the dragging, but do not
            // claim the event was actually handled.
            if (interactionInfo.DragMode == drag_mode::PointerPressed &&
                interactionInfo.PressedPointerPositionHeaders.has_value() &&
                std::abs(interactionInfo.PressedPointerPositionHeaders.value().X - pointerPositionHeaders.X) + std::abs(interactionInfo.PressedPointerPositionHeaders.value().Y - pointerPositionHeaders.Y) > DragThreshold)
            {
                interactionInfo.DragMode = drag_mode::Drag;
                interactionInfo.DragPointerId = e.Pointer().PointerId();
            }

            if (interactionInfo.DragMode == drag_mode::Drag)
            {
                internalOwningGrid->OnColumnHeaderDragDelta(Controls::Primitives::DragDeltaEventArgs{pointerPositionHeaders.X - interactionInfo.LastPointerPositionHeaders.value().X, pointerPositionHeaders.Y - interactionInfo.LastPointerPositionHeaders.value().Y});
            }

            interactionInfo.LastPointerPositionHeaders = pointerPositionHeaders;
        }

        SetResizeCursor(e.Pointer(), pointerPosition);

        if (!isPointerOver_)
        {
            isPointerOver_ = true;
            ApplyState(true /*useTransitions*/);
        }
    }

    void DataGridColumnHeader::DataGridColumnHeader_PointerEntered([[maybe_unused]] Windows::Foundation::IInspectable const& sender, Input::PointerRoutedEventArgs const& e)
    {
        if (!IsEnabled() || !OwningGrid())
        {
            return;
        }

        isPointerOver_ = true;

        SetResizeCursor(e.Pointer(), e.GetCurrentPoint(*this).Position());

        ApplyState(true /*useTransitions*/);
    }

    void DataGridColumnHeader::DataGridColumnHeader_PointerExited([[maybe_unused]] Windows::Foundation::IInspectable const& sender, Input::PointerRoutedEventArgs const& e)
    {
        if (!IsEnabled() || !OwningGrid())
        {
            return;
        }

        isPointerOver_ = false;

        if (auto const& interactionInfo = InternalOwningGrid()->ColumnHeaderInteractionInfo();
            interactionInfo.DragMode == drag_mode::None && interactionInfo.ResizePointerId == e.Pointer().PointerId())
        {
            SetOriginalCursor();
        }

        ApplyState(true /*useTransitions*/);
    }

    void DataGridColumnHeader::DataGridColumnHeader_IsEnabledChanged([[maybe_unused]] Windows::Foundation::IInspectable const& sender, DependencyPropertyChangedEventArgs const& e)
    {
        
        if (auto const internalOwningGrid = InternalOwningGrid();internalOwningGrid && !unbox_value<bool>(e.NewValue()))
        {
            isPressed_ = false;
            isPointerOver_ = false;

            if (auto const& interactionInfo = internalOwningGrid->ColumnHeaderInteractionInfo();
                interactionInfo.CapturedPointer)
            {
                ReleasePointerCapture(interactionInfo.CapturedPointer);
            }

            ApplyState(false /*useTransitions*/);
        }
    }

    void DataGridColumnHeader::DataGridColumnHeader_DoubleTapped(Input::DoubleTappedRoutedEventArgs const& e) const
    {
        auto const internalOwningGrid = InternalOwningGrid();
        if (!internalOwningGrid || !OwningColumn() || e.Handled() || !IsEnabled())
        {
            return;
        }

        auto& interactionInfo = internalOwningGrid->ColumnHeaderInteractionInfo();
        if(interactionInfo.DragMode != drag_mode::Resize)
        {
            return;
        }

        interactionInfo.DragColumn.Width(MiniDumpExplorer::DataGridLength::SizeToCells());
        e.Handled(true);
    }

    void DataGridColumnHeader::OnPointerMove_BeginReorder(uint32_t const pointerId, Windows::Foundation::Point const& pointerPosition) const
    {
        auto const internalOwningGrid = InternalOwningGrid();
        assert(internalOwningGrid);

        MiniDumpExplorer::DataGridColumnHeader const dragIndicator{};
        auto const internalDragIndicator = dragIndicator.as<DataGridColumnHeader>();

        internalDragIndicator->OwningColumn(OwningColumn());
        dragIndicator.IsEnabled(false);
        dragIndicator.Content(Content());
        dragIndicator.ContentTemplate(ContentTemplate());

        Controls::ContentControl const dropLocationIndicator{};
        UIHelper::SetStyleWithType(dropLocationIndicator, internalOwningGrid->DropLocationIndicatorStyle());

        if (auto const internalOwningColumn = InternalOwningColumn();
            internalOwningColumn->DragIndicatorStyle())
        {
            UIHelper::SetStyleWithType(dragIndicator, internalOwningColumn->DragIndicatorStyle());
        }
        else if (internalOwningGrid->DragIndicatorStyle())
        {
            UIHelper::SetStyleWithType(dragIndicator, internalOwningGrid->DragIndicatorStyle());
        }

        // If the user didn't style the dragIndicator's Width, default it to the column header's width.
        if (std::isnan(dragIndicator.Width()))
        {
            dragIndicator.Width(ActualWidth());
        }

        // If the user didn't style the dropLocationIndicator's Height, default to the column header's height.
        if (std::isnan(dropLocationIndicator.Height()))
        {
            dropLocationIndicator.Height(ActualHeight());
        }

        // pass the caret's data template to the user for modification.
        MiniDumpExplorer::DataGridColumnReorderingEventArgs const columnReorderingEventArgs{OwningColumn()};
        columnReorderingEventArgs.DropLocationIndicator(dropLocationIndicator);
        columnReorderingEventArgs.DragIndicator(dragIndicator);
        internalOwningGrid->OnColumnReordering(columnReorderingEventArgs);
        if (columnReorderingEventArgs.Cancel())
        {
            return;
        }

        auto& interactionInfo = internalOwningGrid->ColumnHeaderInteractionInfo();

        // The app didn't cancel, so prepare for the reorder.
        interactionInfo.DragColumn = OwningColumn();
        assert(interactionInfo.DragMode != drag_mode::None);
        interactionInfo.DragMode = drag_mode::Reorder;
        interactionInfo.DragPointerId = pointerId;
        interactionInfo.DragStart = pointerPosition;

        // Display the reordering thumb.
        auto const columnHeaders = internalOwningGrid->ColumnHeaders().as<DataGridColumnHeadersPresenter>();
        columnHeaders->DragColumn(OwningColumn());
        columnHeaders->DragIndicator(columnReorderingEventArgs.DragIndicator());
        columnHeaders->DropLocationIndicator(columnReorderingEventArgs.DropLocationIndicator());
    }

    void DataGridColumnHeader::OnPointerMove_Reorder(bool& handled, Input::Pointer const& pointer, Windows::Foundation::Point const& pointerPosition, Windows::Foundation::Point& pointerPositionHeaders, double const distanceFromLeft, double const distanceFromRight) const
    {
        auto const internalOwningGrid = InternalOwningGrid();
        assert(internalOwningGrid);

        if (handled)
        {
            return;
        }

        auto const& interactionInfo = internalOwningGrid->ColumnHeaderInteractionInfo();

        // Handle entry into reorder mode
        if (auto const resizeRegionWidth = pointer.PointerDeviceType() == Microsoft::UI::Input::PointerDeviceType::Touch ? ResizeRegionWidthLoose : ResizeRegionWidthStrict;
            interactionInfo.DragMode == drag_mode::PointerPressed &&
            !interactionInfo.DragColumn &&
            distanceFromRight > resizeRegionWidth &&
            distanceFromLeft > resizeRegionWidth &&
            interactionInfo.PressedPointerPositionHeaders.has_value() &&
            std::abs(interactionInfo.PressedPointerPositionHeaders.value().X - pointerPositionHeaders.X) + std::abs(interactionInfo.PressedPointerPositionHeaders.value().Y - pointerPositionHeaders.Y) > DragThreshold)
        {
            Controls::Primitives::DragStartedEventArgs const dragStartedEventArgs{pointerPositionHeaders.X - interactionInfo.LastPointerPositionHeaders.value().X, pointerPositionHeaders.Y - interactionInfo.LastPointerPositionHeaders.value().Y};
            internalOwningGrid->OnColumnHeaderDragStarted(dragStartedEventArgs);

            handled = CanReorderColumn(OwningColumn());

            if (handled)
            {
                OnPointerMove_BeginReorder(pointer.PointerId(), pointerPosition);
            }
        }

        // Handle reorder mode (eg, positioning of the popup)
        if (auto const columnHeader = internalOwningGrid->ColumnHeaders().try_as<DataGridColumnHeadersPresenter>();
            interactionInfo.DragMode == drag_mode::Reorder && columnHeader->DragIndicator())
        {
            Controls::Primitives::DragDeltaEventArgs const dragDeltaEventArgs{pointerPositionHeaders.X - interactionInfo.LastPointerPositionHeaders.value().X, pointerPositionHeaders.Y - interactionInfo.LastPointerPositionHeaders.value().Y};
            internalOwningGrid->OnColumnHeaderDragDelta(dragDeltaEventArgs);

            // Find header we're hovering over
            double scrollAmount;
            auto targetColumn = GetReorderingTargetColumn(pointerPositionHeaders, !OwningColumn().IsFrozen() /*scroll*/, scrollAmount);

            columnHeader->DragIndicatorOffset(pointerPosition.X - interactionInfo.DragStart.value().X + scrollAmount);
            columnHeader->InvalidateArrange();

            if (columnHeader->DropLocationIndicator())
            {
                Windows::Foundation::Point targetPosition{0, 0};
                if (!targetColumn || targetColumn == internalOwningGrid->ColumnsInternal().FillerColumn() || targetColumn.IsFrozen() != OwningColumn().IsFrozen())
                {
                    targetColumn = internalOwningGrid->ColumnsInternal().GetLastColumn(true /*isVisible*/, OwningColumn().IsFrozen() /*isFrozen*/, std::nullopt /*isReadOnly*/);
                    targetPosition = UIHelper::Translate(targetColumn.as<DataGridColumn>()->HeaderCell(), internalOwningGrid->ColumnHeaders(), targetPosition);
                    targetPosition.X += static_cast<float>(targetColumn.ActualWidth());
                }
                else
                {
                    targetPosition = UIHelper::Translate(targetColumn.as<DataGridColumn>()->HeaderCell(), internalOwningGrid->ColumnHeaders(), targetPosition);
                }

                internalOwningGrid->ColumnHeaders().as<DataGridColumnHeadersPresenter>()->DropLocationIndicatorOffset(targetPosition.X - scrollAmount);
            }

            handled = true;
        }
    }

    void DataGridColumnHeader::OnPointerMove_Resize(bool& handled, Windows::Foundation::Point const& pointerPositionHeaders) const
    {
        auto const internalOwningGrid = InternalOwningGrid();
        assert(internalOwningGrid);

        if (auto const& interactionInfo = internalOwningGrid->ColumnHeaderInteractionInfo();
            !handled && interactionInfo.DragMode == drag_mode::Resize && interactionInfo.DragColumn && interactionInfo.DragStart.has_value())
        {
            assert(interactionInfo.ResizePointerId != 0);

            // Resize column
            auto const pointerDelta = pointerPositionHeaders.X - interactionInfo.DragStart.value().X;
            auto desiredWidth = interactionInfo.OriginalWidth + pointerDelta;

            auto const internalDragColumn = interactionInfo.DragColumn.as<DataGridColumn>();
            desiredWidth = std::max(internalDragColumn->ActualMinWidth(), std::min(internalDragColumn->ActualMaxWidth(), desiredWidth));
            internalDragColumn->Resize(interactionInfo.DragColumn.Width().Value(), interactionInfo.DragColumn.Width().UnitType(), interactionInfo.DragColumn.Width().DesiredValue(), desiredWidth, true);

            internalOwningGrid->UpdateHorizontalOffset(interactionInfo.OriginalHorizontalOffset);

            handled = true;
        }
    }

    void DataGridColumnHeader::SetOriginalCursor() const
    {
        auto const internalOwningGrid = InternalOwningGrid();
        assert(internalOwningGrid);

        if (auto& interactionInfo = internalOwningGrid->ColumnHeaderInteractionInfo();
            interactionInfo.ResizePointerId != 0)
        {
            ProtectedCursor(interactionInfo.OriginalCursor);
            interactionInfo.OriginalCursor = nullptr;
            interactionInfo.ResizePointerId = 0;
        }
    }

    void DataGridColumnHeader::SetResizeCursor(Input::Pointer const& pointer, Windows::Foundation::Point const& pointerPosition) const
    {
        auto const internalOwningGrid = InternalOwningGrid();

        if(!internalOwningGrid)
        {
            return;
        }

        auto& interactionInfo = internalOwningGrid->ColumnHeaderInteractionInfo();

        if (interactionInfo.DragMode != drag_mode::None || !OwningColumn())
        {
            return;
        }

        // Set mouse cursor if the column can be resized.
        auto const distanceFromLeft = pointerPosition.X;
        auto const distanceFromTop = pointerPosition.Y;
        auto const distanceFromRight = ActualWidth() - distanceFromLeft;
        auto const currentColumn = OwningColumn();
        MiniDumpExplorer::DataGridColumn previousColumn{nullptr};

        if (!OwningColumn().try_as<DataGridFillerColumn>())
        {
            previousColumn = internalOwningGrid->ColumnsInternal().GetPreviousVisibleNonFillerColumn(currentColumn);
        }

        auto const resizeRegionWidth = pointer.PointerDeviceType() == Microsoft::UI::Input::PointerDeviceType::Touch ? ResizeRegionWidthLoose : ResizeRegionWidthStrict;
        auto const nearCurrentResizableColumnRightEdge = distanceFromRight <= resizeRegionWidth && currentColumn && CanResizeColumn(currentColumn) && distanceFromTop < ActualHeight();
        // ReSharper disable once CppTooWideScopeInitStatement
        auto const nearPreviousResizableColumnLeftEdge = distanceFromLeft <= resizeRegionWidth && previousColumn && CanResizeColumn(previousColumn) && distanceFromTop < ActualHeight();

        if (internalOwningGrid->IsEnabled() && (nearCurrentResizableColumnRightEdge || nearPreviousResizableColumnLeftEdge))
        {
            if (interactionInfo.ResizePointerId == 0)
            {
                auto const pointerCursor = ProtectedCursor();
                interactionInfo.OriginalCursor = pointerCursor;
                interactionInfo.ResizePointerId = pointer.PointerId();
                auto const newCursor = Microsoft::UI::Input::InputCursor::CreateFromCoreCursor(Windows::UI::Core::CoreCursor{Windows::UI::Core::CoreCursorType::SizeWestEast, 0});
                ProtectedCursor(newCursor);
            }
        }
        else if (interactionInfo.ResizePointerId == pointer.PointerId())
        {
            SetOriginalCursor();
        }
    }

    bool DataGridColumnHeader::CanResizeColumn(MiniDumpExplorer::DataGridColumn const& column)
    {
        auto const internalColumn = column.as<DataGridColumn>();
        if (auto const internalOwningGrid = internalColumn->InternalOwningGrid();
            internalOwningGrid && internalOwningGrid->UsesStarSizing() &&
            (internalOwningGrid->ColumnsInternal().LastVisibleColumn() == column || !double_utils::are_close(internalOwningGrid->ColumnsInternal().VisibleEdgedColumnsWidth(), internalOwningGrid->CellsWidth())))
        {
            return false;
        }

        return internalColumn->ActualCanUserResize();
    }

    void DataGridColumnHeader::OnSeparatorVisibilityPropertyChanged([[maybe_unused]] DependencyObject const& d, DependencyPropertyChangedEventArgs const& e)
    {
        if(SuspendedHandlers::IsHandlerSuspended(*this, e.Property()))
        {
            desiredSeparatorVisibility_ = unbox_value<Microsoft::UI::Xaml::Visibility>(e.NewValue());
            UpdateSeparatorVisibility(OwningGrid() ? InternalOwningGrid()->ColumnsInternal().LastVisibleColumn() : nullptr);
        }
    }

    void DataGridColumnHeader::ProcessSort() const
    {
        try
        {
            auto const owningColumn = OwningColumn();
            auto const internalOwningColumn = InternalOwningColumn();

            if (auto const internalOwningGrid = InternalOwningGrid();
                internalOwningColumn &&
                internalOwningGrid &&
                !internalOwningGrid->EditingRow() &&
                owningColumn != internalOwningGrid->ColumnsInternal().FillerColumn() &&
                internalOwningGrid->CanUserSortColumns() &&
                internalOwningColumn->CanUserSort())
            {
                internalOwningGrid->OnColumnSorting(MiniDumpExplorer::DataGridColumnEventArgs{owningColumn});

                // Send the Invoked event for the column header's automation peer.
                DataGridAutomationPeer::RaiseAutomationInvokeEvent(*this);
            }
        }
        catch(...)
        {
            logger::Log().HandleUnknownException();
        }
    }
}
