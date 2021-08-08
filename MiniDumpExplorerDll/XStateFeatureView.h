#pragma once

#include "XStateFeatureView.g.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct XStateFeatureView : XStateFeatureViewT<XStateFeatureView>
    {
        XStateFeatureView();

        IXStateFeature Feature() const;
        void Feature(IXStateFeature value);

    private:
        IXStateFeature feature_;
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct XStateFeatureView : XStateFeatureViewT<XStateFeatureView, implementation::XStateFeatureView>
    {
    };
}
