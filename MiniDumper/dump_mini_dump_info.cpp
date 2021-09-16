#include "dump_mini_dump_info.h"

#include <iostream>

#include "dump_file_options.h"
#include "DbgHelpUtils/guid_utils.h"
#include "DbgHelpUtils/hex_dump.h"
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

void dump_mini_dump_xstate_config_feature(XSTATE_CONFIG_FEATURE_MSC_INFO const& xstate, size_t index,
                                          std::wstring_view const& name);

void dump_mini_dump_system_info_stream_data(mini_dump const& mini_dump, size_t const index)
{
    system_info_stream const system_info{mini_dump, index};

    if (!system_info.found())
    {
        wcout << L"SystemInfoStream not found!\n";
        return;
    }

    auto const& info = system_info.system_info();
    wcout << L"  ProcessorArchitecture: " << system_info_utils::processor_architecture_to_string(
        info.ProcessorArchitecture) << L'\n';
    wcout << L"  ProcessorLevel: " << to_hex(info.ProcessorLevel) << L'\n';
    wcout << L"  ProcessorRevision: " << L"Model " << system_info.processor_model() << " Stepping " << system_info.
        processor_stepping() << L" (" << to_hex(info.ProcessorRevision) << L")\n";
    wcout << L"  NumberOfProcessors: " << info.NumberOfProcessors << L'\n';
    wcout << L"  ProductType: " << system_info_utils::product_type_to_string(info.ProductType) << L'\n';
    wcout << L"  OS Version: " << info.MajorVersion << L'.' << info.MinorVersion << L'.' << info.BuildNumber << L" - "
        << system_info_utils::windows_version_to_string(info.MajorVersion, info.MinorVersion, info.BuildNumber,
                                                        info.ProductType, info.ProcessorArchitecture,
                                                        info.SuiteMask) << L'\n';
    wcout << L"  PlatformId: " << system_info_utils::platform_id_to_string(info.PlatformId) << L'\n';
    wcout << L"  CSDVersion: " << system_info.csd_version() << L'\n';
    wcout << L"  SuiteMask: " << to_hex(info.SuiteMask) << L'\n';
    for (auto const& value : system_info_utils::suite_mask_to_strings(info.SuiteMask))
    {
        wcout << L"    " << value << L'\n';
    }
    wcout << L"  Reserved2: " << to_hex(info.Reserved2) << L'\n';
    wcout << L"  CPU:\n";
    wcout << L"    X86CpuInfo\n";
    wcout << L"      VendorId: " << system_info.vendor_id() << L'\n';
    wcout << L"      VersionInformation: " << to_hex(info.Cpu.X86CpuInfo.VersionInformation) << L'\n';
    if (system_info.is_intel())
    {
    }
    wcout << L"      FeatureInformation: " << to_hex(info.Cpu.X86CpuInfo.FeatureInformation) << L'\n';
    wcout << L"      AMDExtendedCpuFeatures: " << to_hex(info.Cpu.X86CpuInfo.AMDExtendedCpuFeatures) << L'\n';
    wcout << L"    OtherCpuInfo\n";
    wcout << L"      ProcessorFeatures: " << to_hex(info.Cpu.OtherCpuInfo.ProcessorFeatures[0]) << L" - " << to_hex(
        info.Cpu.OtherCpuInfo.ProcessorFeatures[1]) << L'\n';
    wcout << L'\n';
}

