#include "pch.h"
#include "DataGridColumnReorderingEventArgs.h"

#if __has_include("DataGridColumnReorderingEventArgs.g.cpp")
// ReSharper disable once CppUnusedIncludeDirective
#include "DataGridColumnReorderingEventArgs.g.cpp"  // NOLINT(bugprone-suspicious-include)
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    DataGridColumnReorderingEventArgs::DataGridColumnReorderingEventArgs(MiniDumpExplorer::DataGridColumn const& dataGridColumn)
        : column_{dataGridColumn}
    {
    }
}
