#include "dump_mini_dump_info.h"

#include "dump_file_options.h"
#include "DbgHelpUtils/guid_utils.h"
#include "DbgHelpUtils/hex_dump.h"
#include "DbgHelpUtils/locale_number_formatting.h"
#include "DbgHelpUtils/mini_dump.h"
#include "DbgHelpUtils/misc_info_stream.h"
#include "DbgHelpUtils/process_vm_counters_stream.h"
#include "DbgHelpUtils/size_units.h"
#include "DbgHelpUtils/stream_hex_dump.h"
#include "DbgHelpUtils/system_info_stream.h"
#include "DbgHelpUtils/system_info_utils.h"
#include "DbgHelpUtils/system_time_utils.h"
#include "DbgHelpUtils/time_utils.h"
#include "DbgHelpUtils/time_units.h"

using namespace std;
using namespace dlg_help_utils::stream_hex_dump;
using namespace dlg_help_utils::system_time_utils;
using namespace dlg_help_utils;

namespace
{
    void dump_mini_dump_xstate_config_feature(std::wostream& log, XSTATE_CONFIG_FEATURE_MSC_INFO const& xstate, uint32_t const feature)
    {
        if (auto const mask = 1ui64 << feature; (xstate.EnabledFeatures & mask) == mask)
        {
            using namespace size_units::base_16;
            log << std::format(L"      {}\n", system_info_utils::xstate_data_feature_to_string(feature));
            log << std::format(L"        Offset: {}\n", to_hex(xstate.Features[feature].Offset));
            log << std::format(L"        Size: {0} ({1})", to_hex(xstate.Features[feature].Size), to_wstring(bytes{xstate.Features[feature].Size}));
        }
    }
}

void dump_mini_dump_system_info_stream_data(std::wostream& log, mini_dump const& mini_dump, size_t const index)
{
    system_info_stream const system_info{mini_dump, index};

    if (!system_info.found())
    {
        log << L"SystemInfoStream not found!\n";
        return;
    }

    auto const& info = system_info.system_info();
    log << std::format(L"  ProcessorArchitecture: {}\n", system_info_utils::processor_architecture_to_string(info.ProcessorArchitecture));
    log << std::format(L"  ProcessorLevel: {}\n", to_hex(info.ProcessorLevel));
    log << std::format(L"  ProcessorRevision: Model {0} Stepping {1} ({2})\n"
        , system_info.processor_model()
        , system_info.processor_stepping()
        , to_hex(info.ProcessorRevision));
    log << std::format(L"  NumberOfProcessors: {}\n", locale_formatting::to_wstring(info.NumberOfProcessors));
    log << std::format(L"  ProductType: {}\n", system_info_utils::product_type_to_string(info.ProductType));
    log << std::format(L"  OS Version: {0}.{1}.{2} - {3}\n", info.MajorVersion, info.MinorVersion, info.BuildNumber, system_info_utils::windows_version_to_string(info.MajorVersion, info.MinorVersion, info.BuildNumber, info.ProductType, info.ProcessorArchitecture, info.SuiteMask));
    log << std::format(L"  PlatformId: {}\n", system_info_utils::platform_id_to_string(info.PlatformId));
    log << std::format(L"  CSDVersion: {}\n", system_info.csd_version());
    log << std::format(L"  SuiteMask: {}\n", to_hex(info.SuiteMask));
    for (auto const& value : system_info_utils::suite_mask_to_strings(info.SuiteMask))
    {
        log << std::format(L"    {}\n", value);
    }
    log << std::format(L"  Reserved2: {}\n", to_hex(info.Reserved2));
    log << L"  CPU:\n";

    if(system_info.has_x86_cpu_info())
    {
        log << L"    X86CpuInfo\n";
        log << std::format(L"      VendorId: {}\n", system_info.vendor_id());
        log << std::format(L"      VersionInformation: {}\n", to_hex(info.Cpu.X86CpuInfo.VersionInformation));
        if (system_info.is_intel())
        {
        }
        log << std::format(L"      FeatureInformation: {}\n", to_hex(info.Cpu.X86CpuInfo.FeatureInformation));
        log << std::format(L"      AMDExtendedCpuFeatures: {}\n", to_hex(info.Cpu.X86CpuInfo.AMDExtendedCpuFeatures));
    }
    else
    {
        log << L"    OtherCpuInfo\n";
        log << std::format(L"      ProcessorFeatures: {0} - {1}\n", to_hex(info.Cpu.OtherCpuInfo.ProcessorFeatures[0]), to_hex(info.Cpu.OtherCpuInfo.ProcessorFeatures[1]));
        for (auto const& value : system_info_utils::other_processor_features_to_strings(info.Cpu.OtherCpuInfo.ProcessorFeatures[0], info.Cpu.OtherCpuInfo.ProcessorFeatures[1]))
        {
            log << std::format(L"        {}\n", value);
        }
        log << L'\n';
    }
}

