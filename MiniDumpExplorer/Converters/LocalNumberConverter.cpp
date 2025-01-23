#include "pch.h"
#include "LocalNumberConverter.h"

#include "DbgHelpUtils/locale_number_formatting.h"
#include "Helpers/GlobalOptions.h"
#include "Utility/InspectableUtility.h"

#if __has_include("LocalNumberConverter.g.cpp")
// ReSharper disable once CppUnusedIncludeDirective
#include "LocalNumberConverter.g.cpp"  // NOLINT(bugprone-suspicious-include)
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    LocalNumberConverter::LocalNumberConverter() = default;

    Windows::Foundation::IInspectable LocalNumberConverter::Convert(Windows::Foundation::IInspectable const& value, [[maybe_unused]] Windows::UI::Xaml::Interop::TypeName const& targetType, [[maybe_unused]] Windows::Foundation::IInspectable const& parameter, [[maybe_unused]] hstring const& language)
    {
        if (!value)
        {
            return value;
        }

        return InspectableUtility::ProcessValueFromInspectable<int64_t, int32_t, int16_t, int8_t, uint64_t, uint32_t, uint16_t, uint8_t, float, double>([]<typename T0>(T0 const v) 
        {
            return box_value(locale_formatting::to_wstring(v, locale_formatting::floating_point_formatting_t{std::is_floating_point_v<T0>}, locale_formatting::floating_point_formatting_scientific_t{GlobalOptions::Options().FloatingPointScientificDisplayFormat()}));
        }, value);
    }

    Windows::Foundation::IInspectable LocalNumberConverter::ConvertBack(Windows::Foundation::IInspectable const& value, [[maybe_unused]] Windows::UI::Xaml::Interop::TypeName const& targetType, [[maybe_unused]] Windows::Foundation::IInspectable const& parameter, [[maybe_unused]] hstring const& language)
    {
        if (!value)
        {
            return value;
        }

        if (!value.try_as<hstring>())
        {
            return value;
        }

        auto const str = value.as<hstring>();
        return box_value(std::stoull(std::wstring(str.begin(), str.end()), nullptr, 10));
    }
}
