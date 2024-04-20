#pragma once

#include "MiscInfoStreamPage.g.h"

#include "DbgHelpUtils/misc_info_stream.h"
#include "Models/GlobalOptionsNotifyPropertyChangedBase.h"
#include "Models/MiniDumpPageBase.h"

namespace dlg_help_utils
{
    class mini_dump;
}

namespace winrt::MiniDumpExplorer::implementation
{
    struct DumpFileTimeStamp;
    struct TimeZoneInformation;
    struct XStateDataEnabledFeaturesDataSource;

    struct MiscInfoStreamPage : MiscInfoStreamPageT<MiscInfoStreamPage>, GlobalOptionsNotifyPropertyChangedBase<MiscInfoStreamPage>, MiniDumpPageBase<MiscInfoStreamPage>
    {
        MiscInfoStreamPage();

        // ReSharper disable once CppHidingFunction
        void InitializeComponent();

        // ReSharper disable once CppHidingFunction
        void OnNavigatedTo(Microsoft::UI::Xaml::Navigation::NavigationEventArgs const& e)
        {
            MiniDumpPageBase::OnNavigatedTo(e);
        }

        uint32_t Flags() const { return misc_info_stream_.misc_info().Flags1; }
        Windows::Foundation::Collections::IObservableVector<hstring> FlagsList() const { return flagsList_; }
        
        bool HasProcessId() const { return Flags() & MINIDUMP_MISC1_PROCESS_ID; }
        uint32_t ProcessId() const { return misc_info_stream_.misc_info().ProcessId; }

        bool HasProcessTimes() const { return Flags() & MINIDUMP_MISC1_PROCESS_TIMES; }
        MiniDumpExplorer::DumpFileTimeStamp ProcessCreateTime() const { return processCreateTime_; }
        uint32_t ProcessUserTime() const { return misc_info_stream_.misc_info().ProcessUserTime; }
        uint32_t ProcessKernelTime() const { return misc_info_stream_.misc_info().ProcessKernelTime; }

        bool HasProcessorPowerInfo() const { return misc_info_stream_.misc_info_version() >= 2 && Flags() & MINIDUMP_MISC1_PROCESSOR_POWER_INFO; }
        uint32_t ProcessorMaxMhz() const { return HasProcessorPowerInfo() ? misc_info_stream_.misc_info_2().ProcessorMaxMhz : 0; }
        uint32_t ProcessorCurrentMhz() const { return HasProcessorPowerInfo() ? misc_info_stream_.misc_info_2().ProcessorCurrentMhz : 0; }
        uint32_t ProcessorMhzLimit() const { return HasProcessorPowerInfo() ? misc_info_stream_.misc_info_2().ProcessorMhzLimit : 0; }
        uint32_t ProcessorMaxIdleState() const { return HasProcessorPowerInfo() ? misc_info_stream_.misc_info_2().ProcessorMaxIdleState : 0; }
        uint32_t ProcessorCurrentIdleState() const { return HasProcessorPowerInfo() ? misc_info_stream_.misc_info_2().ProcessorMaxIdleState : 0; }

        bool HasProcessIntegrityInfo() const { return misc_info_stream_.misc_info_version() >= 3 && Flags() & MINIDUMP_MISC3_PROCESS_INTEGRITY; }
        uint32_t ProcessIntegrityLevel() const { return HasProcessIntegrityInfo() ? misc_info_stream_.misc_info_3().ProcessIntegrityLevel : 0; }
        hstring ProcessIntegrityLevelString() const { return HasProcessIntegrityInfo() ? processIntegrityLevelString_ : hstring{}; }

        bool HasProcessExecuteFlags() const { return misc_info_stream_.misc_info_version() >= 3 && Flags() & MINIDUMP_MISC3_PROCESS_EXECUTE_FLAGS; }
        uint32_t ProcessExecuteFlags() const { return HasProcessExecuteFlags() ? misc_info_stream_.misc_info_3().ProcessExecuteFlags : 0; }
        Windows::Foundation::Collections::IObservableVector<hstring> ProcessExecuteFlagsList() const { return processExecuteFlagsList_; }

