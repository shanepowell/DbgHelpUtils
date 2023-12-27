#include "pch.h"
#include "DataGridRowEventArgs.h"
#if __has_include("DataGridRowEventArgs.g.cpp")
// ReSharper disable once CppUnusedIncludeDirective
#include "DataGridRowEventArgs.g.cpp"  // NOLINT(bugprone-suspicious-include)
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    DataGridRowEventArgs::DataGridRowEventArgs(MiniDumpExplorer::DataGridRow const& dataGridRow)
        : row_{dataGridRow}
    {
    }
}