void dump_mini_dump_misc_info_stream_data(mini_dump const& mini_dump, size_t const index)
{
    misc_info_stream const misc_info{mini_dump, index};

    if (!misc_info.found())
    {
        wcout << L"MiscInfoStream not found!\n";
        return;
    }

    if (!misc_info.is_valid())
    {
        wcout << L"MiscInfoStream version unknown!\n";
        return;
    }

    auto const& info = misc_info.misc_info();
    wcout << L"  Flags: " << to_hex(info.Flags1) << L'\n';

    if (info.Flags1 & MINIDUMP_MISC1_PROCESS_ID)
    {
        wcout << L"  ProcessId: " << info.ProcessId << L'\n';
    }

    if (info.Flags1 & MINIDUMP_MISC1_PROCESS_TIMES)
    {
        using namespace time_units;
        wcout << L"  ProcessCreateTime: [local: " << time_utils::to_local_time(info.ProcessCreateTime) << L"] [UTC: " <<
            time_utils::to_utc_time(info.ProcessCreateTime) << L"]\n";
        wcout << L"  ProcessUserTime: " << info.ProcessUserTime << L" (" << std::chrono::seconds{info.ProcessUserTime}
            << L")\n";
        wcout << L"  ProcessKernelTime: " << info.ProcessKernelTime << L" (" << std::chrono::seconds{
            info.ProcessKernelTime
        } << L")\n";
    }

    if (misc_info.misc_info_version() == 1)
    {
        return;
    }

    auto const& info_2 = misc_info.misc_info_2();

    if (info.Flags1 & MINIDUMP_MISC1_PROCESSOR_POWER_INFO)
    {
        wcout << L"  ProcessorMaxMhz: " << info_2.ProcessorMaxMhz << L'\n';
        wcout << L"  ProcessorCurrentMhz: " << info_2.ProcessorCurrentMhz << L'\n';
        wcout << L"  ProcessorMhzLimit: " << info_2.ProcessorMhzLimit << L'\n';
        wcout << L"  ProcessorMaxIdleState: " << info_2.ProcessorMaxIdleState << L'\n';
        wcout << L"  ProcessorCurrentIdleState: " << info_2.ProcessorCurrentIdleState << L'\n';
    }

    if (misc_info.misc_info_version() == 2)
    {
        return;
    }

    auto const& info_3 = misc_info.misc_info_3();

    if (info.Flags1 & MINIDUMP_MISC3_PROCESS_INTEGRITY)
    {
        wcout << L"  ProcessIntegrityLevel: " << to_hex(info_3.ProcessIntegrityLevel) << L" (" <<
            system_info_utils::process_integrity_level_to_string(info_3.ProcessIntegrityLevel) << L")\n";
    }

    if (info.Flags1 & MINIDUMP_MISC3_PROCESS_EXECUTE_FLAGS)
    {
        wcout << L"  ProcessExecuteFlags: " << to_hex(info_3.ProcessExecuteFlags) << L'\n';
        for (auto const& value : system_info_utils::process_execute_flags_to_strings(info_3.ProcessExecuteFlags))
        {
            wcout << L"    " << value << L'\n';
        }
    }

    if (info.Flags1 & MINIDUMP_MISC3_PROTECTED_PROCESS)
    {
        wcout << L"  ProtectedProcess: " << info_3.ProtectedProcess << L'\n';
    }

    if (info.Flags1 & MINIDUMP_MISC3_TIMEZONE)
    {
        wcout << L"  TimeZoneId: " << info_3.TimeZoneId << L'\n';
        wcout << L"    Bias: " << info_3.TimeZone.Bias << L'\n';
        wcout << L"    StandardName: " << info_3.TimeZone.StandardName << L'\n';
        wcout << L"    StandardDate: " << info_3.TimeZone.StandardDate << L'\n';
        wcout << L"    StandardBias: " << info_3.TimeZone.StandardBias << L'\n';
        wcout << L"    DaylightName: " << info_3.TimeZone.DaylightName << L'\n';
        wcout << L"    DaylightDate: " << info_3.TimeZone.DaylightDate << L'\n';
        wcout << L"    DaylightBias: " << info_3.TimeZone.DaylightBias << L'\n';
    }

    if (misc_info.misc_info_version() == 3)
    {
        return;
    }

    auto const& info_4 = misc_info.misc_info_4();

    if (info.Flags1 & MINIDUMP_MISC4_BUILDSTRING)
    {
        wcout << L"  BuildString: " << info_4.BuildString << L'\n';
        wcout << L"  DbgBldStr: " << info_4.DbgBldStr << L'\n';
    }

    if (misc_info.misc_info_version() == 4)
    {
        return;
    }

    auto const& info_5 = misc_info.misc_info_5();

    if (info.Flags1 & MINIDUMP_MISC5_PROCESS_COOKIE)
    {
        wcout << L"  ProcessCookie: " << info_5.ProcessCookie << L'\n';
    }

    if (info_5.XStateData.SizeOfInfo > 0)
    {
        if (info_5.XStateData.SizeOfInfo != sizeof(XSTATE_CONFIG_FEATURE_MSC_INFO))
        {
            wcout << "Unknown XStateData version\n";
        }
        else
        {
            using namespace size_units::base_16;

            wcout << L"  XStateData:\n";
            wcout << L"    ContextSize: " << info_5.XStateData.ContextSize << " (" << bytes{
                info_5.XStateData.ContextSize
            } << L")\n";
            wcout << L"    EnabledFeatures: " << to_hex(info_5.XStateData.EnabledFeatures) << L'\n';

            dump_mini_dump_xstate_config_feature(info_5.XStateData, XSTATE_LEGACY_FLOATING_POINT,
                                                 L"XSTATE_LEGACY_FLOATING_POINT"sv);
            dump_mini_dump_xstate_config_feature(info_5.XStateData, XSTATE_LEGACY_SSE, L"XSTATE_LEGACY_SSE"sv);
            dump_mini_dump_xstate_config_feature(info_5.XStateData, XSTATE_AVX, L"XSTATE_AVX"sv);
            dump_mini_dump_xstate_config_feature(info_5.XStateData, XSTATE_MPX_BNDREGS, L"XSTATE_MPX_BNDREGS"sv);
            dump_mini_dump_xstate_config_feature(info_5.XStateData, XSTATE_MPX_BNDCSR, L"XSTATE_MPX_BNDCSR"sv);
            dump_mini_dump_xstate_config_feature(info_5.XStateData, XSTATE_AVX512_KMASK, L"XSTATE_AVX512_KMASK"sv);
            dump_mini_dump_xstate_config_feature(info_5.XStateData, XSTATE_AVX512_ZMM_H, L"XSTATE_AVX512_ZMM_H"sv);
            dump_mini_dump_xstate_config_feature(info_5.XStateData, XSTATE_AVX512_ZMM, L"XSTATE_AVX512_ZMM"sv);
            dump_mini_dump_xstate_config_feature(info_5.XStateData, XSTATE_IPT, L"XSTATE_IPT"sv);
            dump_mini_dump_xstate_config_feature(info_5.XStateData, XSTATE_CET_U, L"XSTATE_CET_U"sv);
            dump_mini_dump_xstate_config_feature(info_5.XStateData, XSTATE_LWP, L"XSTATE_LWP"sv);
        }
    }
}

