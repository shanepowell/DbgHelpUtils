#include "pch.h"
#include "X86CpuInfo.h"

#include "../DbgHelpUtils/system_info_stream.h"

namespace MiniDumpExplorerApp
{
    X86CpuInfo::X86CpuInfo(dlg_help_utils::system_info_stream const& system_info)
        : system_info_{system_info}
    {
    }

    uint32_t X86CpuInfo::VendorId0() const
    {
        return system_info_.system_info().Cpu.X86CpuInfo.VendorId[0];
    }

    uint32_t X86CpuInfo::VendorId1() const
    {
        return system_info_.system_info().Cpu.X86CpuInfo.VendorId[1];
    }

    uint32_t X86CpuInfo::VendorId2() const
    {
        return system_info_.system_info().Cpu.X86CpuInfo.VendorId[2];
    }

    uint32_t X86CpuInfo::VersionInformation() const
    {
        return system_info_.system_info().Cpu.X86CpuInfo.VersionInformation;
    }

    uint32_t X86CpuInfo::FeatureInformation() const
    {
        return system_info_.system_info().Cpu.X86CpuInfo.FeatureInformation;
    }

    uint32_t X86CpuInfo::AMDExtendedCpuFeatures() const
    {
        return system_info_.system_info().Cpu.X86CpuInfo.AMDExtendedCpuFeatures;
    }
}
