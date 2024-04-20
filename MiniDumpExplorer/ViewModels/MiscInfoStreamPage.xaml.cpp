#include "pch.h"
#include "MiscInfoStreamPage.xaml.h"

#include "DbgHelpUtils/system_info_utils.h"
#include "Helpers/UIHelper.h"
#include "Models/DumpFileTimeStamp.h"
#include "Models/TimeZoneInformation.h"
#include "Models/XStateDataEnabledFeaturesDataSource.h"

#if __has_include("MiscInfoStreamPage.g.cpp")
// ReSharper disable once CppUnusedIncludeDirective
#include "MiscInfoStreamPage.g.cpp" // NOLINT(bugprone-suspicious-include)
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    struct MiniDumpPageParameters;

    MiscInfoStreamPage::MiscInfoStreamPage()
        : GlobalOptionsNotifyPropertyChangedBase(
            {
                L"Flags",
                L"ProcessId",
                L"ProcessorMaxIdleState",
                L"ProcessorCurrentIdleState",
                L"ProcessIntegrityLevel",
                L"ProcessExecuteFlags",
                L"ProtectedProcess",
                L"TimeZoneId",
                L"ProcessCookie",
            },
            {
                L"XStateDataSize",
                L"XStateDataContextSize",
            }, 
            {
                L"ProcessCreateTime",
            },
            {
                L"ProcessUserTime",
                L"ProcessKernelTime",
            })
    {
    }

    void MiscInfoStreamPage::InitializeComponent()
    {
        MiscInfoStreamPageT::InitializeComponent();
        SetupFlyoutMenus();
    }

    void MiscInfoStreamPage::SetupFlyoutMenus()
    {
        UIHelper::CreateStandardHexNumberMenu(flags(),
            processId(),
            processorMaxIdleState(),
            processorCurrentIdleState(),
            processIntegrityLevel(),
            processExecuteFlags(),
            protectedProcess(),
            timeZoneId(),
            processCookie());

        UIHelper::CreateStandardSizeNumberMenu(xStateDataSize(),
            xStateDataContextSize());

        UIHelper::CreateStandardTimeStampMenu(processCreateTime());

        UIHelper::CreateStandardDurationMenu(processUserTime(),
            processKernelTime(),
            bias(),
            standardBias(),
            daylightBias());
    }

    void MiscInfoStreamPage::MiniDumpLoaded(MiniDumpExplorer::MiniDumpPageParameters const& parameters)
    {
        auto const miniDumpPage = parameters.MiniDump().as<MiniDumpPage>();
        auto const& miniDump = miniDumpPage->MiniDump();

        misc_info_stream_ = dlg_help_utils::misc_info_stream{miniDump, parameters.StreamIndex()};

        if (!misc_info_stream_.found())
        {
            logger::Log().LogMessage(log_level::error, std::format("failed to load mini dump misc info stream index:[{}]", parameters.StreamIndex()));
            return;
        }

        if (!misc_info_stream_.is_valid())
        {
            logger::Log().LogMessage(log_level::error, std::format("failed to load valid mini dump misc info stream index:[{}]", parameters.StreamIndex()));
            return;
        }

        if(HasProcessTimes())
        {
            processCreateTime_.as<DumpFileTimeStamp>()->Set(misc_info_stream_.misc_info().ProcessCreateTime, dlg_help_utils::misc_info_stream::get_dump_file_timezone_info(miniDump));
        }

        if(HasTimeZone())
        {
            timezone_id_string_ = dlg_help_utils::system_info_utils::time_zone_id_to_string(TimeZoneId());
            timeZone_.as<TimeZoneInformation>()->Set(misc_info_stream_.misc_info_3().TimeZone);
        }

        if(HasProcessIntegrityInfo())
        {
            processIntegrityLevelString_ = dlg_help_utils::system_info_utils::process_integrity_level_to_string(ProcessIntegrityLevel());
        }

        if(HasBuildString())
        {
            auto const& info_4 = misc_info_stream_.misc_info_4();
            build_string_ = info_4.BuildString;
            dbg_bld_str_ = info_4.DbgBldStr;
        }

        flagsList_.Clear();
        for(auto const& flag : dlg_help_utils::system_info_utils::misc_info_flags_to_strings(Flags()))
        {
            flagsList_.Append(flag);
        }

        processExecuteFlagsList_.Clear();
        if(HasProcessExecuteFlags())
        {
            for(auto const& flag : dlg_help_utils::system_info_utils::process_execute_flags_to_strings(ProcessExecuteFlags()))
            {
                processExecuteFlagsList_.Append(flag);
            }
        }

        if(IsXStateConfigFeatureMscInfo())
        {
            xStateDataEnabledFeaturesDataSource_.as<XStateDataEnabledFeaturesDataSource>()->SetXStateData(misc_info_stream_.misc_info_5().XStateData);
        }

        RaisePropertyChanged(L"Flags");
        RaisePropertyChanged(L"HasProcessId");
        RaisePropertyChanged(L"ProcessId");
        RaisePropertyChanged(L"HasProcessTimes");
        RaisePropertyChanged(L"ProcessCreateTime");
        RaisePropertyChanged(L"ProcessUserTime");
        RaisePropertyChanged(L"ProcessKernelTime");
        RaisePropertyChanged(L"HasProcessorPowerInfo");
        RaisePropertyChanged(L"ProcessorMaxMhz");
        RaisePropertyChanged(L"ProcessorCurrentMhz");
        RaisePropertyChanged(L"ProcessorMhzLimit");
        RaisePropertyChanged(L"ProcessorMaxIdleState");
        RaisePropertyChanged(L"ProcessorCurrentIdleState");
        RaisePropertyChanged(L"HasProcessIntegrityInfo");
        RaisePropertyChanged(L"ProcessIntegrityLevel");
        RaisePropertyChanged(L"ProcessIntegrityLevelString");
        RaisePropertyChanged(L"HasProcessExecuteFlags");
        RaisePropertyChanged(L"ProcessExecuteFlags");
        RaisePropertyChanged(L"HasProtectedProcess");
        RaisePropertyChanged(L"ProtectedProcess");
        RaisePropertyChanged(L"HasTimeZone");
        RaisePropertyChanged(L"TimeZoneId");
        RaisePropertyChanged(L"TimeZone");
        RaisePropertyChanged(L"HasBuildString");
        RaisePropertyChanged(L"BuildString");
        RaisePropertyChanged(L"DbgBldStr");
        RaisePropertyChanged(L"HasProcessCookie");
        RaisePropertyChanged(L"ProcessCookie");
        RaisePropertyChanged(L"HasXStateData");
        RaisePropertyChanged(L"XStateDataSize");
        RaisePropertyChanged(L"IsXStateConfigFeatureMscInfo");
        RaisePropertyChanged(L"XStateDataContextSize");
        RaisePropertyChanged(L"XStateDataEnabledFeatures");
    }
}
