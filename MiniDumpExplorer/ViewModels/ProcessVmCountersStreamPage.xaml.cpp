#include "pch.h"
#include "ProcessVmCountersStreamPage.xaml.h"

#include "DbgHelpUtils/mini_dump.h"
#include "DbgHelpUtils/system_info_utils.h"
#include "Helpers/UIHelper.h"

#if __has_include("ProcessVmCountersStreamPage.g.cpp")
// ReSharper disable once CppUnusedIncludeDirective
#include "ProcessVmCountersStreamPage.g.cpp" // NOLINT(bugprone-suspicious-include)
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

namespace winrt::MiniDumpExplorer::implementation
{
    ProcessVmCountersStreamPage::ProcessVmCountersStreamPage()
        : GlobalOptionsNotifyPropertyChangedBase(
            {
                L"Flags"
            },
            {
                L"PageFaultCount",
                L"PeakWorkingSetSize",
                L"WorkingSetSize",
                L"QuotaPeakPagedPoolUsage",
                L"QuotaPagedPoolUsage",
                L"QuotaPeakNonPagedPoolUsage",
                L"QuotaNonPagedPoolUsage",
                L"PagefileUsage",
                L"PeakPagefileUsage",
                L"PeakVirtualSize",
                L"VirtualSize",
                L"PrivateUsage",
                L"PrivateWorkingSetSize",
                L"SharedCommitUsage",
                L"JobSharedCommitUsage",
                L"JobPrivateCommitUsage",
                L"JobPeakPrivateCommitUsage",
                L"JobPrivateCommitLimit",
                L"JobTotalCommitLimit",
            }, 
            {
            },
            {
            })
    {
    }

    void ProcessVmCountersStreamPage::InitializeComponent()
    {
        ProcessVmCountersStreamPageT::InitializeComponent();
        SetupFlyoutMenus();
    }

    uint16_t ProcessVmCountersStreamPage::Revision() const
    {
        if(vm_counters_stream_.process_vm_counters_version() == 1)
        {
            return vm_counters_stream_.process_vm_counters().Revision;
        }
        return vm_counters_stream_.process_vm_counters_2().Revision;
    }

    uint32_t ProcessVmCountersStreamPage::PageFaultCount() const
    {
        if(vm_counters_stream_.process_vm_counters_version() == 1)
        {
            return vm_counters_stream_.process_vm_counters().PageFaultCount;
        }
        return vm_counters_stream_.process_vm_counters_2().PageFaultCount;
    }

    uint64_t ProcessVmCountersStreamPage::PeakWorkingSetSize() const
    {
        if(vm_counters_stream_.process_vm_counters_version() == 1)
        {
            return vm_counters_stream_.process_vm_counters().PeakWorkingSetSize;
        }
        return vm_counters_stream_.process_vm_counters_2().PeakWorkingSetSize;
    }

    uint64_t ProcessVmCountersStreamPage::WorkingSetSize() const
    {
        if(vm_counters_stream_.process_vm_counters_version() == 1)
        {
            return vm_counters_stream_.process_vm_counters().WorkingSetSize;
        }
        return vm_counters_stream_.process_vm_counters_2().WorkingSetSize;
    }

    uint64_t ProcessVmCountersStreamPage::QuotaPeakPagedPoolUsage() const
    {
        if(vm_counters_stream_.process_vm_counters_version() == 1)
        {
            return vm_counters_stream_.process_vm_counters().QuotaPeakPagedPoolUsage;
        }
        return vm_counters_stream_.process_vm_counters_2().QuotaPeakPagedPoolUsage;
    }

    uint64_t ProcessVmCountersStreamPage::QuotaPagedPoolUsage() const
    {
        if(vm_counters_stream_.process_vm_counters_version() == 1)
        {
            return vm_counters_stream_.process_vm_counters().QuotaPagedPoolUsage;
        }
        return vm_counters_stream_.process_vm_counters_2().QuotaPagedPoolUsage;
    }

    uint64_t ProcessVmCountersStreamPage::QuotaPeakNonPagedPoolUsage() const
    {
        if(vm_counters_stream_.process_vm_counters_version() == 1)
        {
            return vm_counters_stream_.process_vm_counters().QuotaPeakNonPagedPoolUsage;
        }
        return vm_counters_stream_.process_vm_counters_2().QuotaPeakNonPagedPoolUsage;
    }

    uint64_t ProcessVmCountersStreamPage::QuotaNonPagedPoolUsage() const
    {
        if(vm_counters_stream_.process_vm_counters_version() == 1)
        {
            return vm_counters_stream_.process_vm_counters().QuotaNonPagedPoolUsage;
        }
        return vm_counters_stream_.process_vm_counters_2().QuotaNonPagedPoolUsage;
    }

