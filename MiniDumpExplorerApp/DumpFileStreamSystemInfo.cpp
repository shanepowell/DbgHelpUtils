#include "pch.h"
#include "DumpFileStreamSystemInfo.h"

#include "MiniDumpSystemInfo.h"

namespace MiniDumpExplorerApp
{
    DumpFileStreamSystemInfo::DumpFileStreamSystemInfo(size_t index, dlg_help_utils::mini_dump const& mini_dump)
        : system_info_{mini_dump, index}
        , info_{*winrt::make_self<MiniDumpSystemInfo>(system_info_)}
        , csd_version_{system_info_.csd_version()}
        , vendor_id_{system_info_.vendor_id()}
    {
    }

    uint64_t DumpFileStreamSystemInfo::Index() const
    {
        return system_info_.index();
    }

    winrt::MiniDumpExplorer::IMiniDumpSystemInfo DumpFileStreamSystemInfo::SystemInfo() const
    {
        return info_;
    }

    winrt::hstring DumpFileStreamSystemInfo::CsdVersion() const
    {
        return csd_version_;
    }

    winrt::hstring DumpFileStreamSystemInfo::VendorId() const
    {
        return vendor_id_;
    }

    bool DumpFileStreamSystemInfo::IsIntel() const
    {
        return system_info_.is_intel();
    }

    bool DumpFileStreamSystemInfo::IsAmd() const
    {
        return system_info_.is_amd();
    }

    bool DumpFileStreamSystemInfo::IsX86() const
    {
        return system_info_.is_x86();
    }

    bool DumpFileStreamSystemInfo::IsX64() const
    {
        return system_info_.is_x64();
    }

    uint16_t DumpFileStreamSystemInfo::ProcessorModel() const
    {
        return system_info_.processor_model();
    }

    uint16_t DumpFileStreamSystemInfo::ProcessorStepping() const
    {
        return system_info_.processor_stepping();
    }
}
