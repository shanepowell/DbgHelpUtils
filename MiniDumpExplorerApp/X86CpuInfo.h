#pragma once

namespace dlg_help_utils
{
    class system_info_stream;
}

namespace MiniDumpExplorerApp
{
    struct X86CpuInfo : winrt::implements<X86CpuInfo, winrt::MiniDumpExplorer::IX86CpuInfo>
    {
        X86CpuInfo(dlg_help_utils::system_info_stream const& system_info);

        [[nodiscard]] uint32_t VendorId0() const;
        [[nodiscard]] uint32_t VendorId1() const;
        [[nodiscard]] uint32_t VendorId2() const;
        
        [[nodiscard]] uint32_t VersionInformation() const;
        [[nodiscard]] uint32_t FeatureInformation() const;
        
        [[nodiscard]] uint32_t AMDExtendedCpuFeatures() const;

    private:
        dlg_help_utils::system_info_stream const& system_info_;
    };
}