void dump_mini_dump_misc_info_stream_data(std::wostream& log, mini_dump const& mini_dump, size_t const index)
{
    misc_info_stream const misc_info{mini_dump, index};

    if (!misc_info.found())
    {
        log << L"MiscInfoStream not found!\n";
        return;
    }

    if (!misc_info.is_valid())
    {
        log << L"MiscInfoStream version unknown!\n";
        return;
    }

    auto const& info = misc_info.misc_info();
    log << std::format(L"  Flags: {} - ({})\n", to_hex(info.Flags1), system_info_utils::misc_info_flags_to_string(info.Flags1));

    if (info.Flags1 & MINIDUMP_MISC1_PROCESS_ID)
    {
        log << std::format(L"  ProcessId: {}", to_hex(info.ProcessId));
    }

    auto const local_info = misc_info_stream::get_dump_file_timezone_info(mini_dump);

    if (info.Flags1 & MINIDUMP_MISC1_PROCESS_TIMES)
    {
        using namespace time_units;

        log << std::format(L"  ProcessCreateTime: [local: {0}] [UTC: {1}] [DumpLocale: {2}]\n", from_dump_file_to_local_timestamp_string(info.ProcessCreateTime, local_info), from_dump_file_to_utc_timestamp_string(info.ProcessCreateTime, local_info), to_dump_file_timestamp_string(info.ProcessCreateTime, local_info));
        log << std::format(L"  ProcessUserTime: {0} ({1}\n", to_timespan_wstring(std::chrono::seconds{info.ProcessUserTime}), std::chrono::seconds{info.ProcessUserTime});
        log << std::format(L"  ProcessKernelTime: {0} ({1})\n", to_timespan_wstring(std::chrono::seconds{info.ProcessKernelTime}), std::chrono::seconds{info.ProcessKernelTime});
    }

    if (misc_info.misc_info_version() == 1)
    {
        return;
    }

    auto const& info_2 = misc_info.misc_info_2();

    if (info.Flags1 & MINIDUMP_MISC1_PROCESSOR_POWER_INFO)
    {
        log << std::format(L"  ProcessorMaxMhz: {}\n", locale_formatting::to_wstring(info_2.ProcessorMaxMhz));
        log << std::format(L"  ProcessorCurrentMhz: {}\n", locale_formatting::to_wstring(info_2.ProcessorCurrentMhz));
        log << std::format(L"  ProcessorMhzLimit: {}\n", locale_formatting::to_wstring(info_2.ProcessorMhzLimit));
        log << std::format(L"  ProcessorMaxIdleState: {}\n", locale_formatting::to_wstring(info_2.ProcessorMaxIdleState));
        log << std::format(L"  ProcessorCurrentIdleState: {}\n", locale_formatting::to_wstring(info_2.ProcessorCurrentIdleState));
    }

    if (misc_info.misc_info_version() == 2)
    {
        return;
    }

    auto const& info_3 = misc_info.misc_info_3();

    if (info.Flags1 & MINIDUMP_MISC3_PROCESS_INTEGRITY)
    {
        log << std::format(L"  ProcessIntegrityLevel: {0} ({1})\n", to_hex(info_3.ProcessIntegrityLevel), system_info_utils::process_integrity_level_to_string(info_3.ProcessIntegrityLevel));
    }

    if (info.Flags1 & MINIDUMP_MISC3_PROCESS_EXECUTE_FLAGS)
    {
        log << std::format(L"  ProcessExecuteFlags: {}\n", to_hex(info_3.ProcessExecuteFlags));
        for (auto const& value : system_info_utils::process_execute_flags_to_strings(info_3.ProcessExecuteFlags))
        {
            log << std::format(L"    {}\n", value);
        }
    }

    if (info.Flags1 & MINIDUMP_MISC3_PROTECTED_PROCESS)
    {
        log << std::format(L"  ProtectedProcess: {}\n", locale_formatting::to_wstring(info_3.ProtectedProcess));
    }

    if (info.Flags1 & MINIDUMP_MISC3_TIMEZONE)
    {
        log << std::format(L"  TimeZoneId: {0} ({1})\n", locale_formatting::to_wstring(info_3.TimeZoneId), system_info_utils::time_zone_id_to_string(info_3.TimeZoneId));
        log << std::format(L"    Bias: {}\n", time_units::to_timespan_wstring(chrono::minutes{info_3.TimeZone.Bias}));
        log << std::format(L"    StandardName: {}\n", info_3.TimeZone.StandardName);
        log << std::format(L"    StandardDate: {}\n", to_raw_wstring(info_3.TimeZone.StandardDate));
        log << std::format(L"    StandardBias: {}\n", time_units::to_timespan_wstring(chrono::minutes{info_3.TimeZone.StandardBias}));
        log << std::format(L"    DaylightName: {}\n", info_3.TimeZone.DaylightName);
        log << std::format(L"    DaylightDate: {}\n", to_raw_wstring(info_3.TimeZone.DaylightDate));
        log << std::format(L"    DaylightBias: {}\n", time_units::to_timespan_wstring(chrono::minutes{info_3.TimeZone.DaylightBias}));
    }

    if (misc_info.misc_info_version() == 3)
    {
        return;
    }

    auto const& info_4 = misc_info.misc_info_4();

    if (info.Flags1 & MINIDUMP_MISC4_BUILDSTRING)
    {
        log << std::format(L"  BuildString: {}\n", info_4.BuildString);
        log << std::format(L"  DbgBldStr: {}\n", info_4.DbgBldStr);
    }

    if (misc_info.misc_info_version() == 4)
    {
        return;
    }

    auto const& info_5 = misc_info.misc_info_5();

    if (info.Flags1 & MINIDUMP_MISC5_PROCESS_COOKIE)
    {
        log << std::format(L"  ProcessCookie: {}\n", to_hex(info_5.ProcessCookie));
    }

    if (info_5.XStateData.SizeOfInfo > 0)
    {
        if (info_5.XStateData.SizeOfInfo != sizeof(XSTATE_CONFIG_FEATURE_MSC_INFO))
        {
            log << L"Unknown XStateData version\n";
        }
        else
        {
            using namespace size_units::base_16;

            log << L"  XStateData:\n";
            log << std::format(L"    ContextSize: {0} ({1})\n", locale_formatting::to_wstring(info_5.XStateData.ContextSize), to_wstring(bytes{info_5.XStateData.ContextSize}));
            log << std::format(L"    EnabledFeatures: {0}\n",  to_hex(info_5.XStateData.EnabledFeatures));

            for(uint32_t feature = 0; feature< MAXIMUM_XSTATE_FEATURES; ++feature)
            {
                dump_mini_dump_xstate_config_feature(log, info_5.XStateData, feature);
            }
        }
    }
}


