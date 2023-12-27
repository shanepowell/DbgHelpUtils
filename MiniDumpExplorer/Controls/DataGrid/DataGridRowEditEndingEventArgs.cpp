#include "pch.h"
#include "DataGridRowEditEndingEventArgs.h"
#if __has_include("DataGridRowEditEndingEventArgs.g.cpp")
// ReSharper disable once CppUnusedIncludeDirective
#include "DataGridRowEditEndingEventArgs.g.cpp"  // NOLINT(bugprone-suspicious-include)
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    DataGridRowEditEndingEventArgs::DataGridRowEditEndingEventArgs(MiniDumpExplorer::DataGridRow const& row, DataGridEditAction const editAction)
        : row_{row}
        , editAction_{editAction}
    {
    }
}
