#include "pch.h"
#include "DataGridCell.h"

#include <winrt/Microsoft.UI.Xaml.Input.h>
#include <winrt/Windows.UI.Xaml.Interop.h>

#include "DataGridColumnCollection.h"
#include "DataGridError.h"
#include "DataGridFillerColumn.h"
#include "DataGridRow.h"
#include "Helpers/UIHelper.h"
#include "Helpers/VisualStates.h"
#include "Utility/SuspendedHandlers.h"


#if __has_include("DataGridCell.g.cpp")
// ReSharper disable once CppUnusedIncludeDirective
#include "DataGridCell.g.cpp"  // NOLINT(bugprone-suspicious-include)
#endif

#include "DataGridCellInternal.h"

using namespace winrt;
using namespace Microsoft::UI::Xaml;
using namespace std::string_view_literals;

namespace winrt::MiniDumpExplorer::implementation
{
    struct DataGridCellAutomationPeer;

    namespace
    {
        hstring const ElementRightGridLine = L"RightGridLine";
    }

    DataGridCell::DataGridCell()
    {
        IsTapEnabled(true);
        AddHandler(UIElement::TappedEvent(), winrt::box_value<Input::TappedEventHandler>({[](Windows::Foundation::IInspectable const& sender, Input::TappedRoutedEventArgs const& e) { sender.as<DataGridCell>()->DataGridCell_PointerTapped(e); } }), true /*handledEventsToo*/);

        PointerCanceled(Input::PointerEventHandler{ [](Windows::Foundation::IInspectable const& sender, Input::PointerRoutedEventArgs const& e) { sender.as<DataGridCell>()->DataGridCell_PointerCanceled(e); } });
        PointerCaptureLost(Input::PointerEventHandler{ [](Windows::Foundation::IInspectable const& sender, Input::PointerRoutedEventArgs const& e) { sender.as<DataGridCell>()->DataGridCell_PointerCaptureLost(e); } });
        PointerPressed(Input::PointerEventHandler{ [](Windows::Foundation::IInspectable const& sender, Input::PointerRoutedEventArgs const& e) { sender.as<DataGridCell>()->DataGridCell_PointerPressed(e); } });
        PointerReleased(Input::PointerEventHandler{ [](Windows::Foundation::IInspectable const& sender, Input::PointerRoutedEventArgs const& e) { sender.as<DataGridCell>()->DataGridCell_PointerReleased(e); } });
        PointerEntered(Input::PointerEventHandler{ [](Windows::Foundation::IInspectable const& sender, Input::PointerRoutedEventArgs const& e) { sender.as<DataGridCell>()->DataGridCell_PointerEntered(e); } });
        PointerExited(Input::PointerEventHandler{ [](Windows::Foundation::IInspectable const& sender, Input::PointerRoutedEventArgs const& e) { sender.as<DataGridCell>()->DataGridCell_PointerExited(e); } });
        PointerMoved(Input::PointerEventHandler{ [](Windows::Foundation::IInspectable const& sender, Input::PointerRoutedEventArgs const& e) { sender.as<DataGridCell>()->DataGridCell_PointerMoved(e); } });

        DefaultStyleKey(box_value(L"MiniDumpExplorer.DataGridCell"));
    }

    DependencyProperty DataGridCell::IsValidProperty()
    {
        static DependencyProperty s_IsValidProperty = DependencyProperty::Register(
            L"IsValid",
            xaml_typename<bool>(),
            xaml_typename<MiniDumpExplorer::DataGridCell>(),
            PropertyMetadata{ box_value(true), [](DependencyObject const& d, DependencyPropertyChangedEventArgs const& e) { d.as<DataGridCell>()->OnIsValidPropertyChanged(e); } });

        return s_IsValidProperty;
    }

    bool DataGridCell::IsValid() const
    {
        return unbox_value<bool>(GetValue(IsValidProperty()));
    }

    void DataGridCell::IsValid(bool const value) const
    {
        SetValue(IsValidProperty(), box_value(value));
    }

