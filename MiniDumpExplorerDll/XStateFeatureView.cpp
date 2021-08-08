#include "pch.h"
#include "XStateFeatureView.h"
#if __has_include("XStateFeatureView.g.cpp")
#include "XStateFeatureView.g.cpp"
#endif

using namespace winrt;
using namespace Windows::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    XStateFeatureView::XStateFeatureView()
    {
        InitializeComponent();
    }

    IXStateFeature XStateFeatureView::Feature() const
    {
        return feature_;
    }

    void XStateFeatureView::Feature(IXStateFeature const value)
    {
        feature_ = value;
    }
}
