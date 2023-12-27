#include "pch.h"
#include "DataGridCellEditEndingEventArgs.h"

#if __has_include("DataGridCellEditEndingEventArgs.g.cpp")
// ReSharper disable once CppUnusedIncludeDirective
#include "DataGridCellEditEndingEventArgs.g.cpp"  // NOLINT(bugprone-suspicious-include)
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    DataGridCellEditEndingEventArgs::DataGridCellEditEndingEventArgs(MiniDumpExplorer::DataGridColumn const& column, MiniDumpExplorer::DataGridRow const& row, FrameworkElement const& editingElement, DataGridEditAction const editAction)
        : column_{column}
        , row_{row}
        , editingElement_{editingElement}
        , editAction_{editAction}
    {
    }
}
