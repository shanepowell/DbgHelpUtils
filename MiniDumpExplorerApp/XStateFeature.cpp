#include "pch.h"
#include "XStateFeature.h"

namespace MiniDumpExplorerApp
{
    XStateFeature::XStateFeature(XSTATE_FEATURE const& xstate_feature, std::wstring_view name)
        : xstate_feature_{xstate_feature}
        , name_{name}
    {
    }

    winrt::hstring XStateFeature::Name() const
    {
        return name_;
    }

    uint32_t XStateFeature::Offset() const
    {
        return xstate_feature_.Offset;
    }

    uint32_t XStateFeature::Size() const
    {
        return xstate_feature_.Size;
    }
}
