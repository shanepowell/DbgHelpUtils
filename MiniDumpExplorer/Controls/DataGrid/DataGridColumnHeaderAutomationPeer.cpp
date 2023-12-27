#include "pch.h"
#include "DataGridColumnHeaderAutomationPeer.h"

#include "DataGrid.h"
#include "DataGridColumn.h"
#include "DataGridColumnHeader.h"
#include "DataGridError.h"

#if __has_include("DataGridColumnHeaderAutomationPeer.g.cpp")
// ReSharper disable once CppUnusedIncludeDirective
#include "DataGridColumnHeaderAutomationPeer.g.cpp"  // NOLINT(bugprone-suspicious-include)
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    auto DataGridColumnHeaderAutomationPeer::InternalOwningHeader() const
    {
        return OwningHeader().as<DataGridColumnHeader>();
    }


    DataGridColumnHeaderAutomationPeer::DataGridColumnHeaderAutomationPeer(MiniDumpExplorer::DataGridColumnHeader const& owner)
        : base_type(owner)
    {
    }

    // ReSharper disable once CppMemberFunctionMayBeStatic
    Automation::Peers::AutomationControlType DataGridColumnHeaderAutomationPeer::GetAutomationControlTypeCore() const
    {
        return Automation::Peers::AutomationControlType::HeaderItem;
    }

    hstring DataGridColumnHeaderAutomationPeer::GetClassNameCore() const
    {
        return get_class_name(Owner());
    }

    hstring DataGridColumnHeaderAutomationPeer::GetHelpTextCore() const
    {
        auto const internalOwningHeader = InternalOwningHeader();
        if (auto const internalOwningColumn = internalOwningHeader->OwningColumn();
            internalOwningColumn && internalOwningColumn.SortDirection())
        {
            if (internalOwningColumn.SortDirection().Value() == DataGridSortDirection::Ascending)
            {
                return L"Ascending";
            }

            return L"Descending";
        }

        return base_type::GetHelpTextCore();
    }

    hstring DataGridColumnHeaderAutomationPeer::GetNameCore() const
    {
        if (auto const header = OwningHeader().Content().try_as<hstring>();
            header.has_value())
        {
            return header.value();
        }

        return base_type::GetNameCore();
    }

    Windows::Foundation::IInspectable DataGridColumnHeaderAutomationPeer::GetPatternCore(Automation::Peers::PatternInterface const patternInterface) const
    {
        auto const internalOwningHeader = InternalOwningHeader();
        auto const internalOwningColumn = internalOwningHeader->OwningColumn().as<DataGridColumn>();
        
        if (auto const internalOwningGrid = internalOwningHeader->OwningGrid().try_as<DataGrid>();
            internalOwningGrid)
        {
            switch (patternInterface)
            {
            case Automation::Peers::PatternInterface::Invoke:
                // this.OwningHeader.OwningGrid.DataConnection.AllowSort property is ignored because of the DataGrid.Sorting custom sorting capability.
                if (internalOwningGrid->CanUserSortColumns() &&
                    internalOwningColumn->CanUserSort())
                {
                    return *this;
                }

                break;

            case Automation::Peers::PatternInterface::ScrollItem:
                if (internalOwningGrid->HorizontalScrollBar() &&
                    internalOwningGrid->HorizontalScrollBar().Maximum() > 0)
                {
                    return *this;
                }

                break;

            case Automation::Peers::PatternInterface::Transform:
                if (internalOwningColumn &&
                    internalOwningColumn->ActualCanUserResize())
                {
                    return *this;
                }

                break;

            case Automation::Peers::PatternInterface::Selection:
            case Automation::Peers::PatternInterface::Value:
            case Automation::Peers::PatternInterface::RangeValue:
            case Automation::Peers::PatternInterface::Scroll:
            case Automation::Peers::PatternInterface::ExpandCollapse:
            case Automation::Peers::PatternInterface::Grid:
            case Automation::Peers::PatternInterface::GridItem:
            case Automation::Peers::PatternInterface::MultipleView:
            case Automation::Peers::PatternInterface::Window:
            case Automation::Peers::PatternInterface::SelectionItem:
            case Automation::Peers::PatternInterface::Dock:
            case Automation::Peers::PatternInterface::Table:
            case Automation::Peers::PatternInterface::TableItem:
            case Automation::Peers::PatternInterface::Toggle:
            case Automation::Peers::PatternInterface::Text:
            case Automation::Peers::PatternInterface::ItemContainer:
            case Automation::Peers::PatternInterface::VirtualizedItem:
            case Automation::Peers::PatternInterface::Text2:
            case Automation::Peers::PatternInterface::TextChild:
            case Automation::Peers::PatternInterface::TextRange:
            case Automation::Peers::PatternInterface::Annotation:
            case Automation::Peers::PatternInterface::Drag:
            case Automation::Peers::PatternInterface::DropTarget:
            case Automation::Peers::PatternInterface::ObjectModel:
            case Automation::Peers::PatternInterface::Spreadsheet:
            case Automation::Peers::PatternInterface::SpreadsheetItem:
            case Automation::Peers::PatternInterface::Styles:
            case Automation::Peers::PatternInterface::Transform2:
            case Automation::Peers::PatternInterface::SynchronizedInput:
            case Automation::Peers::PatternInterface::TextEdit:
            case Automation::Peers::PatternInterface::CustomNavigation:
            default:
                break;
            }
        }

        return base_type::GetPatternCore(patternInterface);
    }

    void DataGridColumnHeaderAutomationPeer::Invoke() const
    {
        InternalOwningHeader()->InvokeProcessSort();
    }

    void DataGridColumnHeaderAutomationPeer::ScrollIntoView() const
    {
        auto const internalOwningHeader = InternalOwningHeader();
        internalOwningHeader->OwningGrid().ScrollIntoView(nullptr, internalOwningHeader->OwningColumn());
    }

    bool DataGridColumnHeaderAutomationPeer::CanResize() const
    {
        auto const internalOwningHeader = InternalOwningHeader();
        auto const internalOwningColumn = internalOwningHeader->OwningColumn().try_as<DataGridColumn>();
        return internalOwningColumn && internalOwningColumn->ActualCanUserResize();
    }

    // ReSharper disable once CppMemberFunctionMayBeStatic
    void DataGridColumnHeaderAutomationPeer::Move([[maybe_unused]] double const x, [[maybe_unused]] double const y) const
    {
        throw DataGridError::DataGridAutomationPeer::OperationCannotBePerformed();
    }

    void DataGridColumnHeaderAutomationPeer::Resize(double const width, [[maybe_unused]] double const height) const
    {
        if (auto const internalOwningColumn = InternalOwningHeader()->OwningColumn().try_as<DataGridColumn>();
            internalOwningColumn &&
            internalOwningColumn->ActualCanUserResize())
        {
            internalOwningColumn->Width(MiniDumpExplorer::DataGridLength{width});
        }
    }

    // ReSharper disable once CppMemberFunctionMayBeStatic
    void DataGridColumnHeaderAutomationPeer::Rotate([[maybe_unused]] double const degrees) const
    {
        throw DataGridError::DataGridAutomationPeer::OperationCannotBePerformed();
    }

    MiniDumpExplorer::DataGridColumnHeader DataGridColumnHeaderAutomationPeer::OwningHeader() const
    {
        return Owner().as<MiniDumpExplorer::DataGridColumnHeader>();
    }
}