    double DataGridCell::ActualRightGridLineWidth() const
    {
        if (rightGridLine_)
        {
            return rightGridLine_.ActualWidth();
        }

        return 0.0;
    }

    int32_t DataGridCell::ColumnIndex() const
    {
        if (auto const internalOwningColumn = InternalOwningColumn(); internalOwningColumn)
        {
            return internalOwningColumn->Index();
        }

        return InvalidIndex;
    }

    bool DataGridCell::IsCurrent() const
    {
        auto const internalOwningGrid = InternalOwningGrid();
        auto const internalOwningColumn = InternalOwningColumn();
        auto const internalOwningRow = InternalOwningRow();
        assert(internalOwningGrid && internalOwningColumn && internalOwningRow);

        return internalOwningGrid->CurrentColumnIndex() == internalOwningColumn->Index() &&
               internalOwningGrid->CurrentSlot() == internalOwningRow->Slot();
    }

    bool DataGridCell::IsPointerOver() const
    {
        return interactionInfo_.has_value() && interactionInfo_.value().IsPointerOver;
    }

    void DataGridCell::IsPointerOver(bool const value)
    {
        if (value && !interactionInfo_.has_value())
        {
            interactionInfo_ = DataGridInternal::DataGridInteractionInfo{};
        }

        if (interactionInfo_.has_value())
        {
            interactionInfo_.value().IsPointerOver = value;
        }

        ApplyCellState(true /*animate*/);
    }

    void DataGridCell::OwningColumn(MiniDumpExplorer::DataGridColumn const& owningColumn)
    {
        owningColumn_ = owningColumn;
    }

    void DataGridCell::OwningRow(MiniDumpExplorer::DataGridRow const& owningRow)
    {
        owningRow_ = owningRow;
    }

    MiniDumpExplorer::DataGrid DataGridCell::OwningGrid() const
    {
        if (auto const internalOwningRow = InternalOwningRow(); internalOwningRow && internalOwningRow->OwningGrid())
        {
            return internalOwningRow->OwningGrid();
        }

        if (auto const internalOwningColumn = InternalOwningColumn(); internalOwningColumn && internalOwningColumn->OwningGrid())
        {
            return internalOwningColumn->OwningGrid();
        }

        return nullptr;
    }

    int32_t DataGridCell::RowIndex() const
    {
        if (!owningRow_)
        {
            return InvalidIndex;
        }

        return owningRow_.Index();
    }

    void DataGridCell::OnApplyTemplate()
    {
        base_type::OnApplyTemplate();

        ApplyCellState(false /*animate*/);

        rightGridLine_ = GetTemplateChild(ElementRightGridLine).as<Shapes::Rectangle>();
        if (rightGridLine_ && !owningColumn_)
        {
            // Turn off the right GridLine for filler cells
            rightGridLine_.Visibility(Visibility::Collapsed);
        }
        else
        {
            EnsureGridLine(nullptr);
        }
    }

    Automation::Peers::AutomationPeer DataGridCell::OnCreateAutomationPeer() const
    {
        if (auto const internalOwningGrid = InternalOwningGrid();
            internalOwningGrid &&
            owningColumn_ &&
            owningColumn_ != internalOwningGrid->ColumnsInternal().FillerColumn())
        {
            return MiniDumpExplorer::DataGridCellAutomationPeer{*this};
        }

        return base_type::OnCreateAutomationPeer();
    }

