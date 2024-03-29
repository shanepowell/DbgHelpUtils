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
        case SizeDisplayNumberBase::Base10:
        {
            using namespace dlg_help_utils::size_units::base_10;
            switch(options.SizeNumberDisplayFormat())
            {
            case SizeNumberDisplayFormat::Auto:
                return InspectableUtility::ProcessValueFromInspectable<uint64_t, uint32_t, uint16_t, uint8_t>([sizeFormat](auto const v) { return box_value(to_wstring(bytes{v}, sizeFormat)); }, value, value);

            case SizeNumberDisplayFormat::Bytes:
                return InspectableUtility::ProcessValueFromInspectable<uint64_t, uint32_t, uint16_t, uint8_t>([sizeFormat](auto const v) { return box_value(to_bytes_wstring(bytes{v}, sizeFormat)); }, value, value);

            case SizeNumberDisplayFormat::Kilobytes:
                return InspectableUtility::ProcessValueFromInspectable<uint64_t, uint32_t, uint16_t, uint8_t>([sizeFormat](auto const v) { return box_value(to_kilobytes_wstring(bytes{v}, sizeFormat)); }, value, value);

            case SizeNumberDisplayFormat::Megabytes:
                return InspectableUtility::ProcessValueFromInspectable<uint64_t, uint32_t, uint16_t, uint8_t>([sizeFormat](auto const v) { return box_value(to_megabytes_wstring(bytes{v}, sizeFormat)); }, value, value);

            case SizeNumberDisplayFormat::Gigabytes:
                return InspectableUtility::ProcessValueFromInspectable<uint64_t, uint32_t, uint16_t, uint8_t>([sizeFormat](auto const v) { return box_value(to_gigabytes_wstring(bytes{v}, sizeFormat)); }, value, value);

            case SizeNumberDisplayFormat::Terabytes:
                return InspectableUtility::ProcessValueFromInspectable<uint64_t, uint32_t, uint16_t, uint8_t>([sizeFormat](auto const v) { return box_value(to_terabytes_wstring(bytes{v}, sizeFormat)); }, value, value);

            case SizeNumberDisplayFormat::Petabytes:
                return InspectableUtility::ProcessValueFromInspectable<uint64_t, uint32_t, uint16_t, uint8_t>([sizeFormat](auto const v) { return box_value(to_petabytes_wstring(bytes{v}, sizeFormat)); }, value, value);

            case SizeNumberDisplayFormat::Exabytes:
                return InspectableUtility::ProcessValueFromInspectable<uint64_t, uint32_t, uint16_t, uint8_t>([sizeFormat](auto const v) { return box_value(to_exabytes_wstring(bytes{v}, sizeFormat)); }, value, value);

            case SizeNumberDisplayFormat::Raw:
                return InspectableUtility::ProcessValueFromInspectable<uint64_t, uint32_t, uint16_t, uint8_t>([](auto const v) { return box_value(locale_formatting::to_wstring(v)); }, value, value);

            default:
                break;
            }
            break;
        }
        case SizeDisplayNumberBase::Base16:
        {
            using namespace dlg_help_utils::size_units::base_16;
            switch(options.SizeNumberDisplayFormat())
            {
            case SizeNumberDisplayFormat::Auto:
                return InspectableUtility::ProcessValueFromInspectable<uint64_t, uint32_t, uint16_t, uint8_t>([sizeFormat](auto const v) { return box_value(to_wstring(bytes{v}, sizeFormat)); }, value, value);

            case SizeNumberDisplayFormat::Bytes:
                return InspectableUtility::ProcessValueFromInspectable<uint64_t, uint32_t, uint16_t, uint8_t>([sizeFormat](auto const v) { return box_value(to_bytes_wstring(bytes{v}, sizeFormat)); }, value, value);

            case SizeNumberDisplayFormat::Kilobytes:
                return InspectableUtility::ProcessValueFromInspectable<uint64_t, uint32_t, uint16_t, uint8_t>([sizeFormat](auto const v) { return box_value(to_kilobytes_wstring(bytes{v}, sizeFormat)); }, value, value);

            case SizeNumberDisplayFormat::Megabytes:
                return InspectableUtility::ProcessValueFromInspectable<uint64_t, uint32_t, uint16_t, uint8_t>([sizeFormat](auto const v) { return box_value(to_megabytes_wstring(bytes{v}, sizeFormat)); }, value, value);

            case SizeNumberDisplayFormat::Gigabytes:
                return InspectableUtility::ProcessValueFromInspectable<uint64_t, uint32_t, uint16_t, uint8_t>([sizeFormat](auto const v) { return box_value(to_gigabytes_wstring(bytes{v}, sizeFormat)); }, value, value);

            case SizeNumberDisplayFormat::Terabytes:
                return InspectableUtility::ProcessValueFromInspectable<uint64_t, uint32_t, uint16_t, uint8_t>([sizeFormat](auto const v) { return box_value(to_terabytes_wstring(bytes{v}, sizeFormat)); }, value, value);

            case SizeNumberDisplayFormat::Petabytes:
                return InspectableUtility::ProcessValueFromInspectable<uint64_t, uint32_t, uint16_t, uint8_t>([sizeFormat](auto const v) { return box_value(to_petabytes_wstring(bytes{v}, sizeFormat)); }, value, value);

            case SizeNumberDisplayFormat::Exabytes:
                return InspectableUtility::ProcessValueFromInspectable<uint64_t, uint32_t, uint16_t, uint8_t>([sizeFormat](auto const v) { return box_value(to_exabytes_wstring(bytes{v}, sizeFormat)); }, value, value);

            case SizeNumberDisplayFormat::Raw:
                return InspectableUtility::ProcessValueFromInspectable<uint64_t, uint32_t, uint16_t, uint8_t>([](auto const v) { return box_value(locale_formatting::to_wstring(v)); }, value, value);

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
        case SizeDisplayNumberBase::Base10:
        {
            using namespace dlg_help_utils::size_units::base_10;
            return box_value(from_wstring(std::wstring(str.begin(), str.end())).count());
        }
        case SizeDisplayNumberBase::Base16:
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
