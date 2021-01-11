#pragma once

namespace dlg_help_utils
{
    class misc_info_stream;
}

namespace MiniDumpExplorerApp
{
    struct MiniDumpMiscInfo : winrt::implements<MiniDumpMiscInfo, winrt::MiniDumpExplorer::IMiniDumpMiscInfo>
    {
        MiniDumpMiscInfo(dlg_help_utils::misc_info_stream const& misc_info);

        [[nodiscard]] uint32_t Flags1() const;
        [[nodiscard]] bool HasProcessId() const;
        [[nodiscard]] uint32_t ProcessId() const;
        [[nodiscard]] bool HasProcessTimes() const;
        [[nodiscard]] winrt::clock::time_point ProcessCreateTime() const;
        [[nodiscard]] uint32_t ProcessUserTime() const;
        [[nodiscard]] uint32_t ProcessKernelTime() const;

    private:
        dlg_help_utils::misc_info_stream const& misc_info_;
    };
}