    void DataGridCell::ApplyCellState(bool const animate) const
    {
        auto const internalOwningGrid = InternalOwningGrid();
        auto const internalOwningRow = InternalOwningRow();
        if (!internalOwningGrid || !OwningColumn() || !internalOwningRow || internalOwningRow->Visibility() == Visibility::Collapsed || internalOwningRow->Slot() == DataGridRow::InvalidIndex)
        {
            return;
        }

        // CommonStates
        if (IsPointerOver())
        {
            VisualStates::GoToState(*this, animate, VisualStates::StatePointerOver, VisualStates::StateNormal);
        }
        else
        {
            VisualStates::GoToState(*this, animate, VisualStates::StateNormal);
        }

        // SelectionStates
        if (internalOwningRow->IsSelected())
        {
            VisualStates::GoToState(*this, animate, VisualStates::StateSelected, VisualStates::StateUnselected);
        }
        else
        {
            VisualStates::GoToState(*this, animate, VisualStates::StateUnselected);
        }

        // CurrentStates
        if (IsCurrent() && !internalOwningGrid->ColumnHeaderHasFocus())
        {
            if (internalOwningGrid->ContainsFocus())
            {
                VisualStates::GoToState(*this, animate, VisualStates::StateCurrentWithFocus, VisualStates::StateCurrent, VisualStates::StateRegular);
            }
            else
            {
                VisualStates::GoToState(*this, animate, VisualStates::StateCurrent, VisualStates::StateRegular);
            }
        }
        else
        {
            VisualStates::GoToState(*this, animate, VisualStates::StateRegular);
        }

        // Interaction states
        if (IsEdited())
        {
            VisualStates::GoToState(*this, animate, VisualStates::StateEditing, VisualStates::StateDisplay);
        }
        else
        {
            VisualStates::GoToState(*this, animate, VisualStates::StateDisplay);
        }

        // Validation states
        if (IsValid())
        {
            VisualStates::GoToState(*this, animate, VisualStates::StateValid);
        }
        else
        {
            VisualStates::GoToState(*this, animate, VisualStates::StateInvalid, VisualStates::StateValid);
        }
    }

    void DataGridCell::EnsureGridLine(MiniDumpExplorer::DataGridColumn const& lastVisibleColumn) const
    {
        auto const internalOwningGrid = InternalOwningGrid();
        if (!internalOwningGrid || !rightGridLine_)
        {
            return;
        }

        if (!OwningColumn().try_as<DataGridFillerColumn>() && internalOwningGrid->VerticalGridLinesBrush() && internalOwningGrid->VerticalGridLinesBrush() != rightGridLine_.Fill())
        {
            rightGridLine_.Fill(internalOwningGrid->VerticalGridLinesBrush());
        }

        if (auto const newVisibility =
            (internalOwningGrid->GridLinesVisibility() == DataGridGridLinesVisibility::Vertical || internalOwningGrid->GridLinesVisibility() == DataGridGridLinesVisibility::All) &&
            (internalOwningGrid->ColumnsInternal().FillerColumn().as<DataGridFillerColumn>()->IsActive() || OwningColumn() != lastVisibleColumn)
            ? Visibility::Visible : Visibility::Collapsed;
            newVisibility != rightGridLine_.Visibility())
        {
            rightGridLine_.Visibility(newVisibility);
        }
    }

    void DataGridCell::EnsureStyle(Microsoft::UI::Xaml::Style const& previousStyle) const
    {
        auto const owningColumn = OwningColumn();
        auto const owningGrid = OwningGrid();
        if (auto const currentStyle = Style();
            currentStyle &&
            (!owningColumn || currentStyle != owningColumn.CellStyle()) &&
            (!owningGrid || currentStyle != owningGrid.CellStyle()) &&
            currentStyle != previousStyle)
        {
            return;
        }

        Microsoft::UI::Xaml::Style style{nullptr};
        if (owningColumn)
        {
            style = owningColumn.CellStyle();
        }

        if (!style && owningGrid)
        {
            style = owningGrid.CellStyle();
        }

        UIHelper::SetStyleWithType(*this, style);
    }

    void DataGridCell::Recycle()
    {
        interactionInfo_ = std::nullopt;
    }

    bool DataGridCell::IsEdited() const
    {
        assert(OwningGrid());

        auto const internalOwningGrid = InternalOwningGrid();
        return internalOwningGrid->EditingRow() == OwningRow() &&
               internalOwningGrid->EditingColumnIndex() == ColumnIndex();

    }

