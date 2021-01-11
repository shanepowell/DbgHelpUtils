#pragma once

namespace dlg_help_utils
{
    class misc_info_stream;
}

namespace MiniDumpExplorerApp
{
    struct MiniDumpMiscInfo4 : winrt::implements<MiniDumpMiscInfo4, winrt::MiniDumpExplorer::IMiniDumpMiscInfo4>
    {
        MiniDumpMiscInfo4(dlg_help_utils::misc_info_stream const& misc_info);

        [[nodiscard]] bool HasBuildString() const;
        [[nodiscard]] winrt::hstring BuildString() const;
        [[nodiscard]] winrt::hstring DbgBldStr() const;

    private:
        dlg_help_utils::misc_info_stream const& misc_info_;
        winrt::hstring build_string_;
        winrt::hstring debug_build_string_;
    };
}
