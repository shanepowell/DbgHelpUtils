#include "pch.h"
#include "TimeStampNumberConverter.h"

#include "DbgHelpUtils/time_utils.h"
#include "Utility/ConverterTools.h"
#include "Utility/InspectableUtility.h"

#if __has_include("TimeStampNumberConverter.g.cpp")
// ReSharper disable once CppUnusedIncludeDirective
#include "TimeStampNumberConverter.g.cpp"  // NOLINT(bugprone-suspicious-include)
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    TimeStampNumberConverter::TimeStampNumberConverter() = default;

    Windows::Foundation::IInspectable TimeStampNumberConverter::Convert(Windows::Foundation::IInspectable const& value, [[maybe_unused]] Windows::UI::Xaml::Interop::TypeName const& targetType, Windows::Foundation::IInspectable const& parameter, [[maybe_unused]] hstring const& language)
    {
        if (!value)
        {
            return value;
        }

        if (ConverterTools::TryParseBool(parameter))
        {
            return InspectableUtility::ProcessValueFromInspectable<uint64_t, uint32_t, uint16_t, uint8_t>([](auto const v) { return box_value(dlg_help_utils::time_utils::to_utc_time(v)); }, value, value);
        }

        return InspectableUtility::ProcessValueFromInspectable<uint64_t, uint32_t, uint16_t, uint8_t>([](auto const v) { return box_value(dlg_help_utils::time_utils::to_local_time(v)); }, value, value);
    }

    Windows::Foundation::IInspectable TimeStampNumberConverter::ConvertBack(Windows::Foundation::IInspectable const& value, [[maybe_unused]] Windows::UI::Xaml::Interop::TypeName const& targetType, [[maybe_unused]] Windows::Foundation::IInspectable const& parameter, [[maybe_unused]] hstring const& language)
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
