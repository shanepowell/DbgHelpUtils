#include "pch.h"
#include "OtherCpuInfo.h"

#include "../DbgHelpUtils/system_info_stream.h"

namespace MiniDumpExplorerApp
{
    OtherCpuInfo::OtherCpuInfo(dlg_help_utils::system_info_stream const& system_info)
        : system_info_{system_info}
    {
    }

    uint64_t OtherCpuInfo::ProcessorFeatures0() const
    {
        return system_info_.system_info().Cpu.OtherCpuInfo.ProcessorFeatures[0];
    }

    uint64_t OtherCpuInfo::ProcessorFeatures1() const
    {
        return system_info_.system_info().Cpu.OtherCpuInfo.ProcessorFeatures[1];
    }
}
