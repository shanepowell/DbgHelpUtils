#include "pch.h"
#include "SizeNumberConverter.h"

#include "DbgHelpUtils/locale_number_formatting.h"
#include "DbgHelpUtils/size_units.h"
#include "Helpers/GlobalOptions.h"
#include "Utility/InspectableUtility.h"

#if __has_include("SizeNumberConverter.g.cpp")
// ReSharper disable once CppUnusedIncludeDirective
#include "SizeNumberConverter.g.cpp"  // NOLINT(bugprone-suspicious-include)
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    SizeNumberConverter::SizeNumberConverter() = default;

    Windows::Foundation::IInspectable SizeNumberConverter::Convert(Windows::Foundation::IInspectable const& value, [[maybe_unused]] Windows::UI::Xaml::Interop::TypeName const& targetType, [[maybe_unused]] Windows::Foundation::IInspectable const& parameter, [[maybe_unused]] hstring const& language)
    {
        if (!value)
        {
            return value;
        }

        auto& options = GlobalOptions::Options();
        auto const sizeFormat = options.SizeFormat();

        switch(options.SizeBase())
        {
        case SizeDisplayNumberBaseType::Base10:
        {
            using namespace dlg_help_utils::size_units::base_10;
            switch(options.SizeNumberDisplayFormat())
            {
            case SizeNumberDisplayFormatType::Auto:
                return InspectableUtility::ProcessValueFromInspectable<int64_t, int32_t, int16_t, int8_t, uint64_t, uint32_t, uint16_t, uint8_t>([sizeFormat](auto const v) { return box_value(to_wstring(bytes{v}, sizeFormat)); }, value);

            case SizeNumberDisplayFormatType::Bytes:
                return InspectableUtility::ProcessValueFromInspectable<int64_t, int32_t, int16_t, int8_t, uint64_t, uint32_t, uint16_t, uint8_t>([sizeFormat](auto const v) { return box_value(to_bytes_wstring(bytes{v}, sizeFormat)); }, value);

            case SizeNumberDisplayFormatType::Kilobytes:
                return InspectableUtility::ProcessValueFromInspectable<int64_t, int32_t, int16_t, int8_t, uint64_t, uint32_t, uint16_t, uint8_t>([sizeFormat](auto const v) { return box_value(to_kilobytes_wstring(bytes{v}, sizeFormat)); }, value);

            case SizeNumberDisplayFormatType::Megabytes:
                return InspectableUtility::ProcessValueFromInspectable<int64_t, int32_t, int16_t, int8_t, uint64_t, uint32_t, uint16_t, uint8_t>([sizeFormat](auto const v) { return box_value(to_megabytes_wstring(bytes{v}, sizeFormat)); }, value);

            case SizeNumberDisplayFormatType::Gigabytes:
                return InspectableUtility::ProcessValueFromInspectable<int64_t, int32_t, int16_t, int8_t, uint64_t, uint32_t, uint16_t, uint8_t>([sizeFormat](auto const v) { return box_value(to_gigabytes_wstring(bytes{v}, sizeFormat)); }, value);

            case SizeNumberDisplayFormatType::Terabytes:
                return InspectableUtility::ProcessValueFromInspectable<int64_t, int32_t, int16_t, int8_t, uint64_t, uint32_t, uint16_t, uint8_t>([sizeFormat](auto const v) { return box_value(to_terabytes_wstring(bytes{v}, sizeFormat)); }, value);

            case SizeNumberDisplayFormatType::Petabytes:
                return InspectableUtility::ProcessValueFromInspectable<int64_t, int32_t, int16_t, int8_t, uint64_t, uint32_t, uint16_t, uint8_t>([sizeFormat](auto const v) { return box_value(to_petabytes_wstring(bytes{v}, sizeFormat)); }, value);

            case SizeNumberDisplayFormatType::Exabytes:
                return InspectableUtility::ProcessValueFromInspectable<int64_t, int32_t, int16_t, int8_t, uint64_t, uint32_t, uint16_t, uint8_t>([sizeFormat](auto const v) { return box_value(to_exabytes_wstring(bytes{v}, sizeFormat)); }, value);

            case SizeNumberDisplayFormatType::Raw:
                return InspectableUtility::ProcessValueFromInspectable<int64_t, int32_t, int16_t, int8_t, uint64_t, uint32_t, uint16_t, uint8_t>([](auto const v) { return box_value(locale_formatting::to_wstring(v)); }, value);

            default:
                break;
            }
            break;
        }
        case SizeDisplayNumberBaseType::Base16:
        {
            using namespace dlg_help_utils::size_units::base_16;
            switch(options.SizeNumberDisplayFormat())
            {
            case SizeNumberDisplayFormatType::Auto:
                return InspectableUtility::ProcessValueFromInspectable<int64_t, int32_t, int16_t, int8_t, uint64_t, uint32_t, uint16_t, uint8_t>([sizeFormat](auto const v) { return box_value(to_wstring(bytes{v}, sizeFormat)); }, value);

            case SizeNumberDisplayFormatType::Bytes:
                return InspectableUtility::ProcessValueFromInspectable<int64_t, int32_t, int16_t, int8_t, uint64_t, uint32_t, uint16_t, uint8_t>([sizeFormat](auto const v) { return box_value(to_bytes_wstring(bytes{v}, sizeFormat)); }, value);

            case SizeNumberDisplayFormatType::Kilobytes:
                return InspectableUtility::ProcessValueFromInspectable<int64_t, int32_t, int16_t, int8_t, uint64_t, uint32_t, uint16_t, uint8_t>([sizeFormat](auto const v) { return box_value(to_kilobytes_wstring(bytes{v}, sizeFormat)); }, value);

            case SizeNumberDisplayFormatType::Megabytes:
                return InspectableUtility::ProcessValueFromInspectable<int64_t, int32_t, int16_t, int8_t, uint64_t, uint32_t, uint16_t, uint8_t>([sizeFormat](auto const v) { return box_value(to_megabytes_wstring(bytes{v}, sizeFormat)); }, value);

            case SizeNumberDisplayFormatType::Gigabytes:
                return InspectableUtility::ProcessValueFromInspectable<int64_t, int32_t, int16_t, int8_t, uint64_t, uint32_t, uint16_t, uint8_t>([sizeFormat](auto const v) { return box_value(to_gigabytes_wstring(bytes{v}, sizeFormat)); }, value);

            case SizeNumberDisplayFormatType::Terabytes:
                return InspectableUtility::ProcessValueFromInspectable<int64_t, int32_t, int16_t, int8_t, uint64_t, uint32_t, uint16_t, uint8_t>([sizeFormat](auto const v) { return box_value(to_terabytes_wstring(bytes{v}, sizeFormat)); }, value);

            case SizeNumberDisplayFormatType::Petabytes:
                return InspectableUtility::ProcessValueFromInspectable<int64_t, int32_t, int16_t, int8_t, uint64_t, uint32_t, uint16_t, uint8_t>([sizeFormat](auto const v) { return box_value(to_petabytes_wstring(bytes{v}, sizeFormat)); }, value);

            case SizeNumberDisplayFormatType::Exabytes:
                return InspectableUtility::ProcessValueFromInspectable<int64_t, int32_t, int16_t, int8_t, uint64_t, uint32_t, uint16_t, uint8_t>([sizeFormat](auto const v) { return box_value(to_exabytes_wstring(bytes{v}, sizeFormat)); }, value);

            case SizeNumberDisplayFormatType::Raw:
                return InspectableUtility::ProcessValueFromInspectable<int64_t, int32_t, int16_t, int8_t, uint64_t, uint32_t, uint16_t, uint8_t>([](auto const v) { return box_value(locale_formatting::to_wstring(v)); }, value);

            default:
                break;
            }
            break;
        }

        default:
            break;
        }

        return value;
    }

    Windows::Foundation::IInspectable SizeNumberConverter::ConvertBack(Windows::Foundation::IInspectable const& value, [[maybe_unused]] Windows::UI::Xaml::Interop::TypeName const& targetType, [[maybe_unused]] Windows::Foundation::IInspectable const& parameter, [[maybe_unused]] hstring const& language)
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
        switch(GlobalOptions::Options().SizeBase())
        {
        case SizeDisplayNumberBaseType::Base10:
        {
            using namespace dlg_help_utils::size_units::base_10;
            return box_value(from_wstring(std::wstring(str.begin(), str.end())).count());
        }
        case SizeDisplayNumberBaseType::Base16:
        {
            using namespace dlg_help_utils::size_units::base_16;
            return box_value(from_wstring(std::wstring(str.begin(), str.end())).count());
        }

        default:
            break;
        }

        return value;
    }
}