        bool HasProtectedProcess() const { return misc_info_stream_.misc_info_version() >= 3 && Flags() & MINIDUMP_MISC3_PROTECTED_PROCESS; }
        uint32_t ProtectedProcess() const { return HasProtectedProcess() ? misc_info_stream_.misc_info_3().ProtectedProcess : 0; }

        bool HasTimeZone() const { return misc_info_stream_.misc_info_version() >= 3 && Flags() & MINIDUMP_MISC3_TIMEZONE; }
        uint32_t TimeZoneId() const { return HasTimeZone() ? misc_info_stream_.misc_info_3().TimeZoneId : 0; }
        hstring TimeZoneIdString() const { return timezone_id_string_; }
        MiniDumpExplorer::TimeZoneInformation TimeZone() const { return timeZone_; }

        bool HasBuildString() const { return misc_info_stream_.misc_info_version() >= 4 && Flags() & MINIDUMP_MISC4_BUILDSTRING; }
        hstring BuildString() const { return build_string_; }
        hstring DbgBldStr() const { return dbg_bld_str_; }

        bool HasProcessCookie() const { return misc_info_stream_.misc_info_version() >= 5 && Flags() & MINIDUMP_MISC5_PROCESS_COOKIE; }
        uint32_t ProcessCookie() const { return HasProcessCookie() ? misc_info_stream_.misc_info_5().ProcessCookie : 0; }

        bool HasXStateData() const { return misc_info_stream_.misc_info_version() >= 5 && misc_info_stream_.misc_info_5().XStateData.SizeOfInfo > 0; }
        uint32_t XStateDataSize() const { return HasXStateData() ? misc_info_stream_.misc_info_5().XStateData.SizeOfInfo : 0; }
        bool IsXStateConfigFeatureMscInfo() const { return HasXStateData() && misc_info_stream_.misc_info_5().XStateData.SizeOfInfo == sizeof(XSTATE_CONFIG_FEATURE_MSC_INFO); }
        uint32_t XStateDataContextSize() const { return IsXStateConfigFeatureMscInfo() ? misc_info_stream_.misc_info_5().XStateData.ContextSize : 0; }
        uint64_t XStateDataEnabledFeatures() const { return IsXStateConfigFeatureMscInfo() ? misc_info_stream_.misc_info_5().XStateData.EnabledFeatures : 0; }
        IDataGridDataSource XStateDataEnabledFeaturesSource() const { return xStateDataEnabledFeaturesDataSource_; }

    private:
        void SetupFlyoutMenus();
        void MiniDumpLoaded(MiniDumpExplorer::MiniDumpPageParameters const& parameters) override;

    private:
        dlg_help_utils::misc_info_stream misc_info_stream_{};
        hstring timezone_id_string_;
        MiniDumpExplorer::TimeZoneInformation timeZone_{};
        MiniDumpExplorer::DumpFileTimeStamp processCreateTime_{};
        hstring processIntegrityLevelString_;
        hstring build_string_;
        hstring dbg_bld_str_;
        Windows::Foundation::Collections::IObservableVector<hstring> flagsList_{single_threaded_observable_vector<hstring>()};
        Windows::Foundation::Collections::IObservableVector<hstring> processExecuteFlagsList_{single_threaded_observable_vector<hstring>()};
        MiniDumpExplorer::XStateDataEnabledFeaturesDataSource xStateDataEnabledFeaturesDataSource_{};
    };
}

namespace winrt::MiniDumpExplorer::factory_implementation
{
    struct MiscInfoStreamPage : MiscInfoStreamPageT<MiscInfoStreamPage, implementation::MiscInfoStreamPage>
    {
    };
}
