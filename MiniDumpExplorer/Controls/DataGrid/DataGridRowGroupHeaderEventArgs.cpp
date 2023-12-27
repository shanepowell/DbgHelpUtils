#include "pch.h"
#include "DataGridRowGroupHeaderEventArgs.h"
#if __has_include("DataGridRowGroupHeaderEventArgs.g.cpp")
// ReSharper disable once CppUnusedIncludeDirective
#include "DataGridRowGroupHeaderEventArgs.g.cpp"  // NOLINT(bugprone-suspicious-include)
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    DataGridRowGroupHeaderEventArgs::DataGridRowGroupHeaderEventArgs(MiniDumpExplorer::DataGridRowGroupHeader const& rowGroupHeader)
        : rowGroupHeader_(rowGroupHeader)
    {
    }
}
