#include "pch.h"
#include "DataGridBeginningEditEventArgs.h"

#include "DataGridColumn.h"

#if __has_include("DataGridBeginningEditEventArgs.g.cpp")
// ReSharper disable once CppUnusedIncludeDirective
#include "DataGridBeginningEditEventArgs.g.cpp"  // NOLINT(bugprone-suspicious-include)
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    DataGridBeginningEditEventArgs::DataGridBeginningEditEventArgs(MiniDumpExplorer::DataGridColumn const& column, MiniDumpExplorer::DataGridRow const& row, RoutedEventArgs const& editingEventArgs)
        : column_{column}
        , row_{row}
        , editingEventArgs_{editingEventArgs}
    {
    }

    MiniDumpExplorer::DataGridColumn DataGridBeginningEditEventArgs::Column() const
    {
        return column_;
    }

    MiniDumpExplorer::DataGridRow DataGridBeginningEditEventArgs::Row() const
    {
        return row_;
    }

    RoutedEventArgs DataGridBeginningEditEventArgs::EditingEventArgs()
    {
       return editingEventArgs_;
    }
}
