#include "pch.h"
#include "CpuInformation.h"

#include "X86CpuInfo.h"
#include "OtherCpuInfo.h"

namespace MiniDumpExplorerApp
{
    CpuInformation::CpuInformation(dlg_help_utils::system_info_stream const& system_info)
        : system_info_{system_info}
        , x86_cpu_info_{*winrt::make_self<MiniDumpExplorerApp::X86CpuInfo>(system_info_)}
        , other_cpu_info_{*winrt::make_self<MiniDumpExplorerApp::OtherCpuInfo>(system_info_)}
    {
    }

    winrt::MiniDumpExplorer::IX86CpuInfo CpuInformation::X86CpuInfo() const
    {
        return x86_cpu_info_;
    }

    winrt::MiniDumpExplorer::IOtherCpuInfo CpuInformation::OtherCpuInfo() const
    {
        return other_cpu_info_;
    }
}
