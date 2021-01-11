#pragma once
#include "../DbgHelpUtils/mini_dump.h"
#include "../DbgHelpUtils/misc_info_stream.h"

namespace MiniDumpExplorerApp
{
    struct DumpFileStreamMiscInfo : winrt::implements<DumpFileStreamMiscInfo, winrt::MiniDumpExplorer::IDumpFileStreamMiscInfo>
    {
        DumpFileStreamMiscInfo(size_t index, dlg_help_utils::mini_dump const& mini_dump);

        [[nodiscard]] uint64_t Index() const;
        [[nodiscard]] bool IsValid() const;
        [[nodiscard]] uint64_t MiscInfoVersion() const;
        [[nodiscard]] winrt::MiniDumpExplorer::IMiniDumpMiscInfo MiscInfo() const;
        [[nodiscard]] winrt::MiniDumpExplorer::IMiniDumpMiscInfo2 MiscInfo2() const;
        [[nodiscard]] winrt::MiniDumpExplorer::IMiniDumpMiscInfo3 MiscInfo3() const;
        [[nodiscard]] winrt::MiniDumpExplorer::IMiniDumpMiscInfo4 MiscInfo4() const;
        [[nodiscard]] winrt::MiniDumpExplorer::IMiniDumpMiscInfo5 MiscInfo5() const;

    private:
        dlg_help_utils::misc_info_stream misc_info_;
        winrt::MiniDumpExplorer::IMiniDumpMiscInfo info_;
        winrt::MiniDumpExplorer::IMiniDumpMiscInfo2 info2_;
        winrt::MiniDumpExplorer::IMiniDumpMiscInfo3 info3_;
        winrt::MiniDumpExplorer::IMiniDumpMiscInfo4 info4_;
        winrt::MiniDumpExplorer::IMiniDumpMiscInfo5 info5_;
    };
}
