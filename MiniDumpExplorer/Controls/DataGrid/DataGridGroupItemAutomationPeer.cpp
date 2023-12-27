#include "pch.h"
#include "DataGridGroupItemAutomationPeer.h"

#include "DataGrid.h"
#include "DataGridAutomationPeer.h"
#include "DataGridDataConnection.h"
#include "DataGridDisplayData.h"
#include "DataGridRow.h"
#include "DbgHelpUtils/wide_runtime_error.h"

#if __has_include("DataGridGroupItemAutomationPeer.g.cpp")
// ReSharper disable once CppUnusedIncludeDirective
#include "DataGridGroupItemAutomationPeer.g.cpp"  // NOLINT(bugprone-suspicious-include)
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    DataGridGroupItemAutomationPeer::DataGridGroupItemAutomationPeer(Data::ICollectionViewGroup const& group, MiniDumpExplorer::DataGrid const& dataGrid)
        : base_type(dataGrid)
        , group_{group}
        , dataGridAutomationPeer_{Automation::Peers::FrameworkElementAutomationPeer::CreatePeerForElement(dataGrid)}
    {
        if(!group)
        {
            throw dlg_help_utils::exceptions::wide_runtime_error{L"invalid argument: group"};
        }

        if(!dataGrid)
        {
            throw dlg_help_utils::exceptions::wide_runtime_error{L"invalid argument: dataGrid"};
        }
    }

    hstring DataGridGroupItemAutomationPeer::GetAcceleratorKeyCore() const
    {
        auto const owningRowGroupHeaderPeer = OwningRowGroupHeaderPeer();
        return owningRowGroupHeaderPeer ? owningRowGroupHeaderPeer.GetAcceleratorKey() : hstring{};
    }

    hstring DataGridGroupItemAutomationPeer::GetAccessKeyCore() const
    {
        auto const owningRowGroupHeaderPeer = OwningRowGroupHeaderPeer();
        return owningRowGroupHeaderPeer ? owningRowGroupHeaderPeer.GetAccessKey() : hstring{};
    }

    // ReSharper disable once CppMemberFunctionMayBeStatic
    Automation::Peers::AutomationControlType DataGridGroupItemAutomationPeer::GetAutomationControlTypeCore() const
    {
        return Automation::Peers::AutomationControlType::Group;
    }

    // ReSharper disable once CppMemberFunctionMayBeStatic
    hstring DataGridGroupItemAutomationPeer::GetAutomationIdCore() const
    {
        return {};
    }

    Windows::Foundation::Rect DataGridGroupItemAutomationPeer::GetBoundingRectangleCore() const
    {
        auto const owningRowGroupHeaderPeer = OwningRowGroupHeaderPeer();
        return owningRowGroupHeaderPeer ? owningRowGroupHeaderPeer.GetBoundingRectangle() : Windows::Foundation::Rect{};
    }

    Windows::Foundation::Collections::IVector<Automation::Peers::AutomationPeer> DataGridGroupItemAutomationPeer::GetChildrenCore() const
    {
        Windows::Foundation::Collections::IVector children{single_threaded_vector<Automation::Peers::AutomationPeer>()};
        if (auto const owningRowGroupHeaderPeer = OwningRowGroupHeaderPeer();
            owningRowGroupHeaderPeer)
        {
            owningRowGroupHeaderPeer.InvalidatePeer();
            for (auto const& child : owningRowGroupHeaderPeer.GetChildren())
            {
                children.Append(child);
            }
        }

        auto const owningDataGridPeer = OwningDataGridPeer().as<DataGridAutomationPeer>();
        for (auto const& item : group_.GroupItems() /*Items*/)
        {
            children.Append(owningDataGridPeer->GetOrCreateItemPeer(item));
        }
        return children;
    }

    hstring DataGridGroupItemAutomationPeer::GetClassNameCore() const
    {
        auto const owningRowGroupHeaderPeer = OwningRowGroupHeaderPeer();
        return owningRowGroupHeaderPeer ? owningRowGroupHeaderPeer.GetClassName() : hstring{};
    }

    Windows::Foundation::Point DataGridGroupItemAutomationPeer::GetClickablePointCore() const
    {
        auto const owningRowGroupHeaderPeer = OwningRowGroupHeaderPeer();
        return owningRowGroupHeaderPeer ? owningRowGroupHeaderPeer.GetClickablePoint() : Windows::Foundation::Point{std::nanf(""), std::nanf("")};
    }

    hstring DataGridGroupItemAutomationPeer::GetHelpTextCore() const
    {
        auto const owningRowGroupHeaderPeer = OwningRowGroupHeaderPeer();
        return owningRowGroupHeaderPeer ? owningRowGroupHeaderPeer.GetHelpText() : hstring{};
    }

    hstring DataGridGroupItemAutomationPeer::GetItemStatusCore() const
    {
        auto const owningRowGroupHeaderPeer = OwningRowGroupHeaderPeer();
        return owningRowGroupHeaderPeer ? owningRowGroupHeaderPeer.GetItemStatus() : hstring{};
    }

    hstring DataGridGroupItemAutomationPeer::GetItemTypeCore() const
    {
        auto const owningRowGroupHeaderPeer = OwningRowGroupHeaderPeer();
        return owningRowGroupHeaderPeer ? owningRowGroupHeaderPeer.GetItemType() : hstring{};
    }

    Automation::Peers::AutomationPeer DataGridGroupItemAutomationPeer::GetLabeledByCore() const
    {
        auto const owningRowGroupHeaderPeer = OwningRowGroupHeaderPeer();
        return owningRowGroupHeaderPeer ? owningRowGroupHeaderPeer.GetLabeledBy() : nullptr;
    }

    hstring DataGridGroupItemAutomationPeer::GetLocalizedControlTypeCore() const
    {
        auto const owningRowGroupHeaderPeer = OwningRowGroupHeaderPeer();
        return owningRowGroupHeaderPeer ? owningRowGroupHeaderPeer.GetLocalizedControlType() : hstring{};
    }

    hstring DataGridGroupItemAutomationPeer::GetNameCore() const
    {
        return base_type::GetNameCore();
    }

    Automation::Peers::AutomationOrientation DataGridGroupItemAutomationPeer::GetOrientationCore() const
    {
        auto const owningRowGroupHeaderPeer = OwningRowGroupHeaderPeer();
        return owningRowGroupHeaderPeer ? owningRowGroupHeaderPeer.GetOrientation() : Automation::Peers::AutomationOrientation::None;
    }

    Windows::Foundation::IInspectable DataGridGroupItemAutomationPeer::GetPatternCore(Automation::Peers::PatternInterface const patternInterface) const
    {
        switch (patternInterface)
        {
        case Automation::Peers::PatternInterface::ExpandCollapse:
        case Automation::Peers::PatternInterface::Grid:
        case Automation::Peers::PatternInterface::Selection:
        case Automation::Peers::PatternInterface::Table:
            return *this;
        case Automation::Peers::PatternInterface::ScrollItem:
        {
            if (auto const internalOwningDataGrid = OwningDataGrid().try_as<DataGrid>();
                internalOwningDataGrid->VerticalScrollBar() &&
                internalOwningDataGrid->VerticalScrollBar().Maximum() > 0)
            {
                return *this;
            }
            break;
        }
        default:
            break;
        }

        return base_type::GetPatternCore(patternInterface);
    }

    bool DataGridGroupItemAutomationPeer::HasKeyboardFocusCore() const
    {
        auto const owningRowGroupHeaderPeer = OwningRowGroupHeaderPeer();
        return owningRowGroupHeaderPeer ? owningRowGroupHeaderPeer.HasKeyboardFocus() : false;
    }

    bool DataGridGroupItemAutomationPeer::IsContentElementCore() const
    {
        auto const owningRowGroupHeaderPeer = OwningRowGroupHeaderPeer();
        return owningRowGroupHeaderPeer ? owningRowGroupHeaderPeer.IsContentElement() : true;
    }

    bool DataGridGroupItemAutomationPeer::IsControlElementCore() const
    {
        auto const owningRowGroupHeaderPeer = OwningRowGroupHeaderPeer();
        return owningRowGroupHeaderPeer ? owningRowGroupHeaderPeer.IsControlElement() : true;
    }

    bool DataGridGroupItemAutomationPeer::IsEnabledCore() const
    {
        auto const owningRowGroupHeaderPeer = OwningRowGroupHeaderPeer();
        return owningRowGroupHeaderPeer ? owningRowGroupHeaderPeer.IsEnabled() : false;
    }

    bool DataGridGroupItemAutomationPeer::IsKeyboardFocusableCore() const
    {
        auto const owningRowGroupHeaderPeer = OwningRowGroupHeaderPeer();
        return owningRowGroupHeaderPeer ? owningRowGroupHeaderPeer.IsKeyboardFocusable() : false;
    }

    bool DataGridGroupItemAutomationPeer::IsOffscreenCore() const
    {
        auto const owningRowGroupHeaderPeer = OwningRowGroupHeaderPeer();
        return owningRowGroupHeaderPeer ? owningRowGroupHeaderPeer.IsOffscreen() : true;
    }

    bool DataGridGroupItemAutomationPeer::IsPasswordCore() const
    {
        auto const owningRowGroupHeaderPeer = OwningRowGroupHeaderPeer();
        return owningRowGroupHeaderPeer ? owningRowGroupHeaderPeer.IsPassword() : false;
    }

    bool DataGridGroupItemAutomationPeer::IsRequiredForFormCore() const
    {
        auto const owningRowGroupHeaderPeer = OwningRowGroupHeaderPeer();
        return owningRowGroupHeaderPeer ? owningRowGroupHeaderPeer.IsRequiredForForm() : false;
    }

    void DataGridGroupItemAutomationPeer::SetFocusCore() const
    {
        if (auto const owningRowGroupHeaderPeer = OwningRowGroupHeaderPeer();
            owningRowGroupHeaderPeer)
        {
            owningRowGroupHeaderPeer.SetFocus();
        }
    }

    void DataGridGroupItemAutomationPeer::Collapse() const
    {
        EnsureEnabled();

        if (auto const owningDataGrid = OwningDataGrid();
            owningDataGrid)
        {
            owningDataGrid.CollapseRowGroup(group_, false /*collapseAllSubgroups*/);
        }
    }

    void DataGridGroupItemAutomationPeer::Expand() const
    {
        EnsureEnabled();

        if (auto const owningDataGrid = OwningDataGrid();
            owningDataGrid)
        {
            owningDataGrid.ExpandRowGroup(group_, false /*expandAllSubgroups*/);
        }
    }

    Automation::ExpandCollapseState DataGridGroupItemAutomationPeer::ExpandCollapseState() const
    {
        if (auto const internalOwningDataGrid = OwningDataGrid().try_as<DataGrid>();
            internalOwningDataGrid)
        {
            if (auto const groupInfo = internalOwningDataGrid->RowGroupInfoFromCollectionViewGroup(group_);
                groupInfo && groupInfo->Visibility() == Visibility::Visible)
            {
                return Automation::ExpandCollapseState::Expanded;
            }
        }

        return Automation::ExpandCollapseState::Collapsed;
    }

    int32_t DataGridGroupItemAutomationPeer::ColumnCount() const
    {
        if (auto const internalOwningDataGrid = OwningDataGrid().try_as<DataGrid>();
            internalOwningDataGrid)
        {
            return internalOwningDataGrid->Columns().Size();
        }
        return 0;
    }

    int32_t DataGridGroupItemAutomationPeer::RowCount() const
    {
        return group_.GroupItems().Size() /*ItemCount*/;
    }

    Automation::Provider::IRawElementProviderSimple DataGridGroupItemAutomationPeer::GetItem(int32_t row, int32_t const column) const
    {
        EnsureEnabled();

        if (auto const internalOwningDataGrid = OwningDataGrid().try_as<DataGrid>();
            internalOwningDataGrid &&
            row >= 0 && row < static_cast<int32_t>(group_.GroupItems().Size() /*ItemCount*/) /*ItemCount*/ &&
            column >= 0 && column < static_cast<int32_t>(internalOwningDataGrid->Columns().Size()))
        {
            if (auto const groupInfo = internalOwningDataGrid->RowGroupInfoFromCollectionViewGroup(group_);
                groupInfo)
            {
                // Adjust the row index to be relative to the DataGrid instead of the group
                row = static_cast<int32_t>(internalOwningDataGrid->RowGroupHeadersTable().GetIndexCount(0, groupInfo->Slot())) + row + 1;
                groupInfo->Slot(row);
                assert(row >= 0);
                assert(row < internalOwningDataGrid->DataConnection().Count());
                auto const slot = internalOwningDataGrid->SlotFromRowIndex(row);

                if (!internalOwningDataGrid->IsSlotVisible(slot))
                {
                    auto const item = internalOwningDataGrid->DataConnection().GetDataItem(row);
                    internalOwningDataGrid->ScrollIntoView(item, internalOwningDataGrid->Columns().GetAt(column));
                }

                assert(internalOwningDataGrid->IsSlotVisible(slot));

                auto const dgr = internalOwningDataGrid->DisplayData().GetDisplayedElement(slot).try_as<DataGridRow>();

                // the first cell is always the indentation filler cell if grouping is enabled, so skip it
                assert(column + 1 < dgr->Cells().Count());
                auto const cell = dgr->Cells()[column + 1];
                if (auto const peer = Automation::Peers::FrameworkElementAutomationPeer::CreatePeerForElement(cell);
                    peer)
                {
                    return ProviderFromPeer(peer);
                }
            }
        }

        return nullptr;
    }

    void DataGridGroupItemAutomationPeer::ScrollIntoView() const
    {
        EnsureEnabled();

        if (auto const internalOwningDataGrid = OwningDataGrid().try_as<DataGrid>();
            internalOwningDataGrid)
        {
            if (auto const groupInfo = internalOwningDataGrid->RowGroupInfoFromCollectionViewGroup(group_);
                groupInfo)
            {
                internalOwningDataGrid->ScrollIntoView(groupInfo->CollectionViewGroup(), nullptr);
            }
        }
    }

    com_array<Automation::Provider::IRawElementProviderSimple> DataGridGroupItemAutomationPeer::GetSelection() const
    {
        EnsureEnabled();

        if (auto const internalOwningDataGrid = OwningDataGrid().try_as<DataGrid>();
            internalOwningDataGrid &&
            OwningDataGridPeer() &&
            internalOwningDataGrid->SelectedItems() &&
            group_.GroupItems().Size() /*ItemCount*/ > 0)
        {
            if (auto const groupInfo = internalOwningDataGrid->RowGroupInfoFromCollectionViewGroup(group_);
                groupInfo)
            {
                auto const owningDataGridPeer = OwningDataGridPeer();
                // See which of the selected items are contained within this group
                std::vector<Automation::Provider::IRawElementProviderSimple> selectedProviders{};
                for (auto const startRowIndex = groupInfo->Slot() - internalOwningDataGrid->RowGroupHeadersTable().GetIndexCount(0, groupInfo->Slot()) + 1;
                    auto const& item : internalOwningDataGrid->GetSelectionInclusive(startRowIndex, startRowIndex + group_.GroupItems().Size() /*ItemCount*/ - 1))
                {
                    if (auto const peer = owningDataGridPeer.as<DataGridAutomationPeer>()->GetOrCreateItemPeer(item);
                        peer)
                    {
                        selectedProviders.push_back(ProviderFromPeer(peer));
                    }
                }

                return {selectedProviders.begin(), selectedProviders.end()};
            }
        }

        return {nullptr};
    }

    bool DataGridGroupItemAutomationPeer::CanSelectMultiple() const
    {
        auto const internalOwningDataGrid = OwningDataGrid().try_as<DataGrid>();
        return internalOwningDataGrid && internalOwningDataGrid->SelectionMode() == DataGridSelectionMode::Extended;
    }

    // ReSharper disable once CppMemberFunctionMayBeStatic
    bool DataGridGroupItemAutomationPeer::IsSelectionRequired() const
    {
        return false;
    }

    MiniDumpExplorer::DataGridRowGroupHeaderAutomationPeer DataGridGroupItemAutomationPeer::OwningRowGroupHeaderPeer() const
    {
        MiniDumpExplorer::DataGridRowGroupHeaderAutomationPeer rowGroupHeaderPeer{nullptr};
        if (auto const rowGroupHeader = OwningRowGroupHeader();
            rowGroupHeader)
        {
            rowGroupHeaderPeer = Automation::Peers::FrameworkElementAutomationPeer::FromElement(rowGroupHeader).try_as<MiniDumpExplorer::DataGridRowGroupHeaderAutomationPeer>();
            if (!rowGroupHeaderPeer)
            {
                rowGroupHeaderPeer = Automation::Peers::FrameworkElementAutomationPeer::CreatePeerForElement(rowGroupHeader).as<MiniDumpExplorer::DataGridRowGroupHeaderAutomationPeer>();
            }
        }

        return rowGroupHeaderPeer;
    }

    MiniDumpExplorer::DataGrid DataGridGroupItemAutomationPeer::OwningDataGrid() const
    {
        auto const gridPeer = dataGridAutomationPeer_.as<MiniDumpExplorer::DataGridAutomationPeer>();
        return gridPeer.Owner().try_as<MiniDumpExplorer::DataGrid>();
    }

    MiniDumpExplorer::DataGridAutomationPeer DataGridGroupItemAutomationPeer::OwningDataGridPeer() const
    {
        return dataGridAutomationPeer_.as<MiniDumpExplorer::DataGridAutomationPeer>();
    }

    MiniDumpExplorer::DataGridRowGroupHeader DataGridGroupItemAutomationPeer::OwningRowGroupHeader() const
    {
        if (auto const internalOwningDataGrid = OwningDataGrid().try_as<DataGrid>();
            internalOwningDataGrid)
        {
            if (auto const groupInfo = internalOwningDataGrid->RowGroupInfoFromCollectionViewGroup(group_);
                groupInfo && internalOwningDataGrid->IsSlotVisible(groupInfo->Slot()))
            {
                return internalOwningDataGrid->DisplayData().GetDisplayedElement(groupInfo->Slot()).try_as<MiniDumpExplorer::DataGridRowGroupHeader>();
            }
        }

        return nullptr;
    }

    void DataGridGroupItemAutomationPeer::EnsureEnabled() const
    {
        if (!dataGridAutomationPeer_.IsEnabled())
        {
            throw dlg_help_utils::exceptions::wide_runtime_error{L"ElementNotEnabledException"};
        }
    }
}
