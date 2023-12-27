#include "pch.h"
#include "DataGridAutomationPeer.h"

#include <ranges>

#include "DataGrid.h"
#include "DataGridColumnCollection.h"
#include "DataGridDataConnection.h"
#include "DataGridDisplayData.h"
#include "DataGridError.h"
#include "DataGridFillerColumn.h"
#include "DataGridGroupItemAutomationPeer.h"
#include "DataGridItemAutomationPeer.h"
#include "DataGridRow.h"
#include "DataGridRowGroupHeader.h"
#include "DbgHelpUtils/double_utils.h"

#if __has_include("DataGridAutomationPeer.g.cpp")
// ReSharper disable once CppUnusedIncludeDirective
#include "DataGridAutomationPeer.g.cpp"  // NOLINT(bugprone-suspicious-include)
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    DataGridAutomationPeer::DataGridAutomationPeer(MiniDumpExplorer::DataGrid const& owner)
        : base_type(owner)
    {
        if (HorizontallyScrollable())
        {
            oldHorizontallyScrollable_ = true;
            oldHorizontalScrollPercent_ = HorizontalScrollPercent();
            oldHorizontalViewSize_ = HorizontalViewSize();
        }
        else
        {
            oldHorizontallyScrollable_ = false;
            oldHorizontalScrollPercent_ = Automation::ScrollPatternIdentifiers::NoScroll();
            oldHorizontalViewSize_ = 100.0;
        }

        if (VerticallyScrollable())
        {
            oldVerticallyScrollable_ = true;
            oldVerticalScrollPercent_ = VerticalScrollPercent();
            oldVerticalViewSize_ = VerticalViewSize();
        }
        else
        {
            oldVerticallyScrollable_ = false;
            oldVerticalScrollPercent_ = Automation::ScrollPatternIdentifiers::NoScroll();
            oldVerticalViewSize_ = 100.0;
        }
    }

    // ReSharper disable once CppMemberFunctionMayBeStatic
    Automation::Peers::AutomationControlType DataGridAutomationPeer::GetAutomationControlTypeCore() const
    {
        return Automation::Peers::AutomationControlType::DataGrid;
    }

    Windows::Foundation::Collections::IVector<Automation::Peers::AutomationPeer> DataGridAutomationPeer::GetChildrenCore() const
    {
        auto children = base_type::GetChildrenCore();
        if (auto const internalOwningDataGrid = OwningDataGrid().as<DataGrid>();
            internalOwningDataGrid)
        {
            uint32_t index;
            if(children.IndexOf(Automation::Peers::FrameworkElementAutomationPeer::FromElement(internalOwningDataGrid->HorizontalScrollBar()), index))
            {
                children.RemoveAt(index);
            }
            if(children.IndexOf(Automation::Peers::FrameworkElementAutomationPeer::FromElement(internalOwningDataGrid->VerticalScrollBar()), index))
            {
                children.RemoveAt(index);
            }
        }

        return children;
    }

    hstring DataGridAutomationPeer::GetClassNameCore() const
    {
        return get_class_name(Owner());
    }

    hstring DataGridAutomationPeer::GetNameCore() const
    {
        auto name = base_type::GetNameCore();
        if (name.empty())
        {
            
            if (auto const owningDataGird = OwningDataGrid();
                owningDataGird)
            {
                name = owningDataGird.Name();
            }

            if (name.empty())
            {
                name = GetClassName();
            }
        }

        return name;
    }

    Windows::Foundation::IInspectable DataGridAutomationPeer::GetPatternCore(Automation::Peers::PatternInterface const patternInterface) const
    {
        switch (patternInterface)
        {
        case Automation::Peers::PatternInterface::Grid:
        case Automation::Peers::PatternInterface::Selection:
        case Automation::Peers::PatternInterface::Table:
            return *this;

        case Automation::Peers::PatternInterface::Scroll:
            if (HorizontallyScrollable() || VerticallyScrollable())
            {
                return *this;
            }
            break;

        default:
            break;
        }

        return base_type::GetPatternCore(patternInterface);
    }

    int32_t DataGridAutomationPeer::ColumnCount() const
    {
        auto const internalOwningDataGrid = OwningDataGrid().as<DataGrid>();
        return internalOwningDataGrid->Columns().Size();
    }

    int32_t DataGridAutomationPeer::RowCount() const
    {
        auto const internalOwningDataGrid = OwningDataGrid().as<DataGrid>();
        return internalOwningDataGrid->DataConnection().Count();
    }

    Automation::Provider::IRawElementProviderSimple DataGridAutomationPeer::GetItem(int32_t const row, int32_t column) const
    {
        if (auto const internalOwningDataGrid = OwningDataGrid().try_as<DataGrid>();
            internalOwningDataGrid &&
            row >= 0 && row < internalOwningDataGrid->SlotCount() &&
            column >= 0 && column < static_cast<int32_t>(internalOwningDataGrid->Columns().Size()))
        {
            Windows::Foundation::IInspectable item{nullptr};
            if (!internalOwningDataGrid->IsSlotVisible(internalOwningDataGrid->SlotFromRowIndex(row)))
            {
                item = internalOwningDataGrid->DataConnection().GetDataItem(row);
            }

            internalOwningDataGrid->ScrollIntoView(item, internalOwningDataGrid->Columns().GetAt(column));

            auto const dgr = internalOwningDataGrid->DisplayData().GetDisplayedRow(row).as<DataGridRow>();
            if (internalOwningDataGrid->ColumnsInternal().RowGroupSpacerColumn().as<DataGridFillerColumn>()->IsRepresented())
            {
                column++;
            }

            assert(column >= 0);
            assert(column < internalOwningDataGrid->ColumnsItemsInternal().size());
            auto const cell = dgr->Cells()[column];
            
            if (auto const peer = Automation::Peers::FrameworkElementAutomationPeer::CreatePeerForElement(cell);
                peer)
            {
                return ProviderFromPeer(peer);
            }
        }

        return nullptr;
    }

    bool DataGridAutomationPeer::HorizontallyScrollable() const
    {
        auto const internalOwningDataGrid = OwningDataGrid().as<DataGrid>();
        auto const scrollBar = internalOwningDataGrid->HorizontalScrollBar();
        return scrollBar && scrollBar.Maximum() > 0;
    }

    double DataGridAutomationPeer::HorizontalScrollPercent() const
    {
        if (!HorizontallyScrollable())
        {
            return Automation::ScrollPatternIdentifiers::NoScroll();
        }

        auto const internalOwningDataGrid = OwningDataGrid().as<DataGrid>();
        auto const scrollBar = internalOwningDataGrid->HorizontalScrollBar();
        return scrollBar.Value() * 100.0 / scrollBar.Maximum();
    }

    double DataGridAutomationPeer::HorizontalViewSize() const
    {
        auto const internalOwningDataGrid = OwningDataGrid().as<DataGrid>();
        auto const scrollBar = internalOwningDataGrid->HorizontalScrollBar();
        if (!HorizontallyScrollable() || double_utils::is_zero(scrollBar.Maximum()))
        {
            return 100.0;
        }

        return scrollBar.ViewportSize() * 100.0 / (scrollBar.ViewportSize() + scrollBar.Maximum());
    }

    bool DataGridAutomationPeer::VerticallyScrollable() const
    {
        auto const internalOwningDataGrid = OwningDataGrid().as<DataGrid>();
        auto const scrollBar = internalOwningDataGrid->VerticalScrollBar();
        return scrollBar && scrollBar.Maximum() > 0;
    }

    double DataGridAutomationPeer::VerticalScrollPercent() const
    {
        if (!VerticallyScrollable())
        {
            return Automation::ScrollPatternIdentifiers::NoScroll();
        }

        auto const internalOwningDataGrid = OwningDataGrid().as<DataGrid>();
        auto const scrollBar = internalOwningDataGrid->VerticalScrollBar();
        return scrollBar.Value() * 100.0 / scrollBar.Maximum();
    }

    double DataGridAutomationPeer::VerticalViewSize() const
    {
        auto const internalOwningDataGrid = OwningDataGrid().as<DataGrid>();
        auto const scrollBar = internalOwningDataGrid->VerticalScrollBar();
        if (!VerticallyScrollable() || double_utils::is_zero(scrollBar.Maximum()))
        {
            return 100.0;
        }

        return scrollBar.ViewportSize() * 100.0 / (scrollBar.ViewportSize() + scrollBar.Maximum());
    }

    void DataGridAutomationPeer::Scroll(Automation::ScrollAmount const& horizontalAmount, Automation::ScrollAmount const& verticalAmount) const
    {
        if (!IsEnabled())
        {
            throw DataGridError::DataGridAutomationPeer::ElementNotEnabled();
        }

        auto const scrollHorizontally = horizontalAmount != Automation::ScrollAmount::NoAmount;
        if (auto const scrollVertically = verticalAmount != Automation::ScrollAmount::NoAmount;
            (scrollHorizontally && !HorizontallyScrollable()) || (scrollVertically && !VerticallyScrollable()))
        {
            throw DataGridError::DataGridAutomationPeer::OperationCannotBePerformed();
        }

        auto const internalOwningDataGrid = OwningDataGrid().as<DataGrid>();
        switch (horizontalAmount)
        {
            // In the small increment and decrement calls, ScrollHorizontally will adjust the
            // ScrollBar.Value itself, so we don't need to do it here
        case Automation::ScrollAmount::SmallIncrement:
            internalOwningDataGrid->ProcessHorizontalScroll(Controls::Primitives::ScrollEventType::SmallIncrement);
            break;
        case Automation::ScrollAmount::LargeIncrement:
        {
            auto const scrollBar = internalOwningDataGrid->HorizontalScrollBar();
            scrollBar.Value(scrollBar.Value() + scrollBar.LargeChange());
            internalOwningDataGrid->ProcessHorizontalScroll(Controls::Primitives::ScrollEventType::LargeIncrement);
            break;
        }
        case Automation::ScrollAmount::SmallDecrement:
            internalOwningDataGrid->ProcessHorizontalScroll(Controls::Primitives::ScrollEventType::SmallDecrement);
            break;
        case Automation::ScrollAmount::LargeDecrement:
        {
            auto const scrollBar = internalOwningDataGrid->HorizontalScrollBar();
            scrollBar.Value(scrollBar.Value() - scrollBar.LargeChange());
            internalOwningDataGrid->ProcessHorizontalScroll(Controls::Primitives::ScrollEventType::LargeDecrement);
            break;
        }
        case Automation::ScrollAmount::NoAmount:
            break;
        default:
            throw DataGridError::DataGridAutomationPeer::OperationCannotBePerformed();
        }

        switch (verticalAmount)
        {
            // In the small increment and decrement calls, ScrollVertically will adjust the
            // ScrollBar.Value itself, so we don't need to do it here
        case Automation::ScrollAmount::SmallIncrement:
            internalOwningDataGrid->ProcessVerticalScroll(Controls::Primitives::ScrollEventType::SmallIncrement);
            break;
        case Automation::ScrollAmount::LargeIncrement:
        {
            auto const scrollBar = internalOwningDataGrid->VerticalScrollBar();
            scrollBar.Value(scrollBar.Value() + scrollBar.LargeChange());
            internalOwningDataGrid->ProcessVerticalScroll(Controls::Primitives::ScrollEventType::LargeIncrement);
            break;
        }
        case Automation::ScrollAmount::SmallDecrement:
            internalOwningDataGrid->ProcessVerticalScroll(Controls::Primitives::ScrollEventType::SmallDecrement);
            break;
        case Automation::ScrollAmount::LargeDecrement:
        {
            auto const scrollBar = internalOwningDataGrid->VerticalScrollBar();
            scrollBar.Value(scrollBar.Value() - scrollBar.LargeChange());
            internalOwningDataGrid->ProcessVerticalScroll(Controls::Primitives::ScrollEventType::LargeDecrement);
            break;
        }
        case Automation::ScrollAmount::NoAmount:
            break;
        default:
            throw DataGridError::DataGridAutomationPeer::OperationCannotBePerformed();
        }
    }

    void DataGridAutomationPeer::SetScrollPercent(double const horizontalPercent, double const verticalPercent) const
    {
        if (!IsEnabled())
        {
            throw DataGridError::DataGridAutomationPeer::ElementNotEnabled();
        }

        auto const scrollHorizontally = horizontalPercent != static_cast<double>(Automation::ScrollAmount::NoAmount);
        auto const scrollVertically = verticalPercent != static_cast<double>(Automation::ScrollAmount::NoAmount);
        if ((scrollHorizontally && !HorizontallyScrollable()) || (scrollVertically && !VerticallyScrollable()))
        {
            throw DataGridError::DataGridAutomationPeer::OperationCannotBePerformed();
        }

        if (scrollHorizontally && (horizontalPercent < 0.0 || horizontalPercent > 100.0))
        {
            throw DataGridError::DataGridAutomationPeer::OperationCannotBePerformed();
        }

        if (scrollVertically && (verticalPercent < 0.0 || verticalPercent > 100.0))
        {
            throw DataGridError::DataGridAutomationPeer::OperationCannotBePerformed();
        }

        auto const internalOwningDataGrid = OwningDataGrid().as<DataGrid>();
        if (scrollHorizontally)
        {
            auto const& scrollBar = internalOwningDataGrid->HorizontalScrollBar();
            scrollBar.Value(scrollBar.Maximum() * (horizontalPercent / 100.0));
            internalOwningDataGrid->ProcessHorizontalScroll(Controls::Primitives::ScrollEventType::ThumbPosition);
        }

        if (scrollVertically)
        {
            auto const& scrollBar = internalOwningDataGrid->VerticalScrollBar();
            scrollBar.Value(scrollBar.Maximum() * (horizontalPercent / 100.0));
            internalOwningDataGrid->ProcessHorizontalScroll(Controls::Primitives::ScrollEventType::ThumbPosition);
        }
    }

    com_array<Automation::Provider::IRawElementProviderSimple> DataGridAutomationPeer::GetSelection()
    {
        if (auto const internalOwningDataGrid = OwningDataGrid().as<DataGrid>();
            internalOwningDataGrid)
        {
            std::vector<Automation::Provider::IRawElementProviderSimple> selectedProviders;
            if(auto const selectedItems = internalOwningDataGrid->SelectedItems();
                selectedItems)
            {
                for(auto const& item : selectedItems)
                {
                    if (auto const peer = GetOrCreateItemPeer(item);
                        peer)
                    {
                        selectedProviders.push_back(ProviderFromPeer(peer));
                    }
                }
            }
            return {selectedProviders.begin(), selectedProviders.end()};
        }

        return {};
    }

    bool DataGridAutomationPeer::CanSelectMultiple() const
    {
        return OwningDataGrid().SelectionMode() == DataGridSelectionMode::Extended;
    }

    // ReSharper disable once CppMemberFunctionMayBeStatic
    bool DataGridAutomationPeer::IsSelectionRequired() const
    {
        return false;
    }

    // ReSharper disable once CppMemberFunctionMayBeStatic
    Automation::RowOrColumnMajor DataGridAutomationPeer::RowOrColumnMajor() const
    {
        return Automation::RowOrColumnMajor::RowMajor;
    }

    com_array<Automation::Provider::IRawElementProviderSimple> DataGridAutomationPeer::GetColumnHeaders() const
    {
        if (auto const internalOwningDataGrid = OwningDataGrid().as<DataGrid>();
            internalOwningDataGrid->AreColumnHeadersVisible())
        {
            std::vector<Automation::Provider::IRawElementProviderSimple> providers;
            for(auto const& column : internalOwningDataGrid->Columns())
            {
                auto const internalColumn = column.as<DataGridColumn>();
                if (auto const headerCell = internalColumn->HeaderCell();
                    headerCell)
                {
                    if (auto const peer = Automation::Peers::FrameworkElementAutomationPeer::CreatePeerForElement(headerCell);
                        peer)
                    {
                        providers.push_back(ProviderFromPeer(peer));
                    }
                }
            }

            return {providers.begin(), providers.end()};
        }

        return {};
    }

    com_array<Automation::Provider::IRawElementProviderSimple> DataGridAutomationPeer::GetRowHeaders() const
    {
        if (auto const internalOwningDataGrid = OwningDataGrid().as<DataGrid>();
            internalOwningDataGrid ->AreRowHeadersVisible())
        {
            std::vector<Automation::Provider::IRawElementProviderSimple> providers;
            for(auto const& row : internalOwningDataGrid->DisplayData().GetScrollingElements(true /*onlyRows*/))
            {
                auto const internalRow = row.as<DataGridRow>();
                if (auto const headerCell = internalRow->HeaderCell();
                    headerCell)
                {
                    if (auto const peer = Automation::Peers::FrameworkElementAutomationPeer::CreatePeerForElement(headerCell);
                        peer)
                    {
                        providers.push_back(ProviderFromPeer(peer));
                    }
                }
            }

            return {providers.begin(), providers.end()};
        }

        return {};
    }

    void DataGridAutomationPeer::RaiseAutomationInvokeEvent(UIElement const& element)
    {
        if (Automation::Peers::AutomationPeer::ListenerExists(Automation::Peers::AutomationEvents::InvokePatternOnInvoked))
        {
            if (auto const peer = Automation::Peers::FrameworkElementAutomationPeer::FromElement(element);
                peer)
            {
                peer.RaiseAutomationEvent(Automation::Peers::AutomationEvents::InvokePatternOnInvoked);
            }
        }
    }

    Windows::Foundation::Collections::IVector<Automation::Peers::AutomationPeer> DataGridAutomationPeer::GetChildPeers()
    {
        auto peers = single_threaded_vector<Automation::Peers::AutomationPeer>();
        PopulateGroupItemPeers();
        PopulateItemPeers();
        if (!groupItemPeers_.empty())  // NOLINT(bugprone-branch-clone)
        {
            for (auto const internalOwningDataGrid = OwningDataGrid().as<DataGrid>();
                auto const& group : internalOwningDataGrid->DataConnection().CollectionView().CollectionGroups() /*Groups*/)
            {
                if(auto const it = groupItemPeers_.find(group);
                    it != groupItemPeers_.end())
                {
                    peers.Append(it->second);
                }
            }
        }
        else
        {
            for (auto const& itemPeer : itemPeers_ | std::views::values)
            {
                peers.Append(itemPeer);
            }
        }

        return peers;
    }

    MiniDumpExplorer::DataGridGroupItemAutomationPeer DataGridAutomationPeer::GetOrCreateGroupItemPeer(Windows::Foundation::IInspectable const& group)
    {
        MiniDumpExplorer::DataGridGroupItemAutomationPeer peer{nullptr};

        if (group)
        {
            if (auto const it = groupItemPeers_.find(group);
                it != groupItemPeers_.end())
            {
                peer = it->second;
            }
            else
            {
                peer = MiniDumpExplorer::DataGridGroupItemAutomationPeer{group.as<Data::ICollectionViewGroup>(), OwningDataGrid()};
                groupItemPeers_.insert(std::make_pair(group, peer));
            }

            if (auto const rghPeer = peer.as<DataGridGroupItemAutomationPeer>()->OwningRowGroupHeaderPeer();
                rghPeer)
            {
                rghPeer.EventsSource(peer);
            }
        }

        return peer;
    }

    MiniDumpExplorer::DataGridItemAutomationPeer DataGridAutomationPeer::GetOrCreateItemPeer(Windows::Foundation::IInspectable const& item)
    {
        MiniDumpExplorer::DataGridItemAutomationPeer peer{nullptr};

        if (item)
        {
            if (auto const it = itemPeers_.find(item);
                it != itemPeers_.end())
            {
                peer = it->second;
            }
            else
            {
                peer = MiniDumpExplorer::DataGridItemAutomationPeer{item, OwningDataGrid()};
                itemPeers_.insert(std::make_pair(item, peer));
            }

            
            if (auto const rowPeer = peer.as<DataGridItemAutomationPeer>()->OwningRowPeer();
                rowPeer)
            {
                rowPeer.EventsSource(peer);
            }
        }

        return peer;
    }

    void DataGridAutomationPeer::PopulateGroupItemPeers()
    {
        auto oldChildren = groupItemPeers_;
        groupItemPeers_.clear();

        auto const internalOwningDataGrid = OwningDataGrid().as<DataGrid>();
        
        if (auto const collectionView = internalOwningDataGrid->DataConnection().CollectionView();
            collectionView &&
            collectionView.CollectionGroups())
        {
            for(auto const& group : collectionView.CollectionGroups())
            {
                if(auto const cvGroup = group.try_as<Data::ICollectionViewGroup>();
                    cvGroup)
                {
                    // Add the group's peer to the collection
                    MiniDumpExplorer::DataGridGroupItemAutomationPeer peer{nullptr};

                    if (auto const it = oldChildren.find(cvGroup);
                        it != oldChildren.end())
                    {
                        peer = it->second.as<MiniDumpExplorer::DataGridGroupItemAutomationPeer>();
                    }
                    else
                    {
                        peer = MiniDumpExplorer::DataGridGroupItemAutomationPeer{cvGroup, OwningDataGrid()};
                    }

                    if (peer)
                    {
                        
                        if (auto const rghPeer = peer.as<DataGridGroupItemAutomationPeer>()->OwningRowGroupHeaderPeer();
                            rghPeer)
                        {
                            rghPeer.EventsSource(peer);
                        }
                    }

                    // This guards against the addition of duplicate items
                    if (!groupItemPeers_.contains(cvGroup))
                    {
                        groupItemPeers_.insert(std::make_pair(cvGroup, peer));
                    }
                }
            }
        }
    }

    void DataGridAutomationPeer::PopulateItemPeers()
    {
        auto oldChildren = itemPeers_;
        itemPeers_.clear();

        auto const internalOwningDataGrid = OwningDataGrid().as<DataGrid>();
        if (auto const itemsSource = internalOwningDataGrid->ItemsSource();
            itemsSource)
        {
            for(auto const& item : itemsSource.CollectionView())
            {
                if (item)
                {
                    MiniDumpExplorer::DataGridItemAutomationPeer peer{nullptr};
                    if (auto const it = oldChildren.find(item);
                        it != oldChildren.end())
                    {
                        peer = it->second.as<MiniDumpExplorer::DataGridItemAutomationPeer>();
                    }
                    else
                    {
                        peer = MiniDumpExplorer::DataGridItemAutomationPeer{item, OwningDataGrid()};
                    }

                    if (peer)
                    {
                        if (auto const rowPeer = peer.as<DataGridItemAutomationPeer>()->OwningRowPeer();
                            rowPeer)
                        {
                            rowPeer.EventsSource(peer);
                        }
                    }

                    // This guards against the addition of duplicate items
                    if (!itemPeers_.contains(item))
                    {
                        itemPeers_.insert(std::make_pair(item, peer));
                    }
                }
            }
        }
    }

    void DataGridAutomationPeer::RaiseAutomationCellSelectedEvent(int32_t const slot, int32_t const column) const
    {
        if (auto const cellPeer = GetCellPeer(slot, column);
            cellPeer)
        {
            cellPeer.RaiseAutomationEvent(Automation::Peers::AutomationEvents::SelectionItemPatternOnElementSelected);
        }
    }

    void DataGridAutomationPeer::RaiseAutomationFocusChangedEvent(int32_t const slot, int32_t const column) const
    {
        if (auto const internalOwningDataGrid = OwningDataGrid().as<DataGrid>();
            slot < internalOwningDataGrid->SlotCount() &&
            column < internalOwningDataGrid->ColumnsItemsInternal().size() &&
            internalOwningDataGrid->IsSlotVisible(slot))
        {
            if (internalOwningDataGrid->RowGroupHeadersTable().Contains(slot))
            {
                if (auto const header = internalOwningDataGrid->DisplayData().GetDisplayedElement(slot).as<MiniDumpExplorer::DataGridRowGroupHeader>();
                    header)
                {
                    if (auto const headerPeer = Automation::Peers::FrameworkElementAutomationPeer::CreatePeerForElement(header);
                        headerPeer)
                    {
                        headerPeer.RaiseAutomationEvent(Automation::Peers::AutomationEvents::AutomationFocusChanged);
                    }
                }
            }
            else
            {
                if (auto const cellPeer = GetCellPeer(slot, column);
                    cellPeer)
                {
                    cellPeer.RaiseAutomationEvent(Automation::Peers::AutomationEvents::AutomationFocusChanged);
                }
            }
        }
    }

    void DataGridAutomationPeer::RaiseAutomationInvokeEvents(DataGridEditingUnit const& editingUnit, MiniDumpExplorer::DataGridColumn const& column, MiniDumpExplorer::DataGridRow const& row)
    {
        switch (editingUnit)
        {
        case DataGridEditingUnit::Cell:
        {
            auto const cell = row.as<DataGridRow>()->Cells()[column.as<DataGridColumn>()->Index()];
            auto peer = Automation::Peers::FrameworkElementAutomationPeer::FromElement(cell);
            if (peer)
            {
                peer.InvalidatePeer();
            }
            else
            {
                peer = Automation::Peers::FrameworkElementAutomationPeer::CreatePeerForElement(cell);
            }

            if (peer)
            {
                peer.RaiseAutomationEvent(Automation::Peers::AutomationEvents::InvokePatternOnInvoked);
            }

            break;
        }

        case DataGridEditingUnit::Row:
        {
            auto const peer = GetOrCreateItemPeer(row.DataContext());
            peer.RaiseAutomationEvent(Automation::Peers::AutomationEvents::InvokePatternOnInvoked);
            break;
        }
        }
    }

    void DataGridAutomationPeer::RaiseAutomationScrollEvents()
    {
        auto const newHorizontallyScrollable = HorizontallyScrollable();
        auto const newHorizontalViewSize = HorizontalViewSize();
        auto const newHorizontalScrollPercent = HorizontalScrollPercent();

        auto const newVerticallyScrollable = VerticallyScrollable();
        auto const newVerticalViewSize = VerticalViewSize();
        auto const newVerticalScrollPercent = VerticalScrollPercent();

        if (oldHorizontallyScrollable_ != newHorizontallyScrollable)
        {
            RaisePropertyChangedEvent(
                Automation::ScrollPatternIdentifiers::HorizontallyScrollableProperty(),
                box_value(oldHorizontallyScrollable_),
                box_value(newHorizontallyScrollable));
            oldHorizontallyScrollable_ = newHorizontallyScrollable;
        }

        if (oldHorizontalViewSize_ != newHorizontalViewSize)
        {
            RaisePropertyChangedEvent(
                Automation::ScrollPatternIdentifiers::HorizontalViewSizeProperty(),
                box_value(oldHorizontalViewSize_),
                box_value(newHorizontalViewSize));
            oldHorizontalViewSize_ = newHorizontalViewSize;
        }

        if (oldHorizontalScrollPercent_ != newHorizontalScrollPercent)
        {
            RaisePropertyChangedEvent(
                Automation::ScrollPatternIdentifiers::HorizontalScrollPercentProperty(),
                box_value(oldHorizontalScrollPercent_),
                box_value(newHorizontalScrollPercent));
            oldHorizontalScrollPercent_ = newHorizontalScrollPercent;
        }

        if (oldVerticallyScrollable_ != newVerticallyScrollable)
        {
            RaisePropertyChangedEvent(
                Automation::ScrollPatternIdentifiers::VerticallyScrollableProperty(),
                box_value(oldVerticallyScrollable_),
                box_value(newVerticallyScrollable));
            oldVerticallyScrollable_ = newVerticallyScrollable;
        }

        if (oldVerticalViewSize_ != newVerticalViewSize)
        {
            RaisePropertyChangedEvent(
                Automation::ScrollPatternIdentifiers::VerticalViewSizeProperty(),
                box_value(oldVerticalViewSize_),
                box_value(newVerticalViewSize));
            oldVerticalViewSize_ = newVerticalViewSize;
        }

        if (oldVerticalScrollPercent_ != newVerticalScrollPercent)
        {
            RaisePropertyChangedEvent(
                Automation::ScrollPatternIdentifiers::VerticalScrollPercentProperty(),
                box_value(oldVerticalScrollPercent_),
                box_value(newVerticalScrollPercent));
            oldVerticalScrollPercent_ = newVerticalScrollPercent;
        }
    }

    void DataGridAutomationPeer::RaiseAutomationSelectionEvents(Controls::SelectionChangedEventArgs const& e)
    {
        // If the result of an AddToSelection or RemoveFromSelection is a single selected item,
        // then all we raise is the ElementSelectedEvent for single item
        auto const internalOwningDataGrid = OwningDataGrid().as<DataGrid>();
        if (auto const selectedItems = internalOwningDataGrid->SelectedItems();
            Automation::Peers::AutomationPeer::ListenerExists(Automation::Peers::AutomationEvents::SelectionItemPatternOnElementSelected) &&
            selectedItems.Size() == 1)
        {
            if (auto const selectedItem = internalOwningDataGrid->SelectedItem();
                selectedItem)
            {
                if(auto const it = itemPeers_.find(selectedItem);
                    it != itemPeers_.end())
                {
                    it->second.RaiseAutomationEvent(Automation::Peers::AutomationEvents::SelectionItemPatternOnElementSelected);
                }
            }
        }
        else
        {
            if (Automation::Peers::AutomationPeer::ListenerExists(Automation::Peers::AutomationEvents::SelectionItemPatternOnElementAddedToSelection))
            {
                for (auto const& item : e.AddedItems())
                {
                    if (item)
                    {
                        if(auto const it = itemPeers_.find(item);
                            it != itemPeers_.end())
                        {
                            it->second.RaiseAutomationEvent(Automation::Peers::AutomationEvents::SelectionItemPatternOnElementAddedToSelection);
                        }
                    }
                }
            }

            if (Automation::Peers::AutomationPeer::ListenerExists(Automation::Peers::AutomationEvents::SelectionItemPatternOnElementRemovedFromSelection))
            {
                for (auto const& item : e.RemovedItems())
                {
                    if (item)
                    {
                        if(auto const it = itemPeers_.find(item);
                            it != itemPeers_.end())
                        {
                            it->second.RaiseAutomationEvent(Automation::Peers::AutomationEvents::SelectionItemPatternOnElementRemovedFromSelection);
                        }
                    }
                }
            }
        }
    }

    void DataGridAutomationPeer::UpdateRowGroupHeaderPeerEventsSource(MiniDumpExplorer::DataGridRowGroupHeader const& header)
    {
        auto const group = header.as<DataGridRowGroupHeader>()->RowGroupInfo()->CollectionViewGroup();
        if (auto const peer = Automation::Peers::FrameworkElementAutomationPeer::FromElement(header).as<MiniDumpExplorer::DataGridRowGroupHeaderAutomationPeer>();
            peer && group)
        {
            if(auto const it = groupItemPeers_.find(group);
                it != groupItemPeers_.end())
            {
                peer.EventsSource(it->second);
            }
        }
    }

    void DataGridAutomationPeer::UpdateRowPeerEventsSource(MiniDumpExplorer::DataGridRow const& row)
    {
        auto const peer = Automation::Peers::FrameworkElementAutomationPeer::FromElement(row).as<MiniDumpExplorer::DataGridRowAutomationPeer>();
        if (!peer)
        {
            return;
        }

        auto const dataContext = row.DataContext();
        if(!dataContext)
        {
            return;
        }

        auto const it = itemPeers_.find(dataContext);
        if(it == itemPeers_.end())
        {
            return;
        }

        peer.EventsSource(it->second);
    }

    MiniDumpExplorer::DataGrid DataGridAutomationPeer::OwningDataGrid() const
    {
        return Owner().as<MiniDumpExplorer::DataGrid>();
    }

    Automation::Peers::AutomationPeer DataGridAutomationPeer::GetCellPeer(int32_t const slot, int32_t const column) const
    {
        if (auto const internalOwningDataGrid = OwningDataGrid().as<DataGrid>();
            slot < internalOwningDataGrid->SlotCount() &&
            column < internalOwningDataGrid->ColumnsItemsInternal().size() &&
            internalOwningDataGrid->IsSlotVisible(slot))
        {
            if (auto const row = internalOwningDataGrid->DisplayData().GetDisplayedElement(slot).as<DataGridRow>();
                row)
            {
                assert(column < internalOwningDataGrid->ColumnsItemsInternal().size());
                auto const cell = row->Cells()[column];
                return Automation::Peers::FrameworkElementAutomationPeer::CreatePeerForElement(cell);
            }
        }

        return nullptr;
    }
}
