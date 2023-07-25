#include "pch.h"
#include "HexNumberConverter.h"

#include "DbgHelpUtils/stream_hex_dump.h"
#include "Helpers/GlobalOptions.h"

#if __has_include("HexNumberConverter.g.cpp")
// ReSharper disable once CppUnusedIncludeDirective
#include "HexNumberConverter.g.cpp"  // NOLINT(bugprone-suspicious-include)
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;
using namespace dlg_help_utils::stream_hex_dump;

namespace winrt::MiniDumpExplorer::implementation
{
    HexNumberConverter::HexNumberConverter() = default;

    Windows::Foundation::IInspectable HexNumberConverter::Convert(Windows::Foundation::IInspectable const& value, [[maybe_unused]] Windows::UI::Xaml::Interop::TypeName const& targetType, [[maybe_unused]] Windows::Foundation::IInspectable const& parameter, [[maybe_unused]] hstring const& language)
    {
        if (!value)
        {
            return value;
        }

        switch(GlobalOptions::Options().NumberDisplayFormat())
        {
        case NumberDisplayFormat::Hexadecimal:
            if (value.try_as<uint64_t>())
            {
                return box_value(to_hex(value.as<uint64_t>()));
            }
            if (value.try_as<uint32_t>())
            {
                return box_value(to_hex(value.as<uint32_t>()));
            }
            if (value.try_as<uint16_t>())
            {
                return box_value(to_hex(value.as<uint16_t>()));
            }
            if (value.try_as<uint8_t>())
            {
                return box_value(to_hex(value.as<uint8_t>()));
            }
            break;

        case NumberDisplayFormat::Decimal:
            if (value.try_as<uint64_t>())
            {
                return box_value(std::to_wstring(value.as<uint64_t>()));
            }
            if (value.try_as<uint32_t>())
            {
                return box_value(std::to_wstring(value.as<uint32_t>()));
            }
            if (value.try_as<uint16_t>())
            {
                return box_value(std::to_wstring(value.as<uint16_t>()));
            }
            if (value.try_as<uint8_t>())
            {
                return box_value(std::to_wstring(value.as<uint8_t>()));
            }
            break;

        default:
            break;
        }

        return value;
    }

    Windows::Foundation::IInspectable HexNumberConverter::ConvertBack(Windows::Foundation::IInspectable const& value, [[maybe_unused]] Windows::UI::Xaml::Interop::TypeName const& targetType, [[maybe_unused]] Windows::Foundation::IInspectable const& parameter, [[maybe_unused]] hstring const& language)
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
        if (str.size() > 2 && str[0] == '0' && (str[1] == 'x' || str[1] == 'X'))
        {
            return box_value(std::stoull(std::wstring(str.begin() + 2, str.end()), nullptr, 16));
        }

        return box_value(std::stoull(std::wstring(str.begin() + 2, str.end()), nullptr, 10));
    }
}
