#pragma once

namespace dlg_help_utils
{
    class misc_info_stream;
}

namespace MiniDumpExplorerApp
{
    struct MiniDumpMiscInfo5 : winrt::implements<MiniDumpMiscInfo5, winrt::MiniDumpExplorer::IMiniDumpMiscInfo5>
    {
        MiniDumpMiscInfo5(dlg_help_utils::misc_info_stream const& misc_info);

        [[nodiscard]] winrt::MiniDumpExplorer::IXStateConfigFeatureMscInfo XStateData() const;
        [[nodiscard]] bool HasProcessCookie() const;
        [[nodiscard]] uint32_t ProcessCookie() const;

    private:
        dlg_help_utils::misc_info_stream const& misc_info_;
        winrt::MiniDumpExplorer::IXStateConfigFeatureMscInfo xstate_;
    };
}
