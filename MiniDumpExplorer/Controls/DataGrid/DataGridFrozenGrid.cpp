#include "pch.h"
#include "DataGridFrozenGrid.h"

#include <winrt/Windows.UI.Xaml.Interop.h>

#include "DbgHelpUtils/wide_runtime_error.h"

#if __has_include("DataGridFrozenGrid.g.cpp")
// ReSharper disable once CppUnusedIncludeDirective
#include "DataGridFrozenGrid.g.cpp"  // NOLINT(bugprone-suspicious-include)
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;
using namespace std::string_literals;

namespace winrt::MiniDumpExplorer::implementation
{
    DataGridFrozenGrid::DataGridFrozenGrid() = default;

    DependencyProperty DataGridFrozenGrid::IsFrozenProperty()
    {
        static DependencyProperty s_IsFrozenProperty = DependencyProperty::RegisterAttached(
            L"IsFrozen",
            xaml_typename<bool>(),
            xaml_typename<MiniDumpExplorer::DataGridFrozenGrid>(),
            PropertyMetadata{ nullptr });

        return s_IsFrozenProperty;
    }

    bool DataGridFrozenGrid::GetIsFrozen(DependencyObject const& element)
    {
        if (!element)
        {
            throw dlg_help_utils::exceptions::wide_runtime_error(L"ArgumentNull: element"s);
        }

        return unbox_value<bool>(element.GetValue(IsFrozenProperty()));
    }

    void DataGridFrozenGrid::SetIsFrozen(DependencyObject const& element, bool const value)
    {
        if (!element)
        {
            throw dlg_help_utils::exceptions::wide_runtime_error(L"ArgumentNull: element"s);
        }

        element.SetValue(IsFrozenProperty(), box_value(value));
    }
}
