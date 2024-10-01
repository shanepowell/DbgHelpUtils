#include "pch.h"
#include "DataGridCellEventArgs.h"
#if __has_include("DataGridCellEventArgs.g.cpp")
// ReSharper disable once CppUnusedIncludeDirective
#include "DataGridCellEventArgs.g.cpp"  // NOLINT(bugprone-suspicious-include)
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    DataGridCellEventArgs::DataGridCellEventArgs(MiniDumpExplorer::DataGridCell const& dataGridCell)
        : cell_{dataGridCell}
    {
    }
}
