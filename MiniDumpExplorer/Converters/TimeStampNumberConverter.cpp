#include "pch.h"
#include "TimeStampNumberConverter.h"

#include "DbgHelpUtils/time_utils.h"
#include "Helpers/GlobalOptions.h"
#include "Models/DumpFileTimeStamp.h"
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

    Windows::Foundation::IInspectable TimeStampNumberConverter::Convert(Windows::Foundation::IInspectable const& value, [[maybe_unused]] Windows::UI::Xaml::Interop::TypeName const& targetType, [[maybe_unused]] Windows::Foundation::IInspectable const& parameter, [[maybe_unused]] hstring const& language)
    {
        if (!value)
        {
            return value;
        }

        if(auto const dumpFileTimeStamp = value.try_as<DumpFileTimeStamp>();
            dumpFileTimeStamp)
        {
            auto const local_info = dumpFileTimeStamp->DumpFileTimezoneInfo();
            if(dumpFileTimeStamp->HasTimeStamp())
            {
                switch(GlobalOptions::Options().TimeStampLocale())
                {
                case TimeStampLocaleType::Local:
                    return box_value(from_dump_file_to_local_timestamp_string(dumpFileTimeStamp->TimeStamp(), local_info));

                case TimeStampLocaleType::UTC:
                    return box_value(from_dump_file_to_utc_timestamp_string(dumpFileTimeStamp->TimeStamp(), local_info));

                case TimeStampLocaleType::DumpFile:
                    return box_value(to_dump_file_timestamp_string(dumpFileTimeStamp->TimeStamp(), local_info));
                }
            }
            else if(dumpFileTimeStamp->HasSystemTime())
            {
                switch(GlobalOptions::Options().TimeStampLocale())
                {
                case TimeStampLocaleType::Local:
                    return box_value(from_dump_file_to_local_timestamp_string(dumpFileTimeStamp->SystemTime(), local_info));

                case TimeStampLocaleType::UTC:
                    return box_value(from_dump_file_to_utc_timestamp_string(dumpFileTimeStamp->SystemTime(), local_info));

                case TimeStampLocaleType::DumpFile:
                    return box_value(to_dump_file_timestamp_string(dumpFileTimeStamp->SystemTime(), local_info));
                }
            }
            return box_value(hstring(L""));
        }

        enum class TimeStampType
        {
            TimeT,
            FileTime,
        };

        auto type = TimeStampType::TimeT;
        if(auto const parameterString = parameter.try_as<hstring>();
            parameterString.has_value())
        {
            if(parameterString.value() == L"FileTime")
            {
                type = TimeStampType::FileTime;
            }
            else if(parameterString.value() == L"time_t")
            {
                type = TimeStampType::TimeT;
            }
            else
            {
                throw hresult_invalid_argument();  // NOLINT(hicpp-exception-baseclass)
            }
        }

        switch(GlobalOptions::Options().TimeStampLocale())
        {
        case TimeStampLocaleType::Local:
        case TimeStampLocaleType::DumpFile: // there is no dump file information, so assume local locale
            return InspectableUtility::ProcessValueFromInspectable<int64_t, int32_t, uint64_t, uint32_t>([type](auto const v)
            {
                if(v == 0)
                {
                    return box_value(hstring(L""));
                }

                FILETIME ft{};
                switch(type)
                {
                case TimeStampType::TimeT:
                    ft = dlg_help_utils::time_utils::time_t_to_filetime(v);
                    break;
                case TimeStampType::FileTime:
                    ft = dlg_help_utils::time_utils::to_filetime(v);
                    break;
                }

                return box_value(dlg_help_utils::time_utils::to_local_timestamp_string(ft));
            }, value, value);

        case TimeStampLocaleType::UTC:
            return InspectableUtility::ProcessValueFromInspectable<int64_t, int32_t, uint64_t, uint32_t>([type](auto const v)
            {
                if(v == 0)
                {
                    return box_value(hstring(L""));
                }

                FILETIME ft{};
                switch(type)
                {
                case TimeStampType::TimeT:
                    ft = dlg_help_utils::time_utils::time_t_to_filetime(v);
                    break;
                case TimeStampType::FileTime:
                    ft = dlg_help_utils::time_utils::to_filetime(v);
                    break;
                }

                return box_value(dlg_help_utils::time_utils::to_utc_timestamp_string(ft));
            }, value, value);
        }

        throw hresult_invalid_argument();  // NOLINT(hicpp-exception-baseclass)
    }

    Windows::Foundation::IInspectable TimeStampNumberConverter::ConvertBack([[maybe_unused]] Windows::Foundation::IInspectable const& value, [[maybe_unused]] Windows::UI::Xaml::Interop::TypeName const& targetType, [[maybe_unused]] Windows::Foundation::IInspectable const& parameter, [[maybe_unused]] hstring const& language)
    {
        throw hresult_not_implemented();  // NOLINT(hicpp-exception-baseclass)
    }
}
