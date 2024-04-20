#include "pch.h"
#include "TimeZoneInformation.h"

#include "DbgHelpUtils/system_time_utils.h"
#include "DbgHelpUtils/time_utils.h"

#if __has_include("TimeZoneInformation.g.cpp")
// ReSharper disable once CppUnusedIncludeDirective
#include "TimeZoneInformation.g.cpp"  // NOLINT(bugprone-suspicious-include)
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    TimeZoneInformation::TimeZoneInformation()
        : GlobalOptionsNotifyPropertyChangedBase(
            {
            }, 
            {
            },
            {
            },
            {
                L"Bias",
                L"StandardBias",
                L"DaylightBias",
            })
    {
    }

    void TimeZoneInformation::Set(TIME_ZONE_INFORMATION const& time_zone_information)
    {

        time_zone_information_ = time_zone_information;
        standard_name_ = time_zone_information.StandardName;
        standard_date_ = dlg_help_utils::system_time_utils::to_raw_wstring(time_zone_information.StandardDate);
        daylight_name_ = time_zone_information.DaylightName;
        daylight_date_ = dlg_help_utils::system_time_utils::to_raw_wstring(time_zone_information.DaylightDate);

        RaisePropertyChanged(L"Bias");
        RaisePropertyChanged(L"StandardName");
        RaisePropertyChanged(L"StandardDateString");
        RaisePropertyChanged(L"StandardBias");
        RaisePropertyChanged(L"DaylightName");
        RaisePropertyChanged(L"DaylightDateString");
        RaisePropertyChanged(L"DaylightBias");
    }
}
