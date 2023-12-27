#include "pch.h"
#include "DataGridCellEditEndedEventArgs.h"

#if __has_include("DataGridCellEditEndedEventArgs.g.cpp")
// ReSharper disable once CppUnusedIncludeDirective
#include "DataGridCellEditEndedEventArgs.g.cpp"  // NOLINT(bugprone-suspicious-include)
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    DataGridCellEditEndedEventArgs::DataGridCellEditEndedEventArgs(MiniDumpExplorer::DataGridColumn const& column, MiniDumpExplorer::DataGridRow const& row, DataGridEditAction const editAction)
        : column_{column}
        , row_{row}
        , editAction_{editAction}
    {
    }
}
