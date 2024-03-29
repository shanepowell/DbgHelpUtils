#pragma once

#include "MiniDumpPageParameters.g.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct MiniDumpPage;

    struct MiniDumpPageParameters : MiniDumpPageParametersT<MiniDumpPageParameters>
    {
        MiniDumpPageParameters(MiniDumpExplorer::MiniDumpPage const& miniDump, hstring const& navigationItemTag, uint32_t streamIndex);

        MiniDumpExplorer::MiniDumpPage MiniDump() const { return miniDump_; }
        hstring NavigationItemTag() const { return navigationItemTag_; }
        uint32_t StreamIndex() const { return streamIndex_; }

    private:
        MiniDumpExplorer::MiniDumpPage miniDump_;
        hstring navigationItemTag_;
        uint32_t streamIndex_;
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct MiniDumpPageParameters : MiniDumpPageParametersT<MiniDumpPageParameters, implementation::MiniDumpPageParameters>
    {
    };
}