    void DataGridCell::CancelPointer(Input::PointerRoutedEventArgs const& e)
    {
        if (interactionInfo_.has_value() && interactionInfo_.value().CapturedPointerId == e.Pointer().PointerId())
        {
            interactionInfo_.value().CapturedPointerId = 0;
        }

        IsPointerOver(false);
    }

    void DataGridCell::OnIsValidPropertyChanged(DependencyPropertyChangedEventArgs const& e) const
    {
        if (!SuspendedHandlers::IsHandlerSuspended(*this, e.Property()))
        {
            SuspendedHandlers::SetValueNoCallback(*this, IsValidProperty(), e.OldValue());
            throw DataGridError::DataGrid::UnderlyingPropertyIsReadOnly(L"IsValid"sv);
        }
    }

    void DataGridCell::DataGridCell_PointerTapped(Input::TappedRoutedEventArgs const& e) const
    {
        auto const internalOwningGrid = InternalOwningGrid();

        // OwningGrid is null for TopLeftHeaderCell and TopRightHeaderCell because they have no OwningRow
        if (!internalOwningGrid || internalOwningGrid->HasColumnUserInteraction())
        {
            return;
        }

        if (!e.Handled() && internalOwningGrid->IsTabStop())
        {
            [[maybe_unused]] bool const success = internalOwningGrid->Focus(FocusState::Programmatic);
            assert(success);
        }

        if (auto const internalOwningRow = InternalOwningRow();
            internalOwningRow)
        {
            e.Handled(internalOwningGrid->UpdateStateOnTapped(e, ColumnIndex(), internalOwningRow->Slot(), !e.Handled() /*allowEdit*/));
            internalOwningGrid->UpdatedStateOnTapped(true);
        }
    }

    void DataGridCell::DataGridCell_PointerCanceled(Input::PointerRoutedEventArgs const& e)
    {
        CancelPointer(e);
    }

    void DataGridCell::DataGridCell_PointerCaptureLost(Input::PointerRoutedEventArgs const& e)
    {
        CancelPointer(e);
    }

    void DataGridCell::DataGridCell_PointerPressed(Input::PointerRoutedEventArgs const& e)
    {
        if (auto const internalOwningGrid = InternalOwningGrid();
            e.Pointer().PointerDeviceType() == Microsoft::UI::Input::PointerDeviceType::Touch &&
            internalOwningGrid &&
            internalOwningGrid->AllowsManipulation() &&
            (!interactionInfo_.has_value() || interactionInfo_.value().CapturedPointerId == 0u) &&
            CapturePointer(e.Pointer()))
        {
            if (!interactionInfo_.has_value())
            {
                interactionInfo_ = DataGridInternal::DataGridInteractionInfo{};
            }

            interactionInfo_.value().CapturedPointerId = e.Pointer().PointerId();
        }
    }

    void DataGridCell::DataGridCell_PointerReleased(Input::PointerRoutedEventArgs const& e) const
    {
        if (interactionInfo_.has_value() && interactionInfo_.value().CapturedPointerId == e.Pointer().PointerId())
        {
            ReleasePointerCapture(e.Pointer());
        }
    }

    void DataGridCell::DataGridCell_PointerEntered([[maybe_unused]] Input::PointerRoutedEventArgs const& e)
    {
        UpdateIsPointerOver(true);
    }

    void DataGridCell::DataGridCell_PointerExited([[maybe_unused]] Input::PointerRoutedEventArgs const& e)
    {
        UpdateIsPointerOver(false);
    }

    void DataGridCell::DataGridCell_PointerMoved([[maybe_unused]] Input::PointerRoutedEventArgs const& e)
    {
        UpdateIsPointerOver(true);
    }

    void DataGridCell::UpdateIsPointerOver(bool const isPointerOver)
    {
        if (interactionInfo_.has_value() && interactionInfo_.value().CapturedPointerId == 0)
        {
            return;
        }

        IsPointerOver(isPointerOver);
    }
}
