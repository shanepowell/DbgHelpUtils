#pragma once

#include "../DbgHelpUtils/mini_dump.h"

namespace MiniDumpExplorerApp
{
    struct XStateFeature : winrt::implements<XStateFeature, winrt::MiniDumpExplorer::IXStateFeature>
    {
        XStateFeature(XSTATE_FEATURE const& xstate_feature, std::wstring_view name);

        winrt::hstring Name() const;
        uint32_t Offset() const;
        uint32_t Size() const;

    private:
        XSTATE_FEATURE const& xstate_feature_;
        winrt::hstring name_;
    };
}
