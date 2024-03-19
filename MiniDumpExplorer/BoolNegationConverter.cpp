#include "pch.h"
#include "BoolNegationConverter.h"
#if __has_include("BoolNegationConverter.g.cpp")
// ReSharper disable once CppUnusedIncludeDirective
#include "BoolNegationConverter.g.cpp"  // NOLINT(bugprone-suspicious-include)
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    BoolNegationConverter::BoolNegationConverter() = default;

    Windows::Foundation::IInspectable BoolNegationConverter::Convert(Windows::Foundation::IInspectable const& value, [[maybe_unused]] Windows::UI::Xaml::Interop::TypeName const& targetType, [[maybe_unused]] Windows::Foundation::IInspectable const& parameter, [[maybe_unused]] hstring const& language)
    {
        if (!value)
        {
            return value;
        }

        return box_value(!unbox_value_or(value, false));
    }

    Windows::Foundation::IInspectable BoolNegationConverter::ConvertBack(Windows::Foundation::IInspectable const& value, [[maybe_unused]] Windows::UI::Xaml::Interop::TypeName const& targetType, [[maybe_unused]] Windows::Foundation::IInspectable const& parameter, [[maybe_unused]] hstring const& language)
    {
        if (!value)
        {
            return value;
        }

        return box_value(!unbox_value_or(value, false));
    }
}
