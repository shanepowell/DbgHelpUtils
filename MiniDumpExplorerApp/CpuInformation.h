#pragma once

namespace dlg_help_utils
{
    class system_info_stream;
}

namespace MiniDumpExplorerApp
{
    struct CpuInformation : winrt::implements<CpuInformation, winrt::MiniDumpExplorer::ICpuInformation>
    {
        CpuInformation(dlg_help_utils::system_info_stream const& system_info);

        [[nodiscard]] winrt::MiniDumpExplorer::IX86CpuInfo X86CpuInfo() const;
        [[nodiscard]] winrt::MiniDumpExplorer::IOtherCpuInfo OtherCpuInfo() const;

    private:
        dlg_help_utils::system_info_stream const& system_info_;
        winrt::MiniDumpExplorer::IX86CpuInfo x86_cpu_info_;
        winrt::MiniDumpExplorer::IOtherCpuInfo other_cpu_info_;
    };
}