void dump_mini_dump_process_vm_counters_stream_data(std::wostream& log, mini_dump const& mini_dump, size_t const index)
{
    process_vm_counters_stream const process_vm_counters{mini_dump, index};

    if (!process_vm_counters.found())
    {
        log << L"ProcessVmCountersStream not found!\n";
        return;
    }

    if (!process_vm_counters.is_valid())
    {
        log << L"ProcessVmCountersStream version unknown!\n";
        return;
    }

    if (process_vm_counters.process_vm_counters_version() == 1)
    {
        auto const& counters = process_vm_counters.process_vm_counters();
        using namespace size_units::base_16;
        // ReSharper disable StringLiteralTypo
        log << std::format(L"  Revision: {}\n", locale_formatting::to_wstring(counters.Revision));
        log << std::format(L"  PageFaultCount: {}\n", locale_formatting::to_wstring(counters.PageFaultCount));
        log << std::format(L"  PeakWorkingSetSize: {0} ({1})\n", locale_formatting::to_wstring(counters.PeakWorkingSetSize), to_wstring(bytes{counters.PeakWorkingSetSize}));
        log << std::format(L"  WorkingSetSize: {0} ({1})\n", locale_formatting::to_wstring(counters.WorkingSetSize), to_wstring(bytes{counters.WorkingSetSize}));
        log << std::format(L"  QuotaPeakPagedPoolUsage: {0} ({1})\n", locale_formatting::to_wstring(counters.QuotaPeakPagedPoolUsage), to_wstring(bytes{counters.QuotaPeakPagedPoolUsage}));
        log << std::format(L"  QuotaPagedPoolUsage: {0} ({1})\n", locale_formatting::to_wstring(counters.QuotaPagedPoolUsage ), to_wstring(bytes{counters.QuotaPagedPoolUsage}));
        log << std::format(L"  QuotaPeakNonPagedPoolUsage: {0} ({1})\n", locale_formatting::to_wstring(counters.QuotaPeakNonPagedPoolUsage), to_wstring(bytes{counters.QuotaPeakNonPagedPoolUsage}));
        log << std::format(L"  QuotaNonPagedPoolUsage: {0} ({1})\n", locale_formatting::to_wstring(counters.QuotaNonPagedPoolUsage), to_wstring(bytes{counters.QuotaNonPagedPoolUsage}));
        log << std::format(L"  PagefileUsage: {0} ({1})\n", locale_formatting::to_wstring(counters.PagefileUsage), to_wstring(bytes{counters.PagefileUsage}));
        log << std::format(L"  PeakPagefileUsage: {0} ({1})\n", locale_formatting::to_wstring(counters.PeakPagefileUsage), to_wstring(bytes{counters.PeakPagefileUsage}));
        log << std::format(L"  PrivateUsage: {0} ({1})\n", locale_formatting::to_wstring(counters.PrivateUsage), to_wstring(bytes{counters.PrivateUsage}));
        // ReSharper restore StringLiteralTypo
    }
    else if (process_vm_counters.process_vm_counters_version() == 2)
    {
        auto const& counters = process_vm_counters.process_vm_counters_2();
        using namespace size_units::base_16;
        // ReSharper disable StringLiteralTypo
        log << std::format(L"  Revision: {}\n", locale_formatting::to_wstring(counters.Revision));
        log << std::format(L"  Flags: {0} ({1})\n", system_info_utils::vm_counters_2_flags_to_string(counters.Flags), to_hex(counters.Flags));
        if ((counters.Flags & MINIDUMP_PROCESS_VM_COUNTERS) == MINIDUMP_PROCESS_VM_COUNTERS)
        {
            log << std::format(L"  PageFaultCount: {}\n", locale_formatting::to_wstring(counters.PageFaultCount));
            log << std::format(L"  PeakWorkingSetSize: {0} ({1})\n", locale_formatting::to_wstring(counters.PeakWorkingSetSize), to_wstring(bytes{counters.PeakWorkingSetSize}));
            log << std::format(L"  WorkingSetSize: {0} ({1})\n", locale_formatting::to_wstring(counters.WorkingSetSize), to_wstring(bytes{counters.WorkingSetSize}));
            log << std::format(L"  QuotaPeakPagedPoolUsage: {0} ({1})\n", locale_formatting::to_wstring(counters.QuotaPeakPagedPoolUsage), to_wstring(bytes{counters.QuotaPeakPagedPoolUsage}));
            log << std::format(L"  QuotaPagedPoolUsage: {0} ({1})\n", locale_formatting::to_wstring(counters.QuotaPagedPoolUsage), to_wstring(bytes{counters.QuotaPagedPoolUsage}));
            log << std::format(L"  QuotaPeakNonPagedPoolUsage: {0} ({1})\n", locale_formatting::to_wstring(counters.QuotaPeakNonPagedPoolUsage), to_wstring(bytes{counters.QuotaPeakNonPagedPoolUsage}));
            log << std::format(L"  QuotaNonPagedPoolUsage: {0} ({1})\n", locale_formatting::to_wstring(counters.QuotaNonPagedPoolUsage), to_wstring(bytes{counters.QuotaNonPagedPoolUsage}));
            log << std::format(L"  PagefileUsage: {0} ({1})\n", locale_formatting::to_wstring(counters.PagefileUsage), to_wstring(bytes{counters.PagefileUsage}));
            log << std::format(L"  PeakPagefileUsage: {0} ({1})\n", locale_formatting::to_wstring(counters.PeakPagefileUsage), to_wstring(bytes{counters.PeakPagefileUsage}));
        }
        if ((counters.Flags & MINIDUMP_PROCESS_VM_COUNTERS_VIRTUALSIZE) == MINIDUMP_PROCESS_VM_COUNTERS_VIRTUALSIZE)
        {
            log << std::format(L"  PeakVirtualSize: {0} ({1})\n", locale_formatting::to_wstring(counters.PeakVirtualSize), to_wstring(bytes{counters.PeakVirtualSize}));
            log << std::format(L"  VirtualSize: {0} ({1})\n", locale_formatting::to_wstring(counters.VirtualSize), to_wstring(bytes{counters.VirtualSize}));
        }
        if ((counters.Flags & MINIDUMP_PROCESS_VM_COUNTERS_EX) == MINIDUMP_PROCESS_VM_COUNTERS_EX)
        {
            log << std::format(L"  PrivateUsage: {0} ({1})\n", locale_formatting::to_wstring(counters.PrivateUsage), to_wstring(bytes{counters.PrivateUsage}));
        }
        if ((counters.Flags & MINIDUMP_PROCESS_VM_COUNTERS_EX2) == MINIDUMP_PROCESS_VM_COUNTERS_EX2)
        {
            log << std::format(L"  PrivateWorkingSetSize: {0} ({1})\n", locale_formatting::to_wstring(counters.PrivateWorkingSetSize), to_wstring(bytes{counters.PrivateWorkingSetSize}));
            log << std::format(L"  SharedCommitUsage: {0} ({1})\n", locale_formatting::to_wstring(counters.SharedCommitUsage), to_wstring(bytes{counters.SharedCommitUsage}));
        }
        if ((counters.Flags & MINIDUMP_PROCESS_VM_COUNTERS_JOB) == MINIDUMP_PROCESS_VM_COUNTERS_JOB)
        {
            log << std::format(L"  JobSharedCommitUsage: {0} ({1})\n", locale_formatting::to_wstring(counters.JobSharedCommitUsage), to_wstring(bytes{counters.JobSharedCommitUsage}));
            log << std::format(L"  JobPrivateCommitUsage: {0} ({1})\n", locale_formatting::to_wstring(counters.JobPrivateCommitUsage), to_wstring(bytes{counters.JobPrivateCommitUsage}));
            log << std::format(L"  JobPeakPrivateCommitUsage: {0} ({1})\n", locale_formatting::to_wstring(counters.JobPeakPrivateCommitUsage), to_wstring(bytes{counters.JobPeakPrivateCommitUsage}));
            log << std::format(L"  JobPrivateCommitLimit: {0} ({1})\n", locale_formatting::to_wstring(counters.JobPrivateCommitLimit), to_wstring(bytes{counters.JobPrivateCommitLimit}));
            log << std::format(L"  JobTotalCommitLimit: {0} ({1})\n", locale_formatting::to_wstring(counters.JobTotalCommitLimit), to_wstring(bytes{counters.JobTotalCommitLimit}));
        }
        // ReSharper restore StringLiteralTypo
    }
}
