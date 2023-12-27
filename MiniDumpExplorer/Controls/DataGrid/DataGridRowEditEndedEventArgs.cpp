#include "pch.h"
#include "DataGridRowEditEndedEventArgs.h"
#if __has_include("DataGridRowEditEndedEventArgs.g.cpp")
// ReSharper disable once CppUnusedIncludeDirective
#include "DataGridRowEditEndedEventArgs.g.cpp"  // NOLINT(bugprone-suspicious-include)
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    DataGridRowEditEndedEventArgs::DataGridRowEditEndedEventArgs(MiniDumpExplorer::DataGridRow const& row, DataGridEditAction const editAction)
        : row_{row}
        , editAction_{editAction}
    {
    }
}
