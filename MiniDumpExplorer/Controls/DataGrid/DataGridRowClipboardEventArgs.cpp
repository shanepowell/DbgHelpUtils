#include "pch.h"
#include "DataGridRowClipboardEventArgs.h"
#if __has_include("DataGridRowClipboardEventArgs.g.cpp")
// ReSharper disable once CppUnusedIncludeDirective
#include "DataGridRowClipboardEventArgs.g.cpp"  // NOLINT(bugprone-suspicious-include)
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    DataGridRowClipboardEventArgs::DataGridRowClipboardEventArgs(Windows::Foundation::IInspectable const& item, bool const isColumnHeadersRow)
        : item_{item}
        , isColumnHeadersRow_{isColumnHeadersRow}
    {
    }
}
