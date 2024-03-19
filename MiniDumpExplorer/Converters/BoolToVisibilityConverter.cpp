#include "pch.h"
#include "BoolToVisibilityConverter.h"
#if __has_include("BoolToVisibilityConverter.g.cpp")
// ReSharper disable once CppUnusedIncludeDirective
#include "BoolToVisibilityConverter.g.cpp"  // NOLINT(bugprone-suspicious-include)
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    BoolToVisibilityConverter::BoolToVisibilityConverter()
    {
        TrueValue(box_value(Visibility::Visible));
        FalseValue(box_value(Visibility::Collapsed));
    }
}
