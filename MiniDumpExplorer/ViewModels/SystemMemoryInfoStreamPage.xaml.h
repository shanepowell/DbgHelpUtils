#pragma once

#include "SystemMemoryInfoStreamPage.g.h"

#include "DbgHelpUtils/system_memory_info_stream.h"
#include "Models/GlobalOptionsNotifyPropertyChangedBase.h"
#include "Models/MiniDumpPageBase.h"

namespace dlg_help_utils
{
    class mini_dump;
}

namespace winrt::MiniDumpExplorer::implementation
{
    struct MiniDumpSystemBasicInformation;
    struct MiniDumpSystemFileCacheInformation;
    struct MiniDumpSystemBasicPerformanceInformation;
    struct MinidumpSystemPerformanceInformation;

    struct SystemMemoryInfoStreamPage : SystemMemoryInfoStreamPageT<SystemMemoryInfoStreamPage>, GlobalOptionsNotifyPropertyChangedBase<SystemMemoryInfoStreamPage>, MiniDumpPageBase<SystemMemoryInfoStreamPage>
    {
        SystemMemoryInfoStreamPage();

        // ReSharper disable once CppHidingFunction
        void InitializeComponent();

        // ReSharper disable once CppHidingFunction
        void OnNavigatedTo(Microsoft::UI::Xaml::Navigation::NavigationEventArgs const& e)
        {
            MiniDumpPageBase::OnNavigatedTo(e);
        }

        uint32_t Index() const { return static_cast<uint32_t>(system_memory_info_stream_.index()); }
        uint16_t Revision() const { return system_memory_info_stream_.system_memory_misc_info().Revision; }
        uint16_t Flags() const { return system_memory_info_stream_.system_memory_misc_info().Flags; }
        Windows::Foundation::Collections::IObservableVector<hstring> FlagsList() const { return flagsList_; }

        MiniDumpExplorer::MiniDumpSystemBasicInformation BasicInfo() const { return basicInfo_; }
        MiniDumpExplorer::MiniDumpSystemFileCacheInformation FileCacheInfo() const { return fileCacheInfo_; }
        MiniDumpExplorer::MiniDumpSystemBasicPerformanceInformation BasicPerfInfo() const { return basicPerfInfo_; }
        MiniDumpExplorer::MinidumpSystemPerformanceInformation PerfInfo() const { return perfInfo_; }


    private:
        void SetupFlyoutMenus();
        void MiniDumpLoaded(MiniDumpExplorer::MiniDumpPageParameters const& parameters) override;

    private:
        dlg_help_utils::system_memory_info_stream system_memory_info_stream_{};
        Windows::Foundation::Collections::IObservableVector<hstring> flagsList_{single_threaded_observable_vector<hstring>()};
        MiniDumpExplorer::MiniDumpSystemBasicInformation basicInfo_{};
        MiniDumpExplorer::MiniDumpSystemFileCacheInformation fileCacheInfo_{};
        MiniDumpExplorer::MiniDumpSystemBasicPerformanceInformation basicPerfInfo_{};
        MiniDumpExplorer::MinidumpSystemPerformanceInformation perfInfo_{};
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct SystemMemoryInfoStreamPage : SystemMemoryInfoStreamPageT<SystemMemoryInfoStreamPage, implementation::SystemMemoryInfoStreamPage>
    {
    };
}
