#include "pch.h"
#include "DataGridColumnEventArgs.h"

#if __has_include("DataGridColumnEventArgs.g.cpp")
// ReSharper disable once CppUnusedIncludeDirective
#include "DataGridColumnEventArgs.g.cpp"  // NOLINT(bugprone-suspicious-include)
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    DataGridColumnEventArgs::DataGridColumnEventArgs(MiniDumpExplorer::DataGridColumn const& column)
        : column_(column)
    {
    }
}
