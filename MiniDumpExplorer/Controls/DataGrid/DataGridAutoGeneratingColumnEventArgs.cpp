#include "pch.h"
#include "DataGridAutoGeneratingColumnEventArgs.h"

#include "DataGridColumn.h"

#if __has_include("DataGridAutoGeneratingColumnEventArgs.g.cpp")
// ReSharper disable once CppUnusedIncludeDirective
#include "DataGridAutoGeneratingColumnEventArgs.g.cpp"  // NOLINT(bugprone-suspicious-include)
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    DataGridAutoGeneratingColumnEventArgs::DataGridAutoGeneratingColumnEventArgs(hstring const& propertyName, MiniDumpExplorer::DataGridColumn const& column)
        : propertyName_{propertyName}
        , column_{column}
    {
    }
}
