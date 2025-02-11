#include "pch.h"
#include "HexNumberConverter.h"

#include <ranges>

#include "DbgHelpUtils/int128.h"
#include "DbgHelpUtils/locale_number_formatting.h"
#include "DbgHelpUtils/m128a_utils.h"
#include "DbgHelpUtils/stream_hex_dump.h"
#include "DbgHelpUtils/string_compare.h"
#include "DbgHelpUtils/string_conversation.h"
#include "Helpers/GlobalOptions.h"
#include "Utility/InspectableUtility.h"

#include "Models/M128A.h"

#if __has_include("HexNumberConverter.g.cpp")
// ReSharper disable once CppUnusedIncludeDirective
#include "HexNumberConverter.g.cpp"  // NOLINT(bugprone-suspicious-include)
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;
using namespace dlg_help_utils;

namespace winrt::MiniDumpExplorer::implementation
{
    HexNumberConverter::HexNumberConverter() = default;

    Windows::Foundation::IInspectable HexNumberConverter::Convert(Windows::Foundation::IInspectable const& value, [[maybe_unused]] Windows::UI::Xaml::Interop::TypeName const& targetType, Windows::Foundation::IInspectable const& parameter, [[maybe_unused]] hstring const& language)
    {
        if (!value)
        {
            return value;
        }
        std::wstring parameterValue{unbox_value_or<hstring>(parameter, L"0")};
        std::streamsize width{0};
        bool hex_full = false;
        bool is_uint128 = GlobalOptions::Options().M128AViewDisplayFormat() == M128AViewType::UInt128;
        for (auto param : std::views::split(parameterValue, L';'))
        {
            if (auto const paramValue = std::wstring_view(param.begin(), param.end());
                string_utils::iequals(paramValue, L"full"))
            {
                hex_full = true;
            }
            else if (string_utils::iequals(paramValue, L"uint128"))
            {
                is_uint128 = true;
            }
            else if (string_utils::iequals(paramValue, L"int128"))
            {
                is_uint128 = false;
            }
            else
            {
                auto widthParameter = string_conversation::wstring_to_utf8(paramValue);
                std::from_chars(widthParameter.data(), widthParameter.data() + widthParameter.size(), width);
            }
        }

        if (auto m128a = value.try_as<M128A>();
            m128a)
        {
            switch(GlobalOptions::Options().NumberDisplayFormat())
            {
            case NumberDisplayFormatType::Hexadecimal:
                return box_value(hex_full ? stream_hex_dump::to_hex_full(m128a->Get()) : stream_hex_dump::to_hex(m128a->Get(), width));

            case NumberDisplayFormatType::Decimal:
                if (is_uint128)
                {
                    return box_value(locale_formatting::to_wstring(m128a_utils::to_uint128(m128a->Get())));
                }

                return box_value(locale_formatting::to_wstring(m128a_utils::to_int128(m128a->Get())));

            default:
                break;
            }

            return value;
        }

        switch(GlobalOptions::Options().NumberDisplayFormat())
        {
        case NumberDisplayFormatType::Hexadecimal:
            return InspectableUtility::ProcessValueFromInspectable<uint64_t, int64_t, uint32_t, int32_t, uint16_t, int16_t, uint8_t, int8_t>([hex_full, width](auto const v)
            {
                return box_value(hex_full ? stream_hex_dump::to_hex_full(v) : stream_hex_dump::to_hex(v, width));
            }, value);

        case NumberDisplayFormatType::Decimal:
            return InspectableUtility::ProcessValueFromInspectable<uint64_t, int64_t, uint32_t, int32_t, uint16_t, int16_t, uint8_t, int8_t>([](auto const v) { return box_value(locale_formatting::to_wstring(v)); }, value);

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

        return box_value(std::stoull(std::wstring(str.begin(), str.end()), nullptr, 10));
    }
}
