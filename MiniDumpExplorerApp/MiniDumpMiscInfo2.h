#pragma once

namespace dlg_help_utils
{
    class misc_info_stream;
}

namespace MiniDumpExplorerApp
{
    struct MiniDumpMiscInfo2 : winrt::implements<MiniDumpMiscInfo2, winrt::MiniDumpExplorer::IMiniDumpMiscInfo2>
    {
        MiniDumpMiscInfo2(dlg_help_utils::misc_info_stream const& misc_info);

        [[nodiscard]] bool HasProcessPowerInfo() const;
        [[nodiscard]] uint32_t ProcessorMaxMhz() const;
        [[nodiscard]] uint32_t ProcessorCurrentMhz() const;
        [[nodiscard]] uint32_t ProcessorMhzLimit() const;
        [[nodiscard]] uint32_t ProcessorMaxIdleState() const;
        [[nodiscard]] uint32_t ProcessorCurrentIdleState() const;

    private:
        dlg_help_utils::misc_info_stream const& misc_info_;
    };
}