    uint64_t ProcessVmCountersStreamPage::PagefileUsage() const
    {
        if(vm_counters_stream_.process_vm_counters_version() == 1)
        {
            return vm_counters_stream_.process_vm_counters().PagefileUsage;
        }
        return vm_counters_stream_.process_vm_counters_2().PagefileUsage;
    }

    uint64_t ProcessVmCountersStreamPage::PeakPagefileUsage() const
    {
        if(vm_counters_stream_.process_vm_counters_version() == 1)
        {
            return vm_counters_stream_.process_vm_counters().PeakPagefileUsage;
        }
        return vm_counters_stream_.process_vm_counters_2().PeakPagefileUsage;
    }

    uint64_t ProcessVmCountersStreamPage::PrivateUsage() const
    {
        if(vm_counters_stream_.process_vm_counters_version() == 1)
        {
            return vm_counters_stream_.process_vm_counters().PrivateUsage;
        }
        return vm_counters_stream_.process_vm_counters_2().PrivateUsage;
    }

    void ProcessVmCountersStreamPage::SetupFlyoutMenus()
    {
        UIHelper::CreateStandardHexNumberMenu(
            flags()
        );

        UIHelper::CreateStandardSizeNumberMenu(
            pageFaultCount(),
            peakWorkingSetSize(),
            workingSetSize(),
            quotaPeakPagedPoolUsage(),
            quotaPagedPoolUsage(),
            quotaPeakNonPagedPoolUsage(),
            quotaNonPagedPoolUsage(),
            pagefileUsage(),
            peakPagefileUsage(),
            peakVirtualSize(),
            virtualSize(),
            privateUsage(),
            privateWorkingSetSize(),
            sharedCommitUsage(),
            jobSharedCommitUsage(),
            jobPrivateCommitUsage(),
            jobPeakPrivateCommitUsage(),
            jobPrivateCommitLimit(),
            jobTotalCommitLimit()
        );
    }

    void ProcessVmCountersStreamPage::MiniDumpLoaded(MiniDumpExplorer::MiniDumpPageParameters const& parameters)
    {
        auto const miniDumpPage = parameters.MiniDump().as<MiniDumpPage>();
        auto const& miniDump = miniDumpPage->MiniDump();

        vm_counters_stream_ = dlg_help_utils::process_vm_counters_stream{miniDump, parameters.StreamIndex()};

        if (!vm_counters_stream_.found() && !vm_counters_stream_.is_valid())
        {
            logger::Log().LogMessage(log_level::error, std::format("failed to load mini dump vm counters stream index:[{}]", parameters.StreamIndex()));
            return;
        }

        if(HasFlags())
        {
            for (auto const& value : dlg_help_utils::system_info_utils::vm_counters_2_flags_to_strings(vm_counters_stream_.process_vm_counters_2().Flags))
            {
                flagsList_.Append(value);
            }
        }

        RaisePropertyChanged(L"Index");
        RaisePropertyChanged(L"Revision");
        RaisePropertyChanged(L"HasFlags");
        if(HasFlags())
        {
            RaisePropertyChanged(L"Flags");
            RaisePropertyChanged(L"FlagsList");
        }
        RaisePropertyChanged(L"HasVmCounters");
        if(HasVmCounters())
        {
            RaisePropertyChanged(L"PageFaultCount");
            RaisePropertyChanged(L"PeakWorkingSetSize");
            RaisePropertyChanged(L"WorkingSetSize");
            RaisePropertyChanged(L"QuotaPeakPagedPoolUsage");
            RaisePropertyChanged(L"QuotaPagedPoolUsage");
            RaisePropertyChanged(L"QuotaPeakNonPagedPoolUsage");
            RaisePropertyChanged(L"QuotaNonPagedPoolUsage");
            RaisePropertyChanged(L"PagefileUsage");
            RaisePropertyChanged(L"PeakPagefileUsage");
        }
        RaisePropertyChanged(L"HasVirtualSize");
        if(HasVirtualSize())
        {
            RaisePropertyChanged(L"PeakVirtualSize");
            RaisePropertyChanged(L"VirtualSize");
        }
        RaisePropertyChanged(L"HasPrivateUsage");
        if(HasPrivateUsage())
        {
            RaisePropertyChanged(L"PrivateUsage");
        }
        RaisePropertyChanged(L"HasVmCountersEx2");
        if(HasVmCountersEx2())
        {
            RaisePropertyChanged(L"PrivateWorkingSetSize");
            RaisePropertyChanged(L"SharedCommitUsage");
        }
        RaisePropertyChanged(L"HasJob");
        if(HasJob())
        {
            RaisePropertyChanged(L"JobSharedCommitUsage");
            RaisePropertyChanged(L"JobPrivateCommitUsage");
            RaisePropertyChanged(L"JobPeakPrivateCommitUsage");
            RaisePropertyChanged(L"JobPrivateCommitLimit");
            RaisePropertyChanged(L"JobTotalCommitLimit");
        }
    }
}
