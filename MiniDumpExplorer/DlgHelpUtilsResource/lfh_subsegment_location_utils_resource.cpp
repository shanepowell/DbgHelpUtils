#include "pch.h"
#include "DbgHelpUtils/lfh_subsegment_location_utils.h"

#include <winrt/Microsoft.Windows.ApplicationModel.Resources.h>

#include "DbgHelpUtils/flags_string_utils.h"

using namespace std::string_view_literals;

namespace
{

    std::unordered_map<dlg_help_utils::lfh_subsegment_location_utils::location, std::wstring> const& location_descriptions()
    {
        using dlg_help_utils::lfh_subsegment_location_utils::location;
        winrt::Microsoft::Windows::ApplicationModel::Resources::ResourceManager const rm{};
        static std::unordered_map<location, std::wstring> const location_descriptions =
        {
            {location::available_subsegment_list,   std::wstring{rm.MainResourceMap().GetValue(L"Resources/AvailableSubsegmentList").ValueAsString()}},
            {location::full_subsegment_list,        std::wstring{rm.MainResourceMap().GetValue(L"Resources/FullSubsegmentList").ValueAsString()}},
            {location::retiring_subsegment_list,    std::wstring{rm.MainResourceMap().GetValue(L"Resources/RetiringSubsegmentList").ValueAsString()}},
        };

        return location_descriptions;
    }
}

namespace dlg_help_utils::lfh_subsegment_location_utils
{
    std::wstring dump_location_to_string(location const value)
    {
        return flags_string_utils::generate_enum_string(value, location_descriptions());
    }
}
