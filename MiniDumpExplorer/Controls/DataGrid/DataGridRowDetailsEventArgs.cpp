#include "pch.h"
#include "DataGridRowDetailsEventArgs.h"
#if __has_include("DataGridRowDetailsEventArgs.g.cpp")
// ReSharper disable once CppUnusedIncludeDirective
#include "DataGridRowDetailsEventArgs.g.cpp"  // NOLINT(bugprone-suspicious-include)
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    DataGridRowDetailsEventArgs::DataGridRowDetailsEventArgs(MiniDumpExplorer::DataGridRow const& row, FrameworkElement const& detailsElement)
        : row_{row}
        , detailsElement_{detailsElement}
    {
    }
}
