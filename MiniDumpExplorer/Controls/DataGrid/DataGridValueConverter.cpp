#include "pch.h"
#include "DataGridValueConverter.h"
#if __has_include("DataGridValueConverter.g.cpp")
// ReSharper disable once CppUnusedIncludeDirective
#include "DataGridValueConverter.g.cpp"  // NOLINT(bugprone-suspicious-include)
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    DataGridValueConverter::DataGridValueConverter() = default;

    Windows::Foundation::IInspectable DataGridValueConverter::Convert(Windows::Foundation::IInspectable const& value, [[maybe_unused]] Windows::UI::Xaml::Interop::TypeName const& targetType, [[maybe_unused]] Windows::Foundation::IInspectable const& parameter, [[maybe_unused]] hstring const& language)
    {
        return value;
    }

    Windows::Foundation::IInspectable DataGridValueConverter::ConvertBack(Windows::Foundation::IInspectable const& value, [[maybe_unused]] Windows::UI::Xaml::Interop::TypeName const& targetType, [[maybe_unused]] Windows::Foundation::IInspectable const& parameter, [[maybe_unused]] hstring const& language)
    {
        if(value.try_as<hstring>())
        {
            if(value.as<hstring>().empty())
            {
                return nullptr;
            }
        }

        return value;
    }
}
