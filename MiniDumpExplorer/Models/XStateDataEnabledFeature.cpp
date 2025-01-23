#include "pch.h"
#include "XStateDataEnabledFeature.h"

#include "DbgHelpUtils/system_info_utils.h"

#if __has_include("XStateDataEnabledFeature.g.cpp")
// ReSharper disable once CppUnusedIncludeDirective
#include "XStateDataEnabledFeature.g.cpp"  // NOLINT(bugprone-suspicious-include)
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    XStateDataEnabledFeature::XStateDataEnabledFeature()
        : GlobalOptionsNotifyPropertyChangedBase(
            {
                L"Offset"
            },
            {
                L"Size"
            }, 
            {
            },
            {
            },
            {
            },
            {
            })
    {
    }

    void XStateDataEnabledFeature::Set(uint32_t const feature, XSTATE_FEATURE const& xstate_feature)
    {
        feature_ = feature;
        xstate_feature_ = xstate_feature;
        feature_string_ = dlg_help_utils::system_info_utils::xstate_data_feature_to_string(feature_);
    }
}
