#pragma once

#include "XStateDataEnabledFeature.g.h"

#include "GlobalOptionsNotifyPropertyChangedBase.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct XStateDataEnabledFeature : XStateDataEnabledFeatureT<XStateDataEnabledFeature>, GlobalOptionsNotifyPropertyChangedBase<XStateDataEnabledFeature>
    {
        XStateDataEnabledFeature();

        void Set(uint32_t feature, XSTATE_FEATURE const& xstate_feature);

        uint32_t Feature() const { return feature_; }
        hstring FeatureString() const { return feature_string_; }
        uint32_t Offset() const { return xstate_feature_.Offset; }
        uint32_t Size() const { return xstate_feature_.Size; }

    private:
        XSTATE_FEATURE xstate_feature_{};
        uint32_t feature_{};
        hstring feature_string_{};
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct XStateDataEnabledFeature : XStateDataEnabledFeatureT<XStateDataEnabledFeature, implementation::XStateDataEnabledFeature>
    {
    };
}
