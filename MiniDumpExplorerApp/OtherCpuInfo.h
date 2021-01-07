#pragma once

namespace dlg_help_utils
{
    class system_info_stream;
}

namespace MiniDumpExplorerApp
{
    struct OtherCpuInfo : winrt::implements<OtherCpuInfo, winrt::MiniDumpExplorer::IOtherCpuInfo>
    {
        OtherCpuInfo(dlg_help_utils::system_info_stream const& system_info);

        [[nodiscard]] uint64_t ProcessorFeatures0() const;
        [[nodiscard]] uint64_t ProcessorFeatures1() const;

    private:
        dlg_help_utils::system_info_stream const& system_info_;
    };
}

