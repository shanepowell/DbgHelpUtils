#pragma once

#include "VersionInfoView.g.h"

namespace winrt::MiniDumpExplorer::implementation
{
    struct VersionInfoView : VersionInfoViewT<VersionInfoView>
    {
        VersionInfoView();

        IVsFixedFileInfo VersionInfo();
        void VersionInfo(IVsFixedFileInfo value);

    private:
        IVsFixedFileInfo version_info_;
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct VersionInfoView : VersionInfoViewT<VersionInfoView, implementation::VersionInfoView>
    {
    };
}
