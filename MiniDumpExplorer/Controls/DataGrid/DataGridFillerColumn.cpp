#include "pch.h"
#include "DataGridFillerColumn.h"

#include <winrt/Microsoft.UI.Xaml.Automation.h>

#if __has_include("DataGridFillerColumn.g.cpp")
// ReSharper disable once CppUnusedIncludeDirective
#include "DataGridFillerColumn.g.cpp"  // NOLINT(bugprone-suspicious-include)
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    DataGridFillerColumn::DataGridFillerColumn(MiniDumpExplorer::DataGrid const& owningGrid)
    {
        IsReadOnly(false);
        OwningGrid(owningGrid);
        MinWidth(0);
        MaxWidth(std::numeric_limits<double>::max());
    }

    MiniDumpExplorer::DataGridColumnHeader DataGridFillerColumn::CreateHeader()
{
        auto headerCell = base_type::CreateHeader();
        if (headerCell)
        {
            Automation::AutomationProperties::SetAccessibilityView(
                headerCell,
                Automation::Peers::AccessibilityView::Raw);
            headerCell.IsEnabled(false);
        }

        return headerCell;
    }

    FrameworkElement DataGridFillerColumn::GenerateEditingElement([[maybe_unused]] MiniDumpExplorer::DataGridCell const& cell, [[maybe_unused]] Windows::Foundation::IInspectable const& dataItem)
    {
        return nullptr;
    }

    FrameworkElement DataGridFillerColumn::GenerateElement([[maybe_unused]] MiniDumpExplorer::DataGridCell const& cell, [[maybe_unused]] Windows::Foundation::IInspectable const& dataItem)
    {
        return nullptr;
    }

    Windows::Foundation::IInspectable DataGridFillerColumn::PrepareCellForEdit([[maybe_unused]] FrameworkElement const& editingElement, [[maybe_unused]] RoutedEventArgs const& editingEventArgs)
    {
        assert(false);
        return nullptr;
    }
}