void dump_mini_dump_xstate_config_feature(XSTATE_CONFIG_FEATURE_MSC_INFO const& xstate, size_t const index,
                                          std::wstring_view const& name)
{
    if (auto const mask = 1ui64 << index; (xstate.EnabledFeatures & mask) == mask)
    {
        using namespace size_units::base_16;
        wcout << L"      " << name << L'\n';
        wcout << L"        Offset: " << to_hex(xstate.Features[index].Offset) << L'\n';
        wcout << L"        Size: " << to_hex(xstate.Features[index].Size) << L" (" << bytes{xstate.Features[index].Size}
            << L")\n";
    }
}

void dump_mini_dump_process_vm_counters_stream_data(mini_dump const& mini_dump, size_t const index)
{
    process_vm_counters_stream const process_vm_counters{mini_dump, index};

    if (!process_vm_counters.found())
    {
        wcout << L"ProcessVmCountersStream not found!\n";
        return;
    }

    if (!process_vm_counters.is_valid())
    {
        wcout << L"ProcessVmCountersStream version unknown!\n";
        return;
    }

    if (process_vm_counters.process_vm_counters_version() == 1)
    {
        auto const& counters = process_vm_counters.process_vm_counters();
        using namespace size_units::base_16;
        // ReSharper disable StringLiteralTypo
        wcout << L"  Revision: " << counters.Revision << L'\n';
        wcout << L"  PageFaultCount: " << counters.PageFaultCount << L'\n';
        wcout << L"  PeakWorkingSetSize: " << counters.PeakWorkingSetSize << L" (" << bytes{counters.PeakWorkingSetSize}
            << L")\n";
        wcout << L"  WorkingSetSize: " << counters.WorkingSetSize << L" (" << bytes{counters.WorkingSetSize} << L")\n";
        wcout << L"  QuotaPeakPagedPoolUsage: " << counters.QuotaPeakPagedPoolUsage << L" (" << bytes{
            counters.QuotaPeakPagedPoolUsage
        } << L")\n";
        wcout << L"  QuotaPagedPoolUsage: " << counters.QuotaPagedPoolUsage << L" (" << bytes{
            counters.QuotaPagedPoolUsage
        } << L")\n";
        wcout << L"  QuotaPeakNonPagedPoolUsage: " << counters.QuotaPeakNonPagedPoolUsage << L" (" << bytes{
            counters.QuotaPeakNonPagedPoolUsage
        } << L")\n";
        wcout << L"  QuotaNonPagedPoolUsage: " << counters.QuotaNonPagedPoolUsage << L" (" << bytes{
            counters.QuotaNonPagedPoolUsage
        } << L")\n";
        wcout << L"  PagefileUsage: " << counters.PagefileUsage << L" (" << bytes{counters.PagefileUsage} << L")\n";
        wcout << L"  PeakPagefileUsage: " << counters.PeakPagefileUsage << L" (" << bytes{counters.PeakPagefileUsage} <<
            L")\n";
        wcout << L"  PrivateUsage: " << counters.PrivateUsage << L" (" << bytes{counters.PrivateUsage} << L")\n";
        // ReSharper restore StringLiteralTypo
    }
    else if (process_vm_counters.process_vm_counters_version() == 2)
    {
        auto const& counters = process_vm_counters.process_vm_counters_2();
        using namespace size_units::base_16;
        // ReSharper disable StringLiteralTypo
        wcout << L"  Revision: " << counters.Revision << L'\n';
        wcout << L"  Flags: " << system_info_utils::vm_counters_2_flags_to_string(counters.Flags) << L" (" <<
            to_hex(counters.Flags) << L")\n";
        if ((counters.Flags & MINIDUMP_PROCESS_VM_COUNTERS) == MINIDUMP_PROCESS_VM_COUNTERS)
        {
            wcout << L"  PageFaultCount: " << counters.PageFaultCount << L'\n';
            wcout << L"  PeakWorkingSetSize: " << counters.PeakWorkingSetSize << L" (" << bytes{
                counters.PeakWorkingSetSize
            } << L")\n";
            wcout << L"  WorkingSetSize: " << counters.WorkingSetSize << L" (" << bytes{counters.WorkingSetSize} <<
                L")\n";
            wcout << L"  QuotaPeakPagedPoolUsage: " << counters.QuotaPeakPagedPoolUsage << L" (" << bytes{
                counters.QuotaPeakPagedPoolUsage
            } << L")\n";
            wcout << L"  QuotaPagedPoolUsage: " << counters.QuotaPagedPoolUsage << L" (" << bytes{
                counters.QuotaPagedPoolUsage
            } << L")\n";
            wcout << L"  QuotaPeakNonPagedPoolUsage: " << counters.QuotaPeakNonPagedPoolUsage << L" (" << bytes{
                counters.QuotaPeakNonPagedPoolUsage
            } << L")\n";
            wcout << L"  QuotaNonPagedPoolUsage: " << counters.QuotaNonPagedPoolUsage << L" (" << bytes{
                counters.QuotaNonPagedPoolUsage
            } << L")\n";
            wcout << L"  PagefileUsage: " << counters.PagefileUsage << L" (" << bytes{counters.PagefileUsage} << L")\n";
            wcout << L"  PeakPagefileUsage: " << counters.PeakPagefileUsage << L" (" << bytes{
                counters.PeakPagefileUsage
            } << L")\n";
        }
        if ((counters.Flags & MINIDUMP_PROCESS_VM_COUNTERS_VIRTUALSIZE) == MINIDUMP_PROCESS_VM_COUNTERS_VIRTUALSIZE)
        {
            wcout << L"  PeakVirtualSize: " << counters.PeakVirtualSize << L" (" << bytes{counters.PeakVirtualSize} <<
                L")\n";
            wcout << L"  VirtualSize: " << counters.VirtualSize << L" (" << bytes{counters.VirtualSize} << L")\n";
        }
        if ((counters.Flags & MINIDUMP_PROCESS_VM_COUNTERS_EX) == MINIDUMP_PROCESS_VM_COUNTERS_EX)
        {
            wcout << L"  PrivateUsage: " << counters.PrivateUsage << L" (" << bytes{counters.PrivateUsage} << L")\n";
        }
        if ((counters.Flags & MINIDUMP_PROCESS_VM_COUNTERS_EX2) == MINIDUMP_PROCESS_VM_COUNTERS_EX2)
        {
            wcout << L"  PrivateWorkingSetSize: " << counters.PrivateWorkingSetSize << L" (" << bytes{
                counters.PrivateWorkingSetSize
            } << L")\n";
            wcout << L"  SharedCommitUsage: " << counters.SharedCommitUsage << L" (" << bytes{
                counters.SharedCommitUsage
            } << L")\n";
        }
        if ((counters.Flags & MINIDUMP_PROCESS_VM_COUNTERS_JOB) == MINIDUMP_PROCESS_VM_COUNTERS_JOB)
        {
            wcout << L"  JobSharedCommitUsage: " << counters.JobSharedCommitUsage << L" (" << bytes{
                counters.JobSharedCommitUsage
            } << L")\n";
            wcout << L"  JobPrivateCommitUsage: " << counters.JobPrivateCommitUsage << L" (" << bytes{
                counters.JobPrivateCommitUsage
            } << L")\n";
            wcout << L"  JobPeakPrivateCommitUsage: " << counters.JobPeakPrivateCommitUsage << L" (" << bytes{
                counters.JobPeakPrivateCommitUsage
            } << L")\n";
            wcout << L"  JobPrivateCommitLimit: " << counters.JobPrivateCommitLimit << L" (" << bytes{
                counters.JobPrivateCommitLimit
            } << L")\n";
            wcout << L"  JobTotalCommitLimit: " << counters.JobTotalCommitLimit << L" (" << bytes{
                counters.JobTotalCommitLimit
            } << L")\n";
        }
        // ReSharper restore StringLiteralTypo
    }
}
