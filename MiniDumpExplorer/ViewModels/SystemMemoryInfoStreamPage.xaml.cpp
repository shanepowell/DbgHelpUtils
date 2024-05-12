#include "pch.h"
#include "SystemMemoryInfoStreamPage.xaml.h"

#include "DbgHelpUtils/memory_info_utils.h"
#include "DbgHelpUtils/mini_dump.h"
#include "Helpers/UIHelper.h"
#include "Models/MiniDumpSystemBasicInformation.h"
#include "Models/MiniDumpSystemFileCacheInformation.h"
#include "Models/MiniDumpSystemBasicPerformanceInformation.h"
#include "Models/MinidumpSystemPerformanceInformation.h"

#if __has_include("SystemMemoryInfoStreamPage.g.cpp")
// ReSharper disable once CppUnusedIncludeDirective
#include "SystemMemoryInfoStreamPage.g.cpp" // NOLINT(bugprone-suspicious-include)
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    SystemMemoryInfoStreamPage::SystemMemoryInfoStreamPage()
        : GlobalOptionsNotifyPropertyChangedBase(
            {
                L"Flags"
            },
            {
            }, 
            {
            },
            {
            })
    {
    }

    void SystemMemoryInfoStreamPage::InitializeComponent()
    {
        SystemMemoryInfoStreamPageT::InitializeComponent();
        SetupFlyoutMenus();
    }

    void SystemMemoryInfoStreamPage::SetupFlyoutMenus()
    {
        UIHelper::CreateStandardHexNumberMenu(
            flags(),
            basicInfoMinimumUserModeAddress(),
            basicInfoMaximumUserModeAddress(),
            basicInfoActiveProcessorsAffinityMask(),
            fileCacheInfoFlags()
        );

        UIHelper::CreateStandardSizeNumberMenu(
            basicInfoPageSize(),
            basicInfoNumberOfPhysicalPagesSize(),
            basicInfoAllocationGranularity(),
            fileCacheInfoCurrentSize(),
            fileCacheInfoPeakSize(),
            fileCacheInfoMinimumWorkingSet(),
            fileCacheInfoMaximumWorkingSet(),
            fileCacheInfoCurrentSizeIncludingTransitionInPagesSize(),
            fileCacheInfoPeakSizeIncludingTransitionInPagesSize(),
            basicPerfInfoAvailablePagesSize(),
            basicPerfInfoCommittedPagesSize(),
            basicPerfInfoCommitLimitSize(),
            basicPerfInfoPeakCommitmentSize(),
            perfInfoAvailablePagesSize(),
            perfInfoCommittedPagesSize(),
            perfInfoCommitLimitSize(),
            perfInfoPeakCommitmentSize(),
            perfInfoFreeSystemPtesSize(),
            perfInfoResidentSystemCodePageSize(),
            perfInfoTotalSystemDriverPagesSize(),
            perfInfoTotalSystemCodePagesSize(),
            perfInfoAvailablePagedPoolPagesSize(),
            perfInfoResidentSystemCachePageSize(),
            perfInfoResidentPagedPoolPageSize(),
            perfInfoResidentSystemDriverPageSize(),
            perfInfoCcLazyWritePagesSize(),
            perfInfoCcDataPagesSize(),
            perfInfoCcTotalDirtyPagesSize(),
            perfInfoResidentAvailablePagesSize(),
            perfInfoSharedCommittedPagesSize()
        );

        UIHelper::CreateStandardDurationMenu(perfInfoIdleProcessTime());
    }

    void SystemMemoryInfoStreamPage::MiniDumpLoaded(MiniDumpExplorer::MiniDumpPageParameters const& parameters)
    {
        auto const miniDumpPage = parameters.MiniDump().as<MiniDumpPage>();
        auto const& miniDump = miniDumpPage->MiniDump();

        system_memory_info_stream_ = dlg_help_utils::system_memory_info_stream{miniDump, parameters.StreamIndex()};

        if (!system_memory_info_stream_.found() && !system_memory_info_stream_.is_valid())
        {
            logger::Log().LogMessage(log_level::error, std::format("failed to load mini dump system memory info stream index:[{}]", parameters.StreamIndex()));
            return;
        }

        auto const& info = system_memory_info_stream_.system_memory_misc_info();
        for (auto const& value : dlg_help_utils::memory_info_utils::system_memory_info_flags_to_strings(info.Flags))
        {
            flagsList_.Append(value);
        }

        auto const hasTransitionRePurposeCount = (info.Flags & MINIDUMP_SYSMEMINFO1_FILECACHE_TRANSITIONREPURPOSECOUNT_FLAGS) == MINIDUMP_SYSMEMINFO1_FILECACHE_TRANSITIONREPURPOSECOUNT_FLAGS;
        auto const hasBasicPerfInfo = (info.Flags & MINIDUMP_SYSMEMINFO1_BASICPERF) == MINIDUMP_SYSMEMINFO1_BASICPERF;
        auto const hasCcTotalDirtyPagesCcDirtyPageThreshold = (info.Flags & MINIDUMP_SYSMEMINFO1_PERF_CCTOTALDIRTYPAGES_CCDIRTYPAGETHRESHOLD) == MINIDUMP_SYSMEMINFO1_PERF_CCTOTALDIRTYPAGES_CCDIRTYPAGETHRESHOLD;
        auto const hasResidentAvailablePagesSharedCommittedPages = (info.Flags & MINIDUMP_SYSMEMINFO1_PERF_RESIDENTAVAILABLEPAGES_SHAREDCOMMITPAGES) == MINIDUMP_SYSMEMINFO1_PERF_RESIDENTAVAILABLEPAGES_SHAREDCOMMITPAGES;

        basicInfo_.as<MiniDumpSystemBasicInformation>()->Set(info.BasicInfo);
        fileCacheInfo_.as<MiniDumpSystemFileCacheInformation>()->Set(info.FileCacheInfo, hasTransitionRePurposeCount, info.BasicInfo.PageSize);
        basicPerfInfo_.as<MiniDumpSystemBasicPerformanceInformation>()->Set(info.BasicPerfInfo, hasBasicPerfInfo, info.BasicInfo.PageSize);
        perfInfo_.as<MinidumpSystemPerformanceInformation>()->Set(info.PerfInfo, hasCcTotalDirtyPagesCcDirtyPageThreshold, hasResidentAvailablePagesSharedCommittedPages, info.BasicInfo.PageSize);

        RaisePropertyChanged(L"Index");
        RaisePropertyChanged(L"Revision");
        RaisePropertyChanged(L"Flags");
        RaisePropertyChanged(L"FlagsList");
        RaisePropertyChanged(L"BasicInfo");
        RaisePropertyChanged(L"FileCacheInfo");
        RaisePropertyChanged(L"BasicPerfInfo");
        RaisePropertyChanged(L"PerfInfo");
    }
}
