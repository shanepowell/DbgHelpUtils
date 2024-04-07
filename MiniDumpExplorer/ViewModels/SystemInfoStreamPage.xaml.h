#pragma once

#include "SystemInfoStreamPage.g.h"

#include "DbgHelpUtils/system_info_stream.h"
#include "Models/GlobalOptionsNotifyPropertyChangedBase.h"
#include "Models/MiniDumpPageBase.h"

namespace dlg_help_utils
{
    class mini_dump;
}

namespace winrt::MiniDumpExplorer::implementation
{
    struct MiniDumpPage;

    struct SystemInfoStreamPage : SystemInfoStreamPageT<SystemInfoStreamPage>, GlobalOptionsNotifyPropertyChangedBase<SystemInfoStreamPage>, MiniDumpPageBase<SystemInfoStreamPage>
    {
        SystemInfoStreamPage();

        // ReSharper disable once CppHidingFunction
        void InitializeComponent();

        // ReSharper disable once CppHidingFunction
        void OnNavigatedTo(Microsoft::UI::Xaml::Navigation::NavigationEventArgs const& e)
        {
            MiniDumpPageBase::OnNavigatedTo(e);
        }

        uint16_t ProcessorArchitecture() const { return system_info_stream_.system_info().ProcessorArchitecture; }
        hstring ProcessorArchitectureString() const { return processorArchitectureString_; }
        uint16_t ProcessorLevel() const { return system_info_stream_.system_info().ProcessorLevel; }
        uint16_t ProcessorRevision() const { return system_info_stream_.system_info().ProcessorRevision; }
        uint16_t ProcessorModel() const { return system_info_stream_.processor_model(); }
        uint16_t ProcessorStepping() const { return system_info_stream_.processor_stepping(); }
        uint8_t NumberOfProcessors() const  { return system_info_stream_.system_info().NumberOfProcessors; }
        uint8_t ProductType() const { return system_info_stream_.system_info().ProductType; }
        hstring ProductTypeString() const { return productTypeString_; }
        uint32_t OsMajorVersion() const { return system_info_stream_.system_info().MajorVersion; }
        uint32_t OsMinorVersion() const { return system_info_stream_.system_info().MinorVersion; }
        uint32_t OsBuildNumber() const { return system_info_stream_.system_info().BuildNumber; }
        hstring OsVersionMajorMinorBuildString() const { return osVersionMajorMinorBuildString_; }
        hstring OsVersion() const { return osVersion_; }
        uint32_t OsPlatformId() const { return system_info_stream_.system_info().PlatformId; }
        hstring OsPlatformIdString() const { return osPlatformIdString_; }
        hstring CsdVersion() const { return csdVersion_; }
        uint16_t SuiteMask() const { return system_info_stream_.system_info().SuiteMask; }
        Windows::Foundation::Collections::IObservableVector<hstring> SuiteMaskList() const { return suiteMaskList_; }
        uint16_t Reserved2() const { return system_info_stream_.system_info().Reserved2; }
        hstring X86CpuInfoVendorId() const { return x86CpuInfoVendorId_; }
        uint32_t X86CpuInfoVersionInformation() const { return system_info_stream_.system_info().Cpu.X86CpuInfo.VersionInformation; }
        uint32_t X86CpuInfoFeatureInformation() const { return system_info_stream_.system_info().Cpu.X86CpuInfo.FeatureInformation; }
        uint32_t X86CpuInfoAMDExtendedCpuFeatures() const { return system_info_stream_.system_info().Cpu.X86CpuInfo.AMDExtendedCpuFeatures; }
        uint64_t OtherCpuInfoProcessorFeatures1() const { return system_info_stream_.system_info().Cpu.OtherCpuInfo.ProcessorFeatures[0]; }
        uint64_t OtherCpuInfoProcessorFeatures2() const { return system_info_stream_.system_info().Cpu.OtherCpuInfo.ProcessorFeatures[1]; }


    private:
        void SetupFlyoutMenus();
        void MiniDumpLoaded(MiniDumpExplorer::MiniDumpPageParameters const& parameters) override;

    private:
        dlg_help_utils::system_info_stream system_info_stream_{};
        hstring processorArchitectureString_;
        hstring productTypeString_;
        hstring osVersionMajorMinorBuildString_;
        hstring osVersion_;
        hstring osPlatformIdString_;
        hstring csdVersion_;
        hstring x86CpuInfoVendorId_;
        Windows::Foundation::Collections::IObservableVector<hstring> suiteMaskList_{single_threaded_observable_vector<hstring>()};
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct SystemInfoStreamPage : SystemInfoStreamPageT<SystemInfoStreamPage, implementation::SystemInfoStreamPage>
    {
    };
}
