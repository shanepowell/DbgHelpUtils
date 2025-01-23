#include "pch.h"
#include "DataGridRowHeader.h"

#include <winrt/Microsoft.UI.Xaml.Input.h>
#include <winrt/Windows.UI.Xaml.Interop.h>

#include "DataGrid.h"
#include "DataGridRow.h"
#include "DataGridRowGroupHeader.h"
#include "Helpers/UIHelper.h"
#include "Helpers/VisualStates.h"

#if __has_include("DataGridRowHeader.g.cpp")
// ReSharper disable once CppUnusedIncludeDirective
#include "DataGridRowHeader.g.cpp"  // NOLINT(bugprone-suspicious-include)
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    struct DataGridRowHeaderAutomationPeer;

    namespace
    {
        const hstring ElementRootName = L"RowHeaderRoot";

        const hstring StatePointerOver = L"PointerOver";
        const hstring StatePointerOverCurrentRow = L"PointerOverCurrentRow";
        const hstring StatePointerOverEditingRow = L"PointerOverUnfocusedEditingRow";
        const hstring StatePointerOverEditingRowFocused = L"PointerOverEditingRow";
        const hstring StatePointerOverSelected = L"PointerOverUnfocusedSelected";
        const hstring StatePointerOverSelectedCurrentRow = L"PointerOverUnfocusedCurrentRowSelected";
        const hstring StatePointerOverSelectedCurrentRowFocused = L"PointerOverCurrentRowSelected";
        const hstring StatePointerOverSelectedFocused = L"PointerOverSelected";
        const hstring StateNormal = L"Normal";
        const hstring StateNormalCurrentRow = L"NormalCurrentRow";
        const hstring StateNormalEditingRow = L"UnfocusedEditingRow";
        const hstring StateNormalEditingRowFocused = L"NormalEditingRow";
        const hstring StateSelected = L"UnfocusedSelected";
        const hstring StateSelectedCurrentRow = L"UnfocusedCurrentRowSelected";
        const hstring StateSelectedCurrentRowFocused = L"NormalCurrentRowSelected";
        const hstring StateSelectedFocused = L"NormalSelected";

        constexpr byte StatePointerOverCode = 0;
        constexpr byte StatePointerOverCurrentRowCode = 1;
        constexpr byte StatePointerOverEditingRowCode = 2;
        constexpr byte StatePointerOverEditingRowFocusedCode = 3;
        constexpr byte StatePointerOverSelectedCode = 4;
        constexpr byte StatePointerOverSelectedCurrentRowCode = 5;
        constexpr byte StatePointerOverSelectedCurrentRowFocusedCode = 6;
        constexpr byte StatePointerOverSelectedFocusedCode = 7;
        constexpr byte StateNormalCode = 8;
        constexpr byte StateNormalCurrentRowCode = 9;
        constexpr byte StateNormalEditingRowCode = 10;
        constexpr byte StateNormalEditingRowFocusedCode = 11;
        constexpr byte StateSelectedCode = 12;
        constexpr byte StateSelectedCurrentRowCode = 13;
        constexpr byte StateSelectedCurrentRowFocusedCode = 14;
        constexpr byte StateSelectedFocusedCode = 15;
        constexpr byte StateNullCode = 255;

        constexpr std::array FallbackStateMapping =
        {
            StateNormalCode,
            StateNormalCurrentRowCode,
            StatePointerOverEditingRowFocusedCode,
            StateNormalEditingRowFocusedCode,
            StatePointerOverSelectedFocusedCode,
            StatePointerOverSelectedCurrentRowFocusedCode,
            StateSelectedFocusedCode,
            StateSelectedFocusedCode,
            StateNullCode,
            StateNormalCode,
            StateNormalEditingRowFocusedCode,
            StateSelectedCurrentRowFocusedCode,
            StateSelectedFocusedCode,
            StateSelectedCurrentRowFocusedCode,
            StateNormalCurrentRowCode,
            StateNormalCode,
        };

        constexpr std::array IdealStateMapping =
        {
            StateNormalCode,
            StateNormalCode,
            StatePointerOverCode,
            StatePointerOverCode,
            StateNullCode,
            StateNullCode,
            StateNullCode,
            StateNullCode,
            StateSelectedCode,
            StateSelectedFocusedCode,
            StatePointerOverSelectedCode,
            StatePointerOverSelectedFocusedCode,
            StateNormalEditingRowCode,
            StateNormalEditingRowFocusedCode,
            StatePointerOverEditingRowCode,
            StatePointerOverEditingRowFocusedCode,
            StateNormalCurrentRowCode,
            StateNormalCurrentRowCode,
            StatePointerOverCurrentRowCode,
            StatePointerOverCurrentRowCode,
            StateNullCode,
            StateNullCode,
            StateNullCode,
            StateNullCode,
            StateSelectedCurrentRowCode,
            StateSelectedCurrentRowFocusedCode,
            StatePointerOverSelectedCurrentRowCode,
            StatePointerOverSelectedCurrentRowFocusedCode,
            StateNormalEditingRowCode,
            StateNormalEditingRowFocusedCode,
            StatePointerOverEditingRowCode,
            StatePointerOverEditingRowFocusedCode
        };

        constexpr std::array StateNames =
        {
            &StatePointerOver,
            &StatePointerOverCurrentRow,
            &StatePointerOverEditingRow,
            &StatePointerOverEditingRowFocused,
            &StatePointerOverSelected,
            &StatePointerOverSelectedCurrentRow,
            &StatePointerOverSelectedCurrentRowFocused,
            &StatePointerOverSelectedFocused,
            &StateNormal,
            &StateNormalCurrentRow,
            &StateNormalEditingRow,
            &StateNormalEditingRowFocused,
            &StateSelected,
            &StateSelectedCurrentRow,
            &StateSelectedCurrentRowFocused,
            &StateSelectedFocused
        };
    }

    DataGridRowHeader::DataGridRowHeader()
    {
        IsTapEnabled(true);
        AddHandler(UIElement::TappedEvent(), winrt::box_value<Input::TappedEventHandler>({[](Windows::Foundation::IInspectable const& sender, Input::TappedRoutedEventArgs const& e) { sender.as<DataGridRowHeader>()->DataGridRowHeader_Tapped(e); } }), true /*handledEventsToo*/);

        DefaultStyleKey(box_value(L"MiniDumpExplorer.DataGridRowHeader"));
    }

    DependencyProperty DataGridRowHeader::SeparatorBrushProperty()
    {
        static DependencyProperty s_SeparatorBrushProperty = DependencyProperty::Register(
            L"SeparatorBrush",
            xaml_typename<Media::Brush>(),
            xaml_typename<MiniDumpExplorer::DataGridRowHeader>(),
            PropertyMetadata{ nullptr });

        return s_SeparatorBrushProperty;
    }

    DependencyProperty DataGridRowHeader::SeparatorVisibilityProperty()
    {
        static DependencyProperty s_SeparatorVisibilityProperty = DependencyProperty::Register(
            L"SeparatorVisibility",
            xaml_typename<Microsoft::UI::Xaml::Visibility>(),
            xaml_typename<MiniDumpExplorer::DataGridRowHeader>(),
            PropertyMetadata{ box_value(Visibility::Visible) });

        return s_SeparatorVisibilityProperty;
    }

    Media::Brush DataGridRowHeader::SeparatorBrush() const
    {
        return GetValue(SeparatorBrushProperty()).as<Media::Brush>();
    }

    void DataGridRowHeader::SeparatorBrush(Media::Brush const& value) const
    {
        SetValue(SeparatorBrushProperty(), value);
    }

    Visibility DataGridRowHeader::SeparatorVisibility() const
    {
        return unbox_value<Microsoft::UI::Xaml::Visibility>(GetValue(SeparatorVisibilityProperty()));
    }

    void DataGridRowHeader::SeparatorVisibility(Microsoft::UI::Xaml::Visibility const value) const
    {
        SetValue(SeparatorVisibilityProperty(), box_value(value));
    }

    void DataGridRowHeader::OnApplyTemplate()
    {
        base_type::OnApplyTemplate();

        rootElement_ = GetTemplateChild(ElementRootName).try_as<FrameworkElement>();
        if (rootElement_)
        {
            ApplyOwnerState(false /*animate*/);
        }
    }

    Windows::Foundation::Size DataGridRowHeader::MeasureOverride(Windows::Foundation::Size const& availableSize) const
    {
        if (!OwningRow())
        {
            return base_type::MeasureOverride(availableSize);
        }

        auto const owningGrid = OwningGrid().try_as<DataGrid>();
        if (!owningGrid)
        {
            return base_type::MeasureOverride(availableSize);
        }

        auto const measureHeight = std::isnan(owningGrid->RowHeight()) ? availableSize.Height : owningGrid->RowHeight();
        auto const measureWidth = std::isnan(owningGrid->RowHeaderWidth()) ? availableSize.Width : owningGrid->RowHeaderWidth();
        auto measuredSize = base_type::MeasureOverride({static_cast<float>(measureWidth), static_cast<float>(measureHeight)});

        // Auto grow the row header or force it to a fixed width based on the DataGrid's setting
        if (!std::isnan(owningGrid->RowHeaderWidth()) || measuredSize.Width < owningGrid->ActualRowHeaderWidth())
        {
            return {static_cast<float>(owningGrid->ActualRowHeaderWidth()), measuredSize.Height};
        }

        return measuredSize;
    }

    Automation::Peers::AutomationPeer DataGridRowHeader::OnCreateAutomationPeer() const
    {
        return MiniDumpExplorer::DataGridRowHeaderAutomationPeer{*this};
    }

    void DataGridRowHeader::ApplyOwnerState(bool const animate) const
    {
        if(!rootElement_)
        {
            return;
        }

        if(auto const owner = Owner();
            !owner || owner.Visibility() == Visibility::Visible)
        {
            return;
        }

        byte idealStateMappingIndex = 0;

        auto const internalOwningGrid = OwningGrid().try_as<DataGrid>();
        if (auto const internalOwningRow = OwningRow().try_as<DataGridRow>();
            internalOwningRow)
        {
            if (internalOwningRow->IsValid())
            {
                VisualStates::GoToState(*this, true, VisualStates::StateRowValid);
            }
            else
            {
                VisualStates::GoToState(*this, true, VisualStates::StateRowInvalid, VisualStates::StateRowValid);
            }

            if (internalOwningGrid)
            {
                if (internalOwningGrid->CurrentSlot() == internalOwningRow->Slot())
                {
                    idealStateMappingIndex += 16;
                }

                if (internalOwningGrid->ContainsFocus())
                {
                    idealStateMappingIndex += 1;
                }
            }

            if (internalOwningRow->IsSelected() || internalOwningRow->IsEditing())
            {
                idealStateMappingIndex += 8;
            }

            if (internalOwningRow->IsEditing())
            {
                idealStateMappingIndex += 4;
            }

            if (internalOwningRow->IsPointerOver())
            {
                idealStateMappingIndex += 2;
            }
        }
        else if (auto const internalOwningRowGroupHeader = OwningRowGroupHeader().try_as<DataGridRowGroupHeader>();
            internalOwningRowGroupHeader && internalOwningGrid && internalOwningGrid->CurrentSlot() == internalOwningRowGroupHeader->RowGroupInfo()->Slot())
        {
            idealStateMappingIndex += 16;
        }

        auto stateCode = IdealStateMapping[idealStateMappingIndex];
        assert(stateCode != StateNullCode);

        while (stateCode != StateNullCode)
        {
            if (VisualStateManager::GoToState(*this, *StateNames[stateCode], animate))
            {
                break;
            }

            // The state wasn't implemented so fall back to the next one
            stateCode = FallbackStateMapping[stateCode];
        }
    }

    void DataGridRowHeader::EnsureStyle(Microsoft::UI::Xaml::Style const& previousStyle) const
    {
        auto const currentStyle = Style();
        auto const owningRow = OwningRow();
        auto const owningGrid = OwningGrid();
        if (auto const owningRowGroupHeader = OwningRowGroupHeader();
            currentStyle &&
            owningRow &&
            currentStyle != owningRow.HeaderStyle() &&
            owningRowGroupHeader &&
            currentStyle != owningRowGroupHeader.HeaderStyle() &&
            owningGrid &&
            currentStyle != owningGrid.RowHeaderStyle() &&
            currentStyle != previousStyle)
        {
            return;
        }

        Microsoft::UI::Xaml::Style style{nullptr};
        if (owningRow)
        {
            style = owningRow.HeaderStyle();
        }

        if (!style && owningGrid)
        {
            style = owningGrid.RowHeaderStyle();
        }

        UIHelper::SetStyleWithType(*this, style);
    }

    MiniDumpExplorer::DataGrid DataGridRowHeader::OwningGrid() const
    {
        if(auto const owningRow = OwningRow().try_as<DataGridRow>();
            owningRow)
        {
            return owningRow->OwningGrid();
        }

        if(auto const owningRowGroupHeader = OwningRowGroupHeader().try_as<DataGridRowGroupHeader>();
            owningRowGroupHeader)
        {
            return owningRowGroupHeader->OwningGrid();
        }

        return nullptr;
    }

    MiniDumpExplorer::DataGridRow DataGridRowHeader::OwningRow() const
    {
        return owner_.try_as<MiniDumpExplorer::DataGridRow>();
    }

    MiniDumpExplorer::DataGridRowGroupHeader DataGridRowHeader::OwningRowGroupHeader() const
    {
        return owner_.try_as<MiniDumpExplorer::DataGridRowGroupHeader>();
    }

    int32_t DataGridRowHeader::Slot() const
    {
        if (auto const owningRow = OwningRow();
            owningRow)
        {
            return owningRow.as<DataGridRow>()->Slot();
        }

        if (auto const owningRowGroupHeader = OwningRowGroupHeader();
            owningRowGroupHeader)
        {
            return owningRowGroupHeader.as<DataGridRowGroupHeader>()->RowGroupInfo()->Slot();
        }

        return InvalidIndex;
    }

    void DataGridRowHeader::DataGridRowHeader_Tapped(Input::TappedRoutedEventArgs const& e) const
    {
        auto const internalOwningGrid = OwningGrid().try_as<DataGrid>();
        if(!internalOwningGrid || internalOwningGrid->HasColumnUserInteraction())
        {
            return;
        }

        if (!e.Handled() && internalOwningGrid->IsTabStop())
        {
            [[maybe_unused]] bool const success = internalOwningGrid->Focus(FocusState::Programmatic);
            assert(success);
        }

        if (OwningRow())
        {
            e.Handled(internalOwningGrid->UpdateStateOnTapped(e, InvalidIndex, Slot(), false /*allowEdit*/));
            internalOwningGrid->UpdatedStateOnTapped(true);
        }
    }
}
