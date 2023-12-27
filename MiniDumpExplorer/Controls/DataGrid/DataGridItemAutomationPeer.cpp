#include "pch.h"
#include "DataGridItemAutomationPeer.h"

#include "DataGrid.h"
#include "DataGridDataConnection.h"
#include "DataGridDisplayData.h"
#include "DataGridError.h"
#include "DataGridRow.h"
#include "DbgHelpUtils/wide_runtime_error.h"

#if __has_include("DataGridItemAutomationPeer.g.cpp")
// ReSharper disable once CppUnusedIncludeDirective
#include "DataGridItemAutomationPeer.g.cpp"  // NOLINT(bugprone-suspicious-include)
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    DataGridItemAutomationPeer::DataGridItemAutomationPeer(Windows::Foundation::IInspectable const& item, MiniDumpExplorer::DataGrid const& dataGrid)
        : base_type(dataGrid)
        , item_{item}
        , dataGridAutomationPeer_{Automation::Peers::FrameworkElementAutomationPeer::CreatePeerForElement(dataGrid)}
    {
        if(!item)
        {
            throw dlg_help_utils::exceptions::wide_runtime_error{L"invalid argument: item"};
        }

        if(!dataGrid)
        {
            throw dlg_help_utils::exceptions::wide_runtime_error{L"invalid argument: dataGrid"};
        }
    }

    MiniDumpExplorer::DataGridRowAutomationPeer DataGridItemAutomationPeer::OwningRowPeer() const
    {
        if (auto const row = OwningRow();
            row)
        {
            return Automation::Peers::FrameworkElementAutomationPeer::CreatePeerForElement(row).as<MiniDumpExplorer::DataGridRowAutomationPeer>();
        }

        return {nullptr};
    }

    hstring DataGridItemAutomationPeer::GetAcceleratorKeyCore() const
    {
        auto const owningRowPeer = OwningRowPeer();
        return owningRowPeer ? owningRowPeer.GetAcceleratorKey() : hstring{};
    }

    hstring DataGridItemAutomationPeer::GetAccessKeyCore() const
    {
        auto const owningRowPeer = OwningRowPeer();
        return owningRowPeer ? owningRowPeer.GetAccessKey() : hstring{};
    }

    // ReSharper disable once CppMemberFunctionMayBeStatic
    Automation::Peers::AutomationControlType DataGridItemAutomationPeer::GetAutomationControlTypeCore() const
    {
        return Automation::Peers::AutomationControlType::DataItem;
    }

    // ReSharper disable once CppMemberFunctionMayBeStatic
    hstring DataGridItemAutomationPeer::GetAutomationIdCore() const
    {
        return {};
    }

    Windows::Foundation::Rect DataGridItemAutomationPeer::GetBoundingRectangleCore() const
    {
        auto const owningRowPeer = OwningRowPeer();
        return owningRowPeer ? owningRowPeer.GetBoundingRectangle() : Windows::Foundation::Rect{};
    }

    Windows::Foundation::Collections::IVector<Automation::Peers::AutomationPeer> DataGridItemAutomationPeer::GetChildrenCore() const
    {
        if(auto const owningRowPeer = OwningRowPeer();
            owningRowPeer)
        {
            owningRowPeer.InvalidatePeer();
            return owningRowPeer.GetChildren();
        }

        return single_threaded_vector<Automation::Peers::AutomationPeer>();
    }

    hstring DataGridItemAutomationPeer::GetClassNameCore() const
    {
        auto const owningRowPeer = OwningRowPeer();
        return owningRowPeer ? owningRowPeer.GetClassName() : hstring{};
    }

    Windows::Foundation::Point DataGridItemAutomationPeer::GetClickablePointCore() const
    {
        auto const owningRowPeer = OwningRowPeer();
        return owningRowPeer ? owningRowPeer.GetClickablePoint() : Windows::Foundation::Point{};
    }

    hstring DataGridItemAutomationPeer::GetHelpTextCore() const
    {
        auto const owningRowPeer = OwningRowPeer();
        return owningRowPeer ? owningRowPeer.GetHelpText() : hstring{};
    }

    hstring DataGridItemAutomationPeer::GetItemStatusCore() const
    {
        auto const owningRowPeer = OwningRowPeer();
        return owningRowPeer ? owningRowPeer.GetItemStatus() : hstring{};
    }

    hstring DataGridItemAutomationPeer::GetItemTypeCore() const
    {
        auto const owningRowPeer = OwningRowPeer();
        return owningRowPeer ? owningRowPeer.GetItemType() : hstring{};
    }

    Automation::Peers::AutomationPeer DataGridItemAutomationPeer::GetLabeledByCore() const
    {
        auto const owningRowPeer = OwningRowPeer();
        return owningRowPeer ? owningRowPeer.GetLabeledBy() : nullptr;
    }

    hstring DataGridItemAutomationPeer::GetLocalizedControlTypeCore() const
    {
        auto const owningRowPeer = OwningRowPeer();
        return owningRowPeer ? owningRowPeer.GetLocalizedControlType() : hstring{};
    }

    hstring DataGridItemAutomationPeer::GetNameCore() const
    {
        if(auto const owningRowPeer = OwningRowPeer();
            owningRowPeer)
        {
            if(auto owningPeerName = owningRowPeer.GetName();
                !owningPeerName.empty())
            {
                return owningPeerName;
            }
        }

        return L"row";
    }

    Automation::Peers::AutomationOrientation DataGridItemAutomationPeer::GetOrientationCore() const
    {
        auto const owningRowPeer = OwningRowPeer();
        return owningRowPeer ? owningRowPeer.GetOrientation() : Automation::Peers::AutomationOrientation::None;
    }

    Windows::Foundation::IInspectable DataGridItemAutomationPeer::GetPatternCore(Automation::Peers::PatternInterface const patternInterface) const
    {
        switch(patternInterface)
        {
        case Automation::Peers::PatternInterface::Invoke:
            if (auto const internalOwningDataGrid = OwningDataGrid().as<DataGrid>();
                !internalOwningDataGrid->IsReadOnly())
            {
                return *this;
            }

            break;
        case Automation::Peers::PatternInterface::ScrollItem:
            {
                auto const internalOwningDataGrid = OwningDataGrid().as<DataGrid>();
                if (auto const verticalScrollBar = internalOwningDataGrid->VerticalScrollBar();
                    verticalScrollBar &&
                    verticalScrollBar.Maximum() > 0)
                {
                    return *this;
                }
                break;
            }
        case Automation::Peers::PatternInterface::Selection:
        case Automation::Peers::PatternInterface::SelectionItem:
            return *this;

        default:
            break;
        }

        return base_type::GetPatternCore(patternInterface);
    }

    bool DataGridItemAutomationPeer::HasKeyboardFocusCore() const
    {
        auto const owningRowPeer = OwningRowPeer();
        return owningRowPeer ? owningRowPeer.HasKeyboardFocus() : false;
    }

    bool DataGridItemAutomationPeer::IsContentElementCore() const
    {
        auto const owningRowPeer = OwningRowPeer();
        return owningRowPeer ? owningRowPeer.IsContentElement() : true;
    }

    bool DataGridItemAutomationPeer::IsControlElementCore() const
    {
        auto const owningRowPeer = OwningRowPeer();
        return owningRowPeer ? owningRowPeer.IsControlElement() : true;
    }

    bool DataGridItemAutomationPeer::IsEnabledCore() const
    {
        auto const owningRowPeer = OwningRowPeer();
        return owningRowPeer ? owningRowPeer.IsEnabled() : false;
    }

    bool DataGridItemAutomationPeer::IsKeyboardFocusableCore() const
    {
        auto const owningRowPeer = OwningRowPeer();
        return owningRowPeer ? owningRowPeer.IsKeyboardFocusable() : false;
    }

    bool DataGridItemAutomationPeer::IsOffscreenCore() const
    {
        auto const owningRowPeer = OwningRowPeer();
        return owningRowPeer ? owningRowPeer.IsOffscreen() : true;
    }

    bool DataGridItemAutomationPeer::IsPasswordCore() const
    {
        auto const owningRowPeer = OwningRowPeer();
        return owningRowPeer ? owningRowPeer.IsPassword() : false;
    }

    bool DataGridItemAutomationPeer::IsRequiredForFormCore() const
    {
        auto const owningRowPeer = OwningRowPeer();
        return owningRowPeer ? owningRowPeer.IsRequiredForForm() : false;
    }

    void DataGridItemAutomationPeer::SetFocusCore() const
    {
        if (auto const owningRowPeer = OwningRowPeer();
            owningRowPeer)
        {
            owningRowPeer.SetFocus();
        }
    }

    void DataGridItemAutomationPeer::Invoke()
    {
        EnsureEnabled();

        if (!OwningRowPeer())
        {
            OwningDataGrid().ScrollIntoView(item_, nullptr);
        }

        if (auto const owningRow = OwningRow();
            owningRow)
        {
            auto const internalOwningDataGrid = OwningDataGrid().as<DataGrid>();
            if (internalOwningDataGrid->WaitForLostFocus([weakPtr = get_weak()]
                {
                    if(auto const instance = weakPtr.get())
                    {
                        instance->Invoke();
                    }
                }))
            {
                return;
            }

            if (internalOwningDataGrid->EditingRow() == owningRow)
            {
                [[maybe_unused]] auto const success = internalOwningDataGrid->CommitEdit(DataGridEditingUnit::Row, true /*exitEditing*/);
            }
            else if (internalOwningDataGrid->UpdateSelectionAndCurrency(internalOwningDataGrid->CurrentColumnIndex(), owningRow.as<DataGridRow>()->Slot(), DataGridInternal::DataGridSelectionAction::SelectCurrent, false))
            {
                [[maybe_unused]] auto const success = internalOwningDataGrid->BeginEdit();
            }
        }
    }

    void DataGridItemAutomationPeer::ScrollIntoView() const
    {
        OwningDataGrid().ScrollIntoView(item_, nullptr);
    }

    bool DataGridItemAutomationPeer::IsSelected() const
    {
        auto const& selectedItems = OwningDataGrid().SelectedItems();
        return std::ranges::find(selectedItems, item_) != selectedItems.end();
    }

    Automation::Provider::IRawElementProviderSimple DataGridItemAutomationPeer::SelectionContainer() const
    {
        return ProviderFromPeer(dataGridAutomationPeer_);
    }

    void DataGridItemAutomationPeer::AddToSelection() const
    {
        EnsureEnabled();

        auto const internalOwningDataGrid = OwningDataGrid().as<DataGrid>();
        if (internalOwningDataGrid->SelectionMode() == DataGridSelectionMode::Single)
        {
            if(auto const& selectedItems = internalOwningDataGrid->SelectedItems();
                selectedItems.Size() > 0 &&
                std::ranges::find(selectedItems, item_) == selectedItems.end())
            {
                throw DataGridError::DataGridAutomationPeer::OperationCannotBePerformed();
            }
        }

        if (auto const index = internalOwningDataGrid->DataConnection().IndexOf(item_);
            index != DataGridInternal::DataGridDataConnection::InvalidIndex)
        {
            internalOwningDataGrid->SetRowSelection(internalOwningDataGrid->SlotFromRowIndex(index), true, false);
            return;
        }

        throw DataGridError::DataGridAutomationPeer::OperationCannotBePerformed();
    }

    void DataGridItemAutomationPeer::RemoveFromSelection()
    {
        EnsureEnabled();

        auto const internalOwningDataGrid = OwningDataGrid().as<DataGrid>();
        if (auto const index = internalOwningDataGrid->DataConnection().IndexOf(item_);
            index != DataGridInternal::DataGridDataConnection::InvalidIndex)
        {
            bool success = true;
            if (auto const editingRow = internalOwningDataGrid->EditingRow();
                editingRow && editingRow.Index() == index)
            {
                if (internalOwningDataGrid->WaitForLostFocus([weakPtr = get_weak()]
                    {
                        if(auto const instance = weakPtr.get())
                        {
                            instance->RemoveFromSelection();
                        }
                    }))
                {
                    return;
                }

                success = internalOwningDataGrid->CommitEdit(DataGridEditingUnit::Row, true /*exitEditing*/);
            }

            if (success)
            {
                internalOwningDataGrid->SetRowSelection(internalOwningDataGrid->SlotFromRowIndex(index), false, false);
                return;
            }

            throw DataGridError::DataGridAutomationPeer::OperationCannotBePerformed();
        }
    }

    void DataGridItemAutomationPeer::Select()
    {
        EnsureEnabled();

        auto const internalOwningDataGrid = OwningDataGrid().as<DataGrid>();
        if (auto const index = internalOwningDataGrid->DataConnection().IndexOf(item_);
            index != DataGridInternal::DataGridDataConnection::InvalidIndex)
        {
            bool success = true;
            if (auto const editingRow = internalOwningDataGrid->EditingRow();
                editingRow && editingRow.Index() != index)
            {
                if (internalOwningDataGrid->WaitForLostFocus([weakPtr = get_weak()]
                    {
                        if(auto const instance = weakPtr.get())
                        {
                            instance->Select();
                        }
                    }))
                {
                    return;
                }

                success = internalOwningDataGrid->CommitEdit(DataGridEditingUnit::Row, true /*exitEditing*/);
            }

            if (success)
            {
                // Clear all the other selected items and select this one
                auto const slot = internalOwningDataGrid->SlotFromRowIndex(index);
                internalOwningDataGrid->UpdateSelectionAndCurrency(internalOwningDataGrid->CurrentColumnIndex(), slot, DataGridInternal::DataGridSelectionAction::SelectCurrent, false);
                return;
            }

            throw DataGridError::DataGridAutomationPeer::OperationCannotBePerformed();
        }
    }

    com_array<Automation::Provider::IRawElementProviderSimple> DataGridItemAutomationPeer::GetSelection() const
    {
        auto const internalOwningDataGrid = OwningDataGrid().as<DataGrid>();
        if (auto const internalOwningRow = OwningRow().try_as<DataGridRow>();
            internalOwningRow &&
            internalOwningDataGrid->IsSlotVisible(internalOwningRow->Slot()) &&
            internalOwningDataGrid->CurrentSlot() == internalOwningRow->Slot())
        {
            auto const cell = internalOwningRow->Cells()[internalOwningDataGrid->CurrentColumnIndex()];
            if (auto const peer = Automation::Peers::FrameworkElementAutomationPeer::CreatePeerForElement(cell);
                peer)
            {
                return { ProviderFromPeer(peer) };
            }
        }

        return {};
    }

    // ReSharper disable once CppMemberFunctionMayBeStatic
    bool DataGridItemAutomationPeer::CanSelectMultiple() const
    {
        return false;
    }

    // ReSharper disable once CppMemberFunctionMayBeStatic
    bool DataGridItemAutomationPeer::IsSelectionRequired() const
    {
        return false;
    }

    void DataGridItemAutomationPeer::EnsureEnabled() const
    {
        if (!dataGridAutomationPeer_.IsEnabled())
        {
            throw DataGridError::DataGridAutomationPeer::ElementNotEnabled();
        }
    }

    MiniDumpExplorer::DataGrid DataGridItemAutomationPeer::OwningDataGrid() const
    {
        auto const gridPeer = dataGridAutomationPeer_.as<MiniDumpExplorer::DataGridAutomationPeer>();
        return gridPeer.Owner().as<MiniDumpExplorer::DataGrid>();
    }

    MiniDumpExplorer::DataGridRow DataGridItemAutomationPeer::OwningRow() const
    {
        auto const internalOwningDataGrid = OwningDataGrid().as<DataGrid>();
        auto const index = internalOwningDataGrid->DataConnection().IndexOf(item_);

        if (auto const slot = internalOwningDataGrid->SlotFromRowIndex(index);
            internalOwningDataGrid->IsSlotVisible(slot))
        {
            return internalOwningDataGrid->DisplayData().GetDisplayedElement(slot).as<MiniDumpExplorer::DataGridRow>();
        }

        return nullptr;
    }
}
