#include "pch.h"
#include "DataGridClipboardCellContent.h"

#if __has_include("DataGridClipboardCellContent.g.cpp")
// ReSharper disable once CppUnusedIncludeDirective
#include "DataGridClipboardCellContent.g.cpp"  // NOLINT(bugprone-suspicious-include)
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    DataGridClipboardCellContent::DataGridClipboardCellContent(IInspectable const& item, MiniDumpExplorer::DataGridColumn const& column, IInspectable const& content)
        : item_{item}
        , column_{column}
        , content_{content}
    {
    }
}
