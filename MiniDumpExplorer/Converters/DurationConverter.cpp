#include "pch.h"
#include "DurationConverter.h"

#include "DbgHelpUtils/locale_number_formatting.h"
#include "DbgHelpUtils/string_compare.h"
#include "DbgHelpUtils/time_units.h"
#include "Helpers/GlobalOptions.h"
#include "Utility/InspectableUtility.h"

#if __has_include("DurationConverter.g.cpp")
// ReSharper disable once CppUnusedIncludeDirective
#include "DurationConverter.g.cpp"  // NOLINT(bugprone-suspicious-include)
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{


    DurationConverter::DurationConverter()
    {
    }

    Windows::Foundation::IInspectable DurationConverter::Convert(Windows::Foundation::IInspectable const& value, [[maybe_unused]] Windows::UI::Xaml::Interop::TypeName const& targetType, Windows::Foundation::IInspectable const& parameter, [[maybe_unused]] hstring const& language)
    {
        if (!value)
        {
            return value;
        }

        std::wstring parameterValue{unbox_value_or<hstring>(parameter, L"")};
        return InspectableUtility::ProcessValueFromInspectable<uint64_t, uint32_t, uint16_t, uint8_t, int64_t, int32_t, int16_t, int8_t>([&parameterValue](auto const v)
        {
            using namespace dlg_help_utils::time_units;
            filetime_nanoseconds duration;

            auto durationFormat = GlobalOptions::Options().DurationFormat();

            if(parameterValue.empty() || dlg_help_utils::string_utils::iequals(parameterValue, L"seconds"))
            {
                duration = std::chrono::seconds{v};
                if(durationFormat == DurationFormatType::Raw)
                {
                    durationFormat = DurationFormatType::Seconds;
                }
            }
            else if(dlg_help_utils::string_utils::iequals(parameterValue, L"minutes"))
            {
                duration = std::chrono::minutes{v};
                if(durationFormat == DurationFormatType::Raw)
                {
                    durationFormat = DurationFormatType::Minutes;
                }
            }
            else if(dlg_help_utils::string_utils::iequals(parameterValue, L"milliseconds"))
            {
                duration = std::chrono::milliseconds{v};
                if(durationFormat == DurationFormatType::Raw)
                {
                    durationFormat = DurationFormatType::Milliseconds;
                }
            }
            else if(dlg_help_utils::string_utils::iequals(parameterValue, L"filetime"))
            {
                duration = filetime_nanoseconds{v};
                /*
                if(durationFormat == DurationFormatType::Raw)
                {
                    durationFormat = DurationFormatType::FileTime;
                }
                */
            }
            else if(dlg_help_utils::string_utils::iequals(parameterValue, L"hours"))
            {
                duration = std::chrono::hours{v};
                if(durationFormat == DurationFormatType::Raw)
                {
                    durationFormat = DurationFormatType::Hours;
                }
            }
            else if(dlg_help_utils::string_utils::iequals(parameterValue, L"days"))
            {
                duration = days{v};
                if(durationFormat == DurationFormatType::Raw)
                {
                    durationFormat = DurationFormatType::Days;
                }
            }
            else if(dlg_help_utils::string_utils::iequals(parameterValue, L"weeks"))
            {
                duration = weeks{v};
                if(durationFormat == DurationFormatType::Raw)
                {
                    durationFormat = DurationFormatType::Weeks;
                }
            }
            else if(dlg_help_utils::string_utils::iequals(parameterValue, L"months"))
            {
                duration = months{v};
                if(durationFormat == DurationFormatType::Raw)
                {
                    durationFormat = DurationFormatType::Months;
                }
            }
            else if(dlg_help_utils::string_utils::iequals(parameterValue, L"years"))
            {
                duration = years{v};
                if(durationFormat == DurationFormatType::Raw)
                {
                    durationFormat = DurationFormatType::Years;
                }
            }
            else
            {
                throw hresult_invalid_argument{L"Invalid parameter value."};  // NOLINT(hicpp-exception-baseclass)
            }

            switch(durationFormat)
            {
            case DurationFormatType::Auto:
                break;

            case DurationFormatType::Raw:
                return box_value(locale_formatting::to_wstring(duration.count()));

            case DurationFormatType::TimeSpan:
                return box_value(to_timespan_wstring(duration));
            case DurationFormatType::FileTime:
                return box_value(to_nanoseconds_wstring(duration));
            case DurationFormatType::Milliseconds:
                return box_value(to_milliseconds_wstring(duration));
            case DurationFormatType::Seconds:
                return box_value(to_seconds_wstring(duration));
            case DurationFormatType::Minutes:
                return box_value(to_minutes_wstring(duration));
            case DurationFormatType::Hours:
                return box_value(to_hours_wstring(duration));
            case DurationFormatType::Days:
                return box_value(to_days_wstring(duration));
            case DurationFormatType::Weeks:
                return box_value(to_weeks_wstring(duration));
            case DurationFormatType::Months:
                return box_value(to_months_wstring(duration));
            case DurationFormatType::Years:
                return box_value(to_years_wstring(duration));
            }

            return box_value(dlg_help_utils::time_units::to_wstring(duration));
        }, value, value);
    }

    Windows::Foundation::IInspectable DurationConverter::ConvertBack(Windows::Foundation::IInspectable const& value, [[maybe_unused]] Windows::UI::Xaml::Interop::TypeName const& targetType, [[maybe_unused]] Windows::Foundation::IInspectable const& parameter, [[maybe_unused]] hstring const& language)
    {
        return value;
    }
}
