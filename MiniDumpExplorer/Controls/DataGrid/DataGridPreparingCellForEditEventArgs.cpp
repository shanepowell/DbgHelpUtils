#include "pch.h"
#include "DataGridPreparingCellForEditEventArgs.h"
#if __has_include("DataGridPreparingCellForEditEventArgs.g.cpp")
// ReSharper disable once CppUnusedIncludeDirective
#include "DataGridPreparingCellForEditEventArgs.g.cpp"  // NOLINT(bugprone-suspicious-include)
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    DataGridPreparingCellForEditEventArgs::DataGridPreparingCellForEditEventArgs(MiniDumpExplorer::DataGridColumn const& column, MiniDumpExplorer::DataGridRow const& row, RoutedEventArgs const& editingEventArgs, FrameworkElement const& editingElement)
        : column_{column}
        , row_{row}
        , editingEventArgs_{editingEventArgs}
        , editingElement_{editingElement}
    {
    }
}
