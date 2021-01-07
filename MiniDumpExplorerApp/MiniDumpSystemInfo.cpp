#include "pch.h"
#include "MiniDumpSystemInfo.h"

#include "../DbgHelpUtils/system_info_stream.h"
#include "CpuInformation.h"
#include "../DbgHelpUtils/system_info_utils.h"

namespace MiniDumpExplorerApp
{
    MiniDumpSystemInfo::MiniDumpSystemInfo(dlg_help_utils::system_info_stream const& system_info)
        : system_info_{system_info}
        , processor_architecture_{dlg_help_utils::system_info_utils::processor_architecture_to_string(ProcessorArchitecture())}
        , product_type_{dlg_help_utils::system_info_utils::product_type_to_string(ProductType())}
        , windows_os_version_{dlg_help_utils::system_info_utils::windows_version_to_string(MajorVersion(), MinorVersion(), BuildNumber(),
                                                                         ProductType(), ProcessorArchitecture(),
                                                                         SuiteMask())}
        , platform_{dlg_help_utils::system_info_utils::platform_id_to_string(PlatformId())}
        , cpu_info_{*winrt::make_self<CpuInformation>(system_info_)}
    {
        std::wostringstream ss;
        ss << MajorVersion() << L'.' << MinorVersion() << L'.' << BuildNumber();
        os_version_ = std::move(ss).str();

        for (auto const& value : dlg_help_utils::system_info_utils::suite_mask_to_strings(SuiteMask()))
        {
            suites_.Append(winrt::hstring{value});
        }
    }

    uint16_t MiniDumpSystemInfo::ProcessorArchitecture() const
    {
        return system_info_.system_info().ProcessorArchitecture;
    }

    winrt::hstring MiniDumpSystemInfo::ProcessorArchitectureString() const
    {
        return processor_architecture_;
    }

    uint16_t MiniDumpSystemInfo::ProcessorLevel() const
    {
        return system_info_.system_info().ProcessorLevel;
    }

    uint16_t MiniDumpSystemInfo::ProcessorRevision() const
    {
        return system_info_.system_info().ProcessorRevision;
    }

    uint8_t MiniDumpSystemInfo::NumberOfProcessors() const
    {
        return system_info_.system_info().NumberOfProcessors;
    }

    uint8_t MiniDumpSystemInfo::ProductType() const
    {
        return system_info_.system_info().ProductType;
    }

    winrt::hstring MiniDumpSystemInfo::ProductTypeString() const
    {
        return product_type_;
    }

    uint32_t MiniDumpSystemInfo::MajorVersion() const
    {
        return system_info_.system_info().MajorVersion;
    }

    uint32_t MiniDumpSystemInfo::MinorVersion() const
    {
        return system_info_.system_info().MinorVersion;
    }

    uint32_t MiniDumpSystemInfo::BuildNumber() const
    {
        return system_info_.system_info().BuildNumber;
    }

    winrt::hstring MiniDumpSystemInfo::OsVersionString() const
    {
        return os_version_;
    }

    winrt::hstring MiniDumpSystemInfo::WindowsOsVersionString() const
    {
        return windows_os_version_;
    }

    uint32_t MiniDumpSystemInfo::PlatformId() const
    {
        return system_info_.system_info().PlatformId;
    }

    winrt::hstring MiniDumpSystemInfo::PlatformString() const
    {
        return platform_;
    }

    uint32_t MiniDumpSystemInfo::CSDVersionRva() const
    {
        return system_info_.system_info().CSDVersionRva;
    }

    uint16_t MiniDumpSystemInfo::SuiteMask() const
    {
        return system_info_.system_info().SuiteMask;

    }

    uint16_t MiniDumpSystemInfo::Reserved2() const
    {
        return system_info_.system_info().Reserved2;
    }

    winrt::Windows::Foundation::Collections::IObservableVector<winrt::hstring> MiniDumpSystemInfo::Suites() const
    {
        return suites_;
    }

    winrt::MiniDumpExplorer::ICpuInformation MiniDumpSystemInfo::Cpu() const
    {
        return cpu_info_;
    }
}
