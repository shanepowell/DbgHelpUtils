#include "pch.h"
#include "SystemInfoStreamPage.xaml.h"

#include "DbgHelpUtils/mini_dump.h"
#include "DbgHelpUtils/system_info_utils.h"
#include "Helpers/UIHelper.h"

#if __has_include("SystemInfoStreamPage.g.cpp")
// ReSharper disable once CppUnusedIncludeDirective
#include "SystemInfoStreamPage.g.cpp" // NOLINT(bugprone-suspicious-include)
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    struct MiniDumpPageParameters;

    SystemInfoStreamPage::SystemInfoStreamPage()
        : GlobalOptionsNotifyPropertyChangedBase(
            {
                L"ProcessorLevel",
                L"ProcessorModel",
                L"ProcessorStepping",
                L"ProcessorRevision",
                L"Reserved2",
                L"SuiteMask",
                L"X86CpuInfoVersionInformation",
                L"X86CpuInfoFeatureInformation",
                L"X86CpuInfoAMDExtendedCpuFeatures",
                L"OtherCpuInfoProcessorFeatures1",
                L"OtherCpuInfoProcessorFeatures2"
            },
            {
            }, 
            {
            },
            {
            })
    {
    }

    void SystemInfoStreamPage::InitializeComponent()
    {
        SystemInfoStreamPageT::InitializeComponent();
        SetupFlyoutMenus();
    }

    void SystemInfoStreamPage::SetupFlyoutMenus()
    {
        UIHelper::CreateStandardHexNumberMenu(processorLevel(), 
            processorModel(), 
            processorStepping(), 
            processorRevision(), 
            suiteMask(), 
            reserved2(), 
            x86CpuInfoVersionInformation(), 
            x86CpuInfoFeatureInformation(), 
            x86CpuInfoAMDExtendedCpuFeatures(), 
            otherCpuInfoProcessorFeatures1(),
            otherCpuInfoProcessorFeatures2());
    }

    void SystemInfoStreamPage::MiniDumpLoaded(MiniDumpExplorer::MiniDumpPageParameters const& parameters)
    {
        auto const miniDumpPage = parameters.MiniDump().as<MiniDumpPage>();
        auto const& miniDump = miniDumpPage->MiniDump();

        system_info_stream_ = dlg_help_utils::system_info_stream{miniDump, parameters.StreamIndex()};

        if (!system_info_stream_.found())
        {
            logger::Log().LogMessage(log_level::error, std::format("failed to load mini dump system info stream index:[{}]", parameters.StreamIndex()));
            return;
        }

        auto const& info = system_info_stream_.system_info();
        processorArchitectureString_ = dlg_help_utils::system_info_utils::processor_architecture_to_string(info.ProcessorArchitecture);
        productTypeString_ = dlg_help_utils::system_info_utils::product_type_to_string(info.ProductType);
        osVersionMajorMinorBuildString_ = std::format(L"{}.{}.{}", info.MajorVersion, info.MinorVersion, info.BuildNumber);
        osVersion_ = dlg_help_utils::system_info_utils::windows_version_to_string(info.MajorVersion, info.MinorVersion, info.BuildNumber, info.ProductType, info.ProcessorArchitecture, info.SuiteMask);
        osPlatformIdString_ = dlg_help_utils::system_info_utils::platform_id_to_string(info.PlatformId);
        csdVersion_ = system_info_stream_.csd_version();
        for (auto const& value : dlg_help_utils::system_info_utils::suite_mask_to_strings(info.SuiteMask))
        {
            suiteMaskList_.Append(value);
        }

        if(system_info_stream_.has_x86_cpu_info())  // NOLINT(bugprone-branch-clone)
        {
            x86CpuInfoVendorId_ = system_info_stream_.vendor_id();
        }
        else
        {
            otherCpuInfoProcessorFeaturesList_.Clear();
            for (auto const& value : dlg_help_utils::system_info_utils::other_processor_features_to_strings(info.Cpu.OtherCpuInfo.ProcessorFeatures[0], info.Cpu.OtherCpuInfo.ProcessorFeatures[1]))
            {
                otherCpuInfoProcessorFeaturesList_.Append(value);
            }
        }

        RaisePropertyChanged(L"ProcessorArchitecture");
        RaisePropertyChanged(L"ProcessorArchitectureString");
        RaisePropertyChanged(L"ProcessorLevel");
        RaisePropertyChanged(L"ProcessorRevision");
        RaisePropertyChanged(L"ProcessorModel");
        RaisePropertyChanged(L"ProcessorStepping");
        RaisePropertyChanged(L"NumberOfProcessors");
        RaisePropertyChanged(L"ProductType");
        RaisePropertyChanged(L"ProductTypeString");
        RaisePropertyChanged(L"OsMajorVersion");
        RaisePropertyChanged(L"OsMinorVersion");
        RaisePropertyChanged(L"OsBuildNumber");
        RaisePropertyChanged(L"OsVersionMajorMinorBuildString");
        RaisePropertyChanged(L"OsVersion");
        RaisePropertyChanged(L"OsPlatformId");
        RaisePropertyChanged(L"OsPlatformIdString");
        RaisePropertyChanged(L"CsdVersion");
        RaisePropertyChanged(L"SuiteMask");
        RaisePropertyChanged(L"Reserved2");
        RaisePropertyChanged(L"HasX86CpuInfo");

        if(system_info_stream_.has_x86_cpu_info())
        {
            RaisePropertyChanged(L"X86CpuInfoVendorId");
            RaisePropertyChanged(L"X86CpuInfoVersionInformation");
            RaisePropertyChanged(L"X86CpuInfoFeatureInformation");
            RaisePropertyChanged(L"X86CpuInfoAMDExtendedCpuFeatures");
        }
        else
        {
            RaisePropertyChanged(L"OtherCpuInfoProcessorFeatures1");
            RaisePropertyChanged(L"OtherCpuInfoProcessorFeatures2");
            RaisePropertyChanged(L"OtherCpuInfoProcessorFeaturesList");
        }
    }
}
