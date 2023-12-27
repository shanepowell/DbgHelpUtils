#include "pch.h"
#include "DataGridCellAutomationPeer.h"

#include "DataGrid.h"
#include "DataGridBoundColumn.h"
#include "DataGridCell.h"
#include "DataGridColumn.h"
#include "DataGridColumnCollection.h"
#include "DataGridError.h"
#include "DataGridFillerColumn.h"
#include "DataGridRow.h"
#include "DataGridSelectionAction.h"

#if __has_include("DataGridCellAutomationPeer.g.cpp")
// ReSharper disable once CppUnusedIncludeDirective
#include "DataGridCellAutomationPeer.g.cpp"  // NOLINT(bugprone-suspicious-include)
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    auto DataGridCellAutomationPeer::InternalOwningCell() const
    {
        return OwningCell().as<DataGridCell>();
    }

    auto DataGridCellAutomationPeer::InternalOwningColumn() const
    {
        return OwningColumn().as<DataGridColumn>();
    }

    auto DataGridCellAutomationPeer::TryInternalOwningColumn() const
    {
        return OwningColumn().try_as<DataGridColumn>();
    }

    auto DataGridCellAutomationPeer::TryInternalOwningGrid() const
    {
        return OwningGrid().try_as<DataGrid>();
    }

    auto DataGridCellAutomationPeer::InternalOwningRow() const
    {
        return OwningRow().as<DataGridRow>();
    }

    auto DataGridCellAutomationPeer::TryInternalOwningRow() const
    {
        return OwningRow().try_as<DataGridRow>();
    }

    DataGridCellAutomationPeer::DataGridCellAutomationPeer(MiniDumpExplorer::DataGridCell const& owner)
        : base_type(owner)
    {
    }

    Automation::Provider::IRawElementProviderSimple DataGridCellAutomationPeer::ContainingGrid() const
    {
        if (auto const peer = Automation::Peers::FrameworkElementAutomationPeer::CreatePeerForElement(OwningGrid());
            peer)
        {
            return ProviderFromPeer(peer);
        }

        return nullptr;
    }

    Automation::Peers::AutomationControlType DataGridCellAutomationPeer::GetAutomationControlTypeCore() const
    {
        if (auto const owningColumn = OwningColumn(); owningColumn)
        {
            if (owningColumn.try_as<DataGridCheckBoxColumn>())
            {
                return Automation::Peers::AutomationControlType::CheckBox;
            }

            if (owningColumn.try_as<DataGridTextColumn>())
            {
                return Automation::Peers::AutomationControlType::Text;
            }

            if (owningColumn.try_as<DataGridComboBoxColumn>())
            {
                return Automation::Peers::AutomationControlType::ComboBox;
            }
        }

        return Automation::Peers::AutomationControlType::Custom;
    }

    hstring DataGridCellAutomationPeer::GetClassNameCore() const
    {
        return get_class_name(Owner());
    }

    hstring DataGridCellAutomationPeer::GetNameCore() const
    {
        if (auto const textBlock = OwningCell().Content().as<Controls::TextBlock>();
            textBlock)
        {
            return textBlock.Text();
        }

        if (auto const textBox = OwningCell().Content().try_as<Controls::TextBox>();
            textBox)
        {
            return textBox.Text();
        }

        auto const owningColumn = TryInternalOwningColumn();
        if (auto const owningRow = OwningRow();
            owningColumn && owningRow)
        {
            Windows::Foundation::IPropertyValue cellContent{nullptr};
            if (auto const boundColumn = owningColumn.try_as<DataGridBoundColumn>();
                boundColumn && boundColumn->Binding())
            {
                cellContent = boundColumn->GetCellValue(owningRow.DataContext(), boundColumn->Binding()).try_as<Windows::Foundation::IPropertyValue>();
            }

            if (!cellContent && owningColumn->ClipboardContentBinding())
            {
                cellContent = owningColumn->GetCellValue(owningRow.DataContext(), owningColumn->ClipboardContentBinding()).try_as<Windows::Foundation::IPropertyValue>();
            }
            
            if (cellContent)
            {
                if (auto const cellName = cellContent.GetString();
                    !cellName.empty())
                {
                    return cellName;
                }
            }
        }

        return base_type::GetNameCore();
    }

    Windows::Foundation::IInspectable DataGridCellAutomationPeer::GetPatternCore(Automation::Peers::PatternInterface const patternInterface) const
    {
        if (auto const owningGird = TryInternalOwningGrid(); owningGird)
        {
            switch (patternInterface)
            {
            case Automation::Peers::PatternInterface::Invoke:
            {
                if (auto const owningColumn = OwningColumn();
                    !owningGird->IsReadOnly() &&
                    owningColumn &&
                    !owningColumn.IsReadOnly())
                {
                    return *this;
                }

                break;
            }

            case Automation::Peers::PatternInterface::ScrollItem:
            {
                if (owningGird->HorizontalScrollBar() &&
                    owningGird->HorizontalScrollBar().Maximum() > 0)
                {
                    return *this;
                }

                break;
            }

            case Automation::Peers::PatternInterface::GridItem:
            case Automation::Peers::PatternInterface::SelectionItem:
            case Automation::Peers::PatternInterface::TableItem:
                return *this;

            default: 
                break;
            }
        }

        return base_type::GetPatternCore(patternInterface);
    }

    int32_t DataGridCellAutomationPeer::Column() const
    {
        auto column = InternalOwningCell()->ColumnIndex();
        if (auto const owningGrid = TryInternalOwningGrid();
            column != DataGridCell::InvalidIndex && owningGrid && owningGrid->ColumnsInternal().RowGroupSpacerColumn().as<DataGridFillerColumn>()->IsRepresented())
        {
            column--;
        }

        return static_cast<int32_t>(column);
    }

    int32_t DataGridCellAutomationPeer::Row() const
    {
        return static_cast<int32_t>(InternalOwningCell()->RowIndex());
    }

    void DataGridCellAutomationPeer::Invoke()
    {
        EnsureEnabled();

        if (auto const owningGrid = TryInternalOwningGrid();
            owningGrid)
        {
            if (owningGrid->WaitForLostFocus([this]() { Invoke(); }))
            {
                return;
            }

            if (auto const owningCell = InternalOwningCell();
                owningGrid->EditingRow() == OwningRow() && owningGrid->EditingColumnIndex() == owningCell->ColumnIndex())
            {
                owningGrid->CommitEdit(DataGridEditingUnit::Cell, true /*exitEditingMode*/);
            }
            else if (owningGrid->UpdateSelectionAndCurrency(owningCell->ColumnIndex(), InternalOwningRow()->Slot(), DataGridInternal::DataGridSelectionAction::SelectCurrent, true))
            {
                owningGrid->BeginEdit();
            }
        }
    }

    void DataGridCellAutomationPeer::ScrollIntoView() const
    {
        if (auto const owningGrid = OwningGrid();
            owningGrid)
        {
            owningGrid.ScrollIntoView(OwningCell().DataContext(), OwningColumn());
        }
        else
        {
            throw DataGridError::DataGridAutomationPeer::OperationCannotBePerformed();
        }
    }

    bool DataGridCellAutomationPeer::IsSelected() const
    {
        if (auto const owningRow = TryInternalOwningRow();
            owningRow && OwningGrid())
        {
            return owningRow->IsSelected();
        }

        throw DataGridError::DataGridAutomationPeer::OperationCannotBePerformed();
    }

    Automation::Provider::IRawElementProviderSimple DataGridCellAutomationPeer::SelectionContainer() const
    {
        if (auto const peer = Automation::Peers::FrameworkElementAutomationPeer::CreatePeerForElement(OwningRow());
            peer)
        {
            return ProviderFromPeer(peer);
        }

        return nullptr;
    }

    void DataGridCellAutomationPeer::AddToSelection() const
    {
        EnsureEnabled();
        auto const owningCell = InternalOwningCell();
        if(!owningCell)
        {
            throw DataGridError::DataGridAutomationPeer::OperationCannotBePerformed();
        }
        
        if (auto const owningGrid = owningCell->OwningGrid().as<DataGrid>();
            !owningGrid ||
            owningGrid->CurrentSlot() != owningCell->RowIndex() ||
            owningGrid->CurrentColumnIndex() != owningCell->ColumnIndex())
        {
            throw DataGridError::DataGridAutomationPeer::OperationCannotBePerformed();
        }
    }

    void DataGridCellAutomationPeer::RemoveFromSelection() const
    {
        EnsureEnabled();
        auto const owningCell = InternalOwningCell();
        if (!owningCell)
        {
            throw DataGridError::DataGridAutomationPeer::OperationCannotBePerformed();
        }
        if (auto const owningGrid = owningCell->OwningGrid().as<DataGrid>();
            !owningGrid ||
            (owningGrid->CurrentSlot() == owningCell->RowIndex() &&
            owningGrid->CurrentColumnIndex() == owningCell->ColumnIndex()))
        {
            throw DataGridError::DataGridAutomationPeer::OperationCannotBePerformed();
        }
    }

    void DataGridCellAutomationPeer::Select()
    {
        EnsureEnabled();

        if (auto const owningGrid = TryInternalOwningGrid();
            owningGrid)
        {
            if (owningGrid->WaitForLostFocus([this]() { Select(); }))
            {
                return;
            }

            owningGrid->UpdateSelectionAndCurrency(InternalOwningCell()->ColumnIndex(), InternalOwningRow()->Slot(), DataGridInternal::DataGridSelectionAction::SelectCurrent, false);
        }
    }

    com_array<Automation::Provider::IRawElementProviderSimple> DataGridCellAutomationPeer::GetColumnHeaderItems() const
    {
        if (auto const owningGrid = TryInternalOwningGrid();
            !owningGrid || !owningGrid->AreColumnHeadersVisible())
        {
            return {nullptr};
        }

        auto const headerCell = InternalOwningColumn()->HeaderCell();
        if(!headerCell)
        {
            return {nullptr};
        }

        if (auto const peer = Automation::Peers::FrameworkElementAutomationPeer::CreatePeerForElement(headerCell);
            peer)
        {
            return com_array(1, ProviderFromPeer(peer));
        }

        return {nullptr};
    }

    com_array<Automation::Provider::IRawElementProviderSimple> DataGridCellAutomationPeer::GetRowHeaderItems() const
    {
        if (auto const owningGrid = TryInternalOwningGrid();
            !owningGrid || !owningGrid->AreRowHeadersVisible())
        {
            return {nullptr};
        }

        auto const headerCell = InternalOwningRow()->HeaderCell();
        if(!headerCell)
        {
            return {nullptr};
        }

        if (auto const peer = Automation::Peers::FrameworkElementAutomationPeer::CreatePeerForElement(headerCell);
            peer)
        {
            return com_array(1, ProviderFromPeer(peer));
        }

        return {nullptr};
    }

    MiniDumpExplorer::DataGridColumn DataGridCellAutomationPeer::OwningColumn() const
    {
        return InternalOwningCell()->OwningColumn();
    }

    MiniDumpExplorer::DataGrid DataGridCellAutomationPeer::OwningGrid() const
    {
        return InternalOwningCell()->OwningGrid();
    }

    MiniDumpExplorer::DataGridRow DataGridCellAutomationPeer::OwningRow() const
    {
        return InternalOwningCell()->OwningRow();
    }

    void DataGridCellAutomationPeer::EnsureEnabled() const
    {
        if (!IsEnabled())
        {
            throw DataGridError::DataGridAutomationPeer::ElementNotEnabled();
        }
    }
}
