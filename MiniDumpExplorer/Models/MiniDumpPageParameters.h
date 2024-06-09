#pragma once

#include "MiniDumpPageParameters.g.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct MiniDumpPage;

    struct MiniDumpPageParameters : MiniDumpPageParametersT<MiniDumpPageParameters>
    {
        MiniDumpPageParameters(MiniDumpExplorer::MiniDumpPage const& miniDump, hstring const& findItemTag, hstring const& navigationItemTag, uint32_t streamIndex, uint32_t streamSubType, uint32_t streamSubIndex);

        MiniDumpExplorer::MiniDumpPage MiniDump() const { return miniDump_; }
        hstring FindItemTag() const { return findItemTag_; }
        hstring NavigationItemTag() const { return navigationItemTag_; }
        uint32_t StreamIndex() const { return streamIndex_; }
        uint32_t StreamSubType() const { return streamSubType_; }
        uint32_t StreamSubIndex() const { return streamSubIndex_; }

    private:
        MiniDumpExplorer::MiniDumpPage miniDump_;
        hstring findItemTag_;
        hstring navigationItemTag_;
        uint32_t streamIndex_;
        uint32_t streamSubType_;
        uint32_t streamSubIndex_;
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct MiniDumpPageParameters : MiniDumpPageParametersT<MiniDumpPageParameters, implementation::MiniDumpPageParameters>
    {
    };
}
