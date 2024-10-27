#include "pch.h"
#include "DbgHelpUtils/system_info_utils.h"
#include "DbgHelpUtils/system_info_utils_resources.h"

#include <winrt/Microsoft.Windows.ApplicationModel.Resources.h>

#include "DbgHelpUtils/flags_string_utils.h"
#include "DbgHelpUtils/mini_dump.h"

#include <avrfsdk.h>
#include <string>
#include <unordered_map>


using namespace dlg_help_utils::system_info_utils::resources;
using namespace std::string_literals;
using namespace std::string_view_literals;

namespace
{
    std::unordered_map<platform_type, std::wstring> const& platform_types()
    {
        winrt::Microsoft::Windows::ApplicationModel::Resources::ResourceManager const rm{};
        static std::unordered_map<platform_type, std::wstring> const platform_types =
        {
            {platform_type::workstation, std::wstring{rm.MainResourceMap().GetValue(L"Resources/workstation").ValueAsString()}},
            {platform_type::home_server, std::wstring{rm.MainResourceMap().GetValue(L"Resources/home_server").ValueAsString()}},
            {platform_type::professional_edition, std::wstring{rm.MainResourceMap().GetValue(L"Resources/professional_edition").ValueAsString()}},
            {platform_type::server, std::wstring{rm.MainResourceMap().GetValue(L"Resources/server").ValueAsString()}},
            {platform_type::server_2019, std::wstring{rm.MainResourceMap().GetValue(L"Resources/server_2019").ValueAsString()}},
            {platform_type::server_v_next, std::wstring{rm.MainResourceMap().GetValue(L"Resources/server_v_next").ValueAsString()}},
        };

        return platform_types;
    }

    std::unordered_map<windows_version, std::wstring> const& version_strings()
    {
        winrt::Microsoft::Windows::ApplicationModel::Resources::ResourceManager const rm{};

        using namespace dlg_help_utils::system_info_utils::resources::windows_versions;
        // see https://en.wikipedia.org/wiki/List_of_Microsoft_Windows_versions
        static std::unordered_map<windows_version, std::wstring> const version_strings =
        {
            // see https://en.wikipedia.org/wiki/Windows_Server_2022
            {windows_server_2022, std::wstring{rm.MainResourceMap().GetValue(L"Resources/windows_server_2022").ValueAsString()}},
            {windows_server_2022_preview, std::wstring{rm.MainResourceMap().GetValue(L"Resources/windows_server_2022_preview").ValueAsString()}},

            // see https://en.wikipedia.org/wiki/Windows_Server_2019
            {windows_server_2019_2004, std::wstring{rm.MainResourceMap().GetValue(L"Resources/windows_server_2019_2004").ValueAsString()}},
            {windows_server_2019_2009_20H2, std::wstring{rm.MainResourceMap().GetValue(L"Resources/windows_server_2019_2009_20H2").ValueAsString()}},
            {windows_server_2019_1903, std::wstring{rm.MainResourceMap().GetValue(L"Resources/windows_server_2019_1903").ValueAsString()}},
            {windows_server_2019_1909, std::wstring{rm.MainResourceMap().GetValue(L"Resources/windows_server_2019_1909").ValueAsString()}},
            {windows_server_2019_20H2, std::wstring{rm.MainResourceMap().GetValue(L"Resources/windows_server_2019_20H2").ValueAsString()}},
            {windows_server_vnext, std::wstring{rm.MainResourceMap().GetValue(L"Resources/windows_server_vnext").ValueAsString()}},
            {windows_server_2019, std::wstring{rm.MainResourceMap().GetValue(L"Resources/windows_server_2019").ValueAsString()}},
            {windows_server_2019_datacenter, std::wstring{rm.MainResourceMap().GetValue(L"Resources/windows_server_2019_datacenter").ValueAsString()}},

            {windows_server_2016_fall_creators_update, std::wstring{rm.MainResourceMap().GetValue(L"Resources/windows_server_2016_fall_creators_update").ValueAsString()}},
            {windows_server_2016, std::wstring{rm.MainResourceMap().GetValue(L"Resources/windows_server_2016").ValueAsString()}},

            {windows_server_2012_R2, std::wstring{rm.MainResourceMap().GetValue(L"Resources/windows_server_2012_R2").ValueAsString()}},
            {windows_server_2012, std::wstring{rm.MainResourceMap().GetValue(L"Resources/windows_server_2012").ValueAsString()}},
            {windows_server_2008_R2, std::wstring{rm.MainResourceMap().GetValue(L"Resources/windows_server_2008_R2").ValueAsString()}},
            {windows_server_2008, std::wstring{rm.MainResourceMap().GetValue(L"Resources/windows_server_2008").ValueAsString()}},
            {windows_server_home_server, std::wstring{rm.MainResourceMap().GetValue(L"Resources/windows_server_home_server").ValueAsString()}},
            {windows_server_2003, std::wstring{rm.MainResourceMap().GetValue(L"Resources/windows_server_2003").ValueAsString()}},

            // see https://en.wikipedia.org/wiki/Windows_11_version_history
            {windows_11_23H2, std::wstring{rm.MainResourceMap().GetValue(L"Resources/windows_11_23H2").ValueAsString()}},
            {windows_11_22H2, std::wstring{rm.MainResourceMap().GetValue(L"Resources/windows_11_22H2").ValueAsString()}},
            {windows_11, std::wstring{rm.MainResourceMap().GetValue(L"Resources/windows_11").ValueAsString()}},

            // see https://en.wikipedia.org/wiki/Windows_10_version_history
            {windows_10_22H2, std::wstring{rm.MainResourceMap().GetValue(L"Resources/windows_10_22H2").ValueAsString()}},
            {windows_10_21H2, std::wstring{rm.MainResourceMap().GetValue(L"Resources/windows_10_21H2").ValueAsString()}},
            {windows_10_21H1, std::wstring{rm.MainResourceMap().GetValue(L"Resources/windows_10_21H1").ValueAsString()}},
            {windows_10_20H2, std::wstring{rm.MainResourceMap().GetValue(L"Resources/windows_10_20H2").ValueAsString()}},
            {windows_10_2004, std::wstring{rm.MainResourceMap().GetValue(L"Resources/windows_10_2004").ValueAsString()}},
            {windows_10_1909, std::wstring{rm.MainResourceMap().GetValue(L"Resources/windows_10_1909").ValueAsString()}},
            {windows_10_1903, std::wstring{rm.MainResourceMap().GetValue(L"Resources/windows_10_1903").ValueAsString()}},
            {windows_10_october_2018, std::wstring{rm.MainResourceMap().GetValue(L"Resources/windows_10_october_2018").ValueAsString()}},
            {windows_10_april_2018, std::wstring{rm.MainResourceMap().GetValue(L"Resources/windows_10_april_2018").ValueAsString()}},
            {windows_10_fall_creators_update, std::wstring{rm.MainResourceMap().GetValue(L"Resources/windows_10_fall_creators_update").ValueAsString()}},
            {windows_10_creators_update, std::wstring{rm.MainResourceMap().GetValue(L"Resources/windows_10_creators_update").ValueAsString()}},
            {windows_10_anniversary_update, std::wstring{rm.MainResourceMap().GetValue(L"Resources/windows_10_anniversary_update").ValueAsString()}},
            {windows_10_november_update, std::wstring{rm.MainResourceMap().GetValue(L"Resources/windows_10_november_update").ValueAsString()}},
            {windows_10_gold, std::wstring{rm.MainResourceMap().GetValue(L"Resources/windows_10_gold").ValueAsString()}},
            {windows_10, std::wstring{rm.MainResourceMap().GetValue(L"Resources/windows_10").ValueAsString()}},

            {windows_8_1, std::wstring{rm.MainResourceMap().GetValue(L"Resources/windows_8_1").ValueAsString()}},
            {windows_8, std::wstring{rm.MainResourceMap().GetValue(L"Resources/windows_8").ValueAsString()}},

            {windows_7, std::wstring{rm.MainResourceMap().GetValue(L"Resources/windows_7").ValueAsString()}},
            {windows_vista, std::wstring{rm.MainResourceMap().GetValue(L"Resources/windows_vista").ValueAsString()}},
            {windows_server, std::wstring{rm.MainResourceMap().GetValue(L"Resources/windows_server").ValueAsString()}},
            {windows_xp, std::wstring{rm.MainResourceMap().GetValue(L"Resources/windows_xp").ValueAsString()}},
            {windows_2000, std::wstring{rm.MainResourceMap().GetValue(L"Resources/windows_2000").ValueAsString()}},
        };

        return version_strings;
    }

    std::unordered_map<uint16_t, std::wstring> const& vm_counters_2_flag_masks()
    {
        winrt::Microsoft::Windows::ApplicationModel::Resources::ResourceManager const rm{};
        static std::unordered_map<uint16_t, std::wstring> const vm_counters_2_flag_masks =
        {
            {static_cast<uint16_t>(MINIDUMP_PROCESS_VM_COUNTERS), std::wstring{rm.MainResourceMap().GetValue(L"Resources/MINIDUMP_PROCESS_VM_COUNTERS").ValueAsString()}},
            {static_cast<uint16_t>(MINIDUMP_PROCESS_VM_COUNTERS_VIRTUALSIZE), std::wstring{rm.MainResourceMap().GetValue(L"Resources/MINIDUMP_PROCESS_VM_COUNTERS_VIRTUALSIZE").ValueAsString()}},
            {static_cast<uint16_t>(MINIDUMP_PROCESS_VM_COUNTERS_EX), std::wstring{rm.MainResourceMap().GetValue(L"Resources/MINIDUMP_PROCESS_VM_COUNTERS_EX").ValueAsString()}},
            {static_cast<uint16_t>(MINIDUMP_PROCESS_VM_COUNTERS_EX2), std::wstring{rm.MainResourceMap().GetValue(L"Resources/MINIDUMP_PROCESS_VM_COUNTERS_EX2").ValueAsString()}},
            {static_cast<uint16_t>(MINIDUMP_PROCESS_VM_COUNTERS_JOB), std::wstring{rm.MainResourceMap().GetValue(L"Resources/MINIDUMP_PROCESS_VM_COUNTERS_JOB").ValueAsString()}},
        };

        return vm_counters_2_flag_masks;
    }

    std::unordered_map<uint32_t, std::wstring> const& version_file_flag_masks()
    {
        winrt::Microsoft::Windows::ApplicationModel::Resources::ResourceManager const rm{};
        static std::unordered_map<uint32_t, std::wstring> const version_file_flag_masks =
        {
            {static_cast<uint32_t>(VS_FF_DEBUG), std::wstring{rm.MainResourceMap().GetValue(L"Resources/VS_FF_DEBUG").ValueAsString()}},
            {static_cast<uint32_t>(VS_FF_INFOINFERRED), std::wstring{rm.MainResourceMap().GetValue(L"Resources/VS_FF_INFOINFERRED").ValueAsString()}},
            {static_cast<uint32_t>(VS_FF_PATCHED), std::wstring{rm.MainResourceMap().GetValue(L"Resources/VS_FF_PATCHED").ValueAsString()}},
            {static_cast<uint32_t>(VS_FF_PRERELEASE), std::wstring{rm.MainResourceMap().GetValue(L"Resources/VS_FF_PRERELEASE").ValueAsString()}},
            {static_cast<uint32_t>(VS_FF_PRIVATEBUILD), std::wstring{rm.MainResourceMap().GetValue(L"Resources/VS_FF_PRIVATEBUILD").ValueAsString()}},
            {static_cast<uint32_t>(VS_FF_SPECIALBUILD), std::wstring{rm.MainResourceMap().GetValue(L"Resources/VS_FF_SPECIALBUILD").ValueAsString()}},
        };

        return version_file_flag_masks;
    }

    std::unordered_map<uint32_t, std::wstring> const& version_file_os_masks()
    {
        winrt::Microsoft::Windows::ApplicationModel::Resources::ResourceManager const rm{};
        static std::unordered_map<uint32_t, std::wstring> const version_file_os_masks =
        {
            {static_cast<uint32_t>(VOS__WINDOWS16), std::wstring{rm.MainResourceMap().GetValue(L"Resources/VOS__WINDOWS16").ValueAsString()}},
            {static_cast<uint32_t>(VOS__WINDOWS32), std::wstring{rm.MainResourceMap().GetValue(L"Resources/VOS__WINDOWS32").ValueAsString()}},
            {static_cast<uint32_t>(VOS__PM16), std::wstring{rm.MainResourceMap().GetValue(L"Resources/VOS__PM16").ValueAsString()}},
            {static_cast<uint32_t>(VOS__PM32), std::wstring{rm.MainResourceMap().GetValue(L"Resources/VOS__PM32").ValueAsString()}},
            {static_cast<uint32_t>(VOS_DOS), std::wstring{rm.MainResourceMap().GetValue(L"Resources/VOS_DOS").ValueAsString()}},
            {static_cast<uint32_t>(VOS_NT), std::wstring{rm.MainResourceMap().GetValue(L"Resources/VOS_NT").ValueAsString()}},
            {static_cast<uint32_t>(VOS_OS216), std::wstring{rm.MainResourceMap().GetValue(L"Resources/VOS_OS216").ValueAsString()}},
            {static_cast<uint32_t>(VOS_OS232), std::wstring{rm.MainResourceMap().GetValue(L"Resources/VOS_OS232").ValueAsString()}},
            {static_cast<uint32_t>(VOS_WINCE), std::wstring{rm.MainResourceMap().GetValue(L"Resources/VOS_WINCE").ValueAsString()}},
        };

        return version_file_os_masks;
    }

    std::unordered_map<dlg_help_utils::dump_file_type, std::wstring> const& dump_file_types()
    {
        winrt::Microsoft::Windows::ApplicationModel::Resources::ResourceManager const rm{};
        static std::unordered_map<dlg_help_utils::dump_file_type, std::wstring> const dump_file_types =
        {
            {dlg_help_utils::dump_file_type::invalid, std::wstring{rm.MainResourceMap().GetValue(L"Resources/dump_file_type_invalid").ValueAsString()}},
            {dlg_help_utils::dump_file_type::user_mode_dump, std::wstring{rm.MainResourceMap().GetValue(L"Resources/dump_file_type_user_mode_dump").ValueAsString()}},
            {dlg_help_utils::dump_file_type::kernel_mode_dump_x86, std::wstring{rm.MainResourceMap().GetValue(L"Resources/dump_file_type_kernel_mode_dump_x86").ValueAsString()}},
            {dlg_help_utils::dump_file_type::kernel_mode_dump_x64, std::wstring{rm.MainResourceMap().GetValue(L"Resources/dump_file_type_kernel_mode_dump_x64").ValueAsString()}},
        };

        return dump_file_types;
    }

    std::unordered_map<unsigned short, std::wstring> const& processor_architectures()
    {
        winrt::Microsoft::Windows::ApplicationModel::Resources::ResourceManager const rm{};
        static std::unordered_map<unsigned short, std::wstring> const processor_architectures =
        {
            {static_cast<unsigned short>(PROCESSOR_ARCHITECTURE_INTEL), std::wstring{rm.MainResourceMap().GetValue(L"Resources/PROCESSOR_ARCHITECTURE_INTEL").ValueAsString()}},
            {static_cast<unsigned short>(PROCESSOR_ARCHITECTURE_MIPS), std::wstring{rm.MainResourceMap().GetValue(L"Resources/PROCESSOR_ARCHITECTURE_MIPS").ValueAsString()}},
            {static_cast<unsigned short>(PROCESSOR_ARCHITECTURE_ALPHA), std::wstring{rm.MainResourceMap().GetValue(L"Resources/PROCESSOR_ARCHITECTURE_ALPHA").ValueAsString()}},
            {static_cast<unsigned short>(PROCESSOR_ARCHITECTURE_PPC), std::wstring{rm.MainResourceMap().GetValue(L"Resources/PROCESSOR_ARCHITECTURE_PPC").ValueAsString()}},
            {static_cast<unsigned short>(PROCESSOR_ARCHITECTURE_SHX), std::wstring{rm.MainResourceMap().GetValue(L"Resources/PROCESSOR_ARCHITECTURE_SHX").ValueAsString()}},
            {static_cast<unsigned short>(PROCESSOR_ARCHITECTURE_ARM), std::wstring{rm.MainResourceMap().GetValue(L"Resources/PROCESSOR_ARCHITECTURE_ARM").ValueAsString()}},
            {static_cast<unsigned short>(PROCESSOR_ARCHITECTURE_IA64), std::wstring{rm.MainResourceMap().GetValue(L"Resources/PROCESSOR_ARCHITECTURE_IA64").ValueAsString()}},
            {static_cast<unsigned short>(PROCESSOR_ARCHITECTURE_ALPHA64), std::wstring{rm.MainResourceMap().GetValue(L"Resources/PROCESSOR_ARCHITECTURE_ALPHA64").ValueAsString()}},
            {static_cast<unsigned short>(PROCESSOR_ARCHITECTURE_MSIL), std::wstring{rm.MainResourceMap().GetValue(L"Resources/PROCESSOR_ARCHITECTURE_MSIL").ValueAsString()}},
            {static_cast<unsigned short>(PROCESSOR_ARCHITECTURE_AMD64), std::wstring{rm.MainResourceMap().GetValue(L"Resources/PROCESSOR_ARCHITECTURE_AMD64").ValueAsString()}},
            {static_cast<unsigned short>(PROCESSOR_ARCHITECTURE_IA32_ON_WIN64), std::wstring{rm.MainResourceMap().GetValue(L"Resources/PROCESSOR_ARCHITECTURE_IA32_ON_WIN64").ValueAsString()}},
            {static_cast<unsigned short>(PROCESSOR_ARCHITECTURE_NEUTRAL), std::wstring{rm.MainResourceMap().GetValue(L"Resources/PROCESSOR_ARCHITECTURE_NEUTRAL").ValueAsString()}},
            {static_cast<unsigned short>(PROCESSOR_ARCHITECTURE_ARM64), std::wstring{rm.MainResourceMap().GetValue(L"Resources/PROCESSOR_ARCHITECTURE_ARM64").ValueAsString()}},
            {static_cast<unsigned short>(PROCESSOR_ARCHITECTURE_ARM32_ON_WIN64), std::wstring{rm.MainResourceMap().GetValue(L"Resources/PROCESSOR_ARCHITECTURE_ARM32_ON_WIN64").ValueAsString()}},
            {static_cast<unsigned short>(PROCESSOR_ARCHITECTURE_IA32_ON_ARM64), std::wstring{rm.MainResourceMap().GetValue(L"Resources/PROCESSOR_ARCHITECTURE_IA32_ON_ARM64").ValueAsString()}},
            {static_cast<unsigned short>(PROCESSOR_ARCHITECTURE_UNKNOWN), std::wstring{rm.MainResourceMap().GetValue(L"Resources/PROCESSOR_ARCHITECTURE_UNKNOWN").ValueAsString()}},
        };

        return processor_architectures;
    }

    std::unordered_map<unsigned char, std::wstring> const& product_types()
    {
        winrt::Microsoft::Windows::ApplicationModel::Resources::ResourceManager const rm{};
        static std::unordered_map<unsigned char, std::wstring> const product_types =
        {
            {static_cast<unsigned char>(VER_NT_WORKSTATION), std::wstring{rm.MainResourceMap().GetValue(L"Resources/VER_NT_WORKSTATION").ValueAsString()}},
            {static_cast<unsigned char>(VER_NT_SERVER), std::wstring{rm.MainResourceMap().GetValue(L"Resources/VER_NT_SERVER").ValueAsString()}},
            {static_cast<unsigned char>(VER_NT_DOMAIN_CONTROLLER), std::wstring{rm.MainResourceMap().GetValue(L"Resources/VER_NT_DOMAIN_CONTROLLER").ValueAsString()}},
        };

        return product_types;
    }

    std::unordered_map<uint32_t, std::wstring> const& platform_ids()
    {
        winrt::Microsoft::Windows::ApplicationModel::Resources::ResourceManager const rm{};
        static std::unordered_map<uint32_t, std::wstring> const platform_ids =
        {
            {VER_PLATFORM_WIN32_NT, std::wstring{rm.MainResourceMap().GetValue(L"Resources/VER_PLATFORM_WIN32_NT").ValueAsString()}},
            {VER_PLATFORM_WIN32s, std::wstring{rm.MainResourceMap().GetValue(L"Resources/VER_PLATFORM_WIN32s").ValueAsString()}},
            {VER_PLATFORM_WIN32_WINDOWS, std::wstring{rm.MainResourceMap().GetValue(L"Resources/VER_PLATFORM_WIN32_WINDOWS").ValueAsString()}},
        };

        return platform_ids;
    }

    std::unordered_map<unsigned short, std::wstring> const& suite_masks()
    {
        winrt::Microsoft::Windows::ApplicationModel::Resources::ResourceManager const rm{};
        static std::unordered_map<unsigned short, std::wstring> const suite_masks =
        {
            {static_cast<unsigned short>(VER_SUITE_SMALLBUSINESS), std::wstring{rm.MainResourceMap().GetValue(L"Resources/VER_SUITE_SMALLBUSINESS").ValueAsString()}},
            {static_cast<unsigned short>(VER_SUITE_ENTERPRISE), std::wstring{rm.MainResourceMap().GetValue(L"Resources/VER_SUITE_ENTERPRISE").ValueAsString()}},
            {static_cast<unsigned short>(VER_SUITE_BACKOFFICE), std::wstring{rm.MainResourceMap().GetValue(L"Resources/VER_SUITE_BACKOFFICE").ValueAsString()}},
            {static_cast<unsigned short>(VER_SUITE_COMMUNICATIONS), std::wstring{rm.MainResourceMap().GetValue(L"Resources/VER_SUITE_COMMUNICATIONS").ValueAsString()}},
            {static_cast<unsigned short>(VER_SUITE_TERMINAL | VER_SUITE_SINGLEUSERTS), std::wstring{rm.MainResourceMap().GetValue(L"Resources/VER_SUITE_TERMINAL_VER_SUITE_SINGLEUSERTS").ValueAsString()}},
            {static_cast<unsigned short>(VER_SUITE_TERMINAL), std::wstring{rm.MainResourceMap().GetValue(L"Resources/VER_SUITE_TERMINAL").ValueAsString()}},
            {static_cast<unsigned short>(VER_SUITE_SINGLEUSERTS), std::wstring{rm.MainResourceMap().GetValue(L"Resources/VER_SUITE_SINGLEUSERTS").ValueAsString()}},
            {static_cast<unsigned short>(VER_SUITE_SMALLBUSINESS_RESTRICTED), std::wstring{rm.MainResourceMap().GetValue(L"Resources/VER_SUITE_SMALLBUSINESS_RESTRICTED").ValueAsString()}},
            {static_cast<unsigned short>(VER_SUITE_EMBEDDEDNT), std::wstring{rm.MainResourceMap().GetValue(L"Resources/VER_SUITE_EMBEDDEDNT").ValueAsString()}},
            {static_cast<unsigned short>(VER_SUITE_DATACENTER), std::wstring{rm.MainResourceMap().GetValue(L"Resources/VER_SUITE_DATACENTER").ValueAsString()}},
            {static_cast<unsigned short>(VER_SUITE_PERSONAL), std::wstring{rm.MainResourceMap().GetValue(L"Resources/VER_SUITE_PERSONAL").ValueAsString()}},
            {static_cast<unsigned short>(VER_SUITE_BLADE), std::wstring{rm.MainResourceMap().GetValue(L"Resources/VER_SUITE_BLADE").ValueAsString()}},
            {static_cast<unsigned short>(VER_SUITE_EMBEDDED_RESTRICTED), std::wstring{rm.MainResourceMap().GetValue(L"Resources/VER_SUITE_EMBEDDED_RESTRICTED").ValueAsString()}},
            {static_cast<unsigned short>(VER_SUITE_SECURITY_APPLIANCE), std::wstring{rm.MainResourceMap().GetValue(L"Resources/VER_SUITE_SECURITY_APPLIANCE").ValueAsString()}},
            {static_cast<unsigned short>(VER_SUITE_STORAGE_SERVER), std::wstring{rm.MainResourceMap().GetValue(L"Resources/VER_SUITE_STORAGE_SERVER").ValueAsString()}},
            {static_cast<unsigned short>(VER_SUITE_COMPUTE_SERVER), std::wstring{rm.MainResourceMap().GetValue(L"Resources/VER_SUITE_COMPUTE_SERVER").ValueAsString()}},
            {static_cast<unsigned short>(VER_SUITE_WH_SERVER), std::wstring{rm.MainResourceMap().GetValue(L"Resources/VER_SUITE_WH_SERVER").ValueAsString()}},
            {static_cast<unsigned short>(VER_SUITE_MULTIUSERTS), std::wstring{rm.MainResourceMap().GetValue(L"Resources/VER_SUITE_MULTIUSERTS").ValueAsString()}},
        };

        return suite_masks;
    }

    std::unordered_map<uint32_t, std::wstring> const& timezone_ids()
    {
        winrt::Microsoft::Windows::ApplicationModel::Resources::ResourceManager const rm{};
        static std::unordered_map<uint32_t, std::wstring> const timezone_ids =
        {
            {static_cast<unsigned short>(TIME_ZONE_ID_UNKNOWN), std::wstring{rm.MainResourceMap().GetValue(L"Resources/TIME_ZONE_ID_UNKNOWN").ValueAsString()}},
            {static_cast<unsigned short>(TIME_ZONE_ID_STANDARD), std::wstring{rm.MainResourceMap().GetValue(L"Resources/TIME_ZONE_ID_STANDARD").ValueAsString()}},
            {static_cast<unsigned short>(TIME_ZONE_ID_DAYLIGHT), std::wstring{rm.MainResourceMap().GetValue(L"Resources/TIME_ZONE_ID_DAYLIGHT").ValueAsString()}},
        };

        return timezone_ids;
    }

    std::unordered_map<uint32_t, std::wstring> const& process_integrity_levels()
    {
        winrt::Microsoft::Windows::ApplicationModel::Resources::ResourceManager const rm{};
        static std::unordered_map<uint32_t, std::wstring> const process_integrity_levels =
        {
            {SECURITY_MANDATORY_UNTRUSTED_RID, std::wstring{rm.MainResourceMap().GetValue(L"Resources/SECURITY_MANDATORY_UNTRUSTED_RID").ValueAsString()}},
            {SECURITY_MANDATORY_LOW_RID, std::wstring{rm.MainResourceMap().GetValue(L"Resources/SECURITY_MANDATORY_LOW_RID").ValueAsString()}},
            {SECURITY_MANDATORY_MEDIUM_RID, std::wstring{rm.MainResourceMap().GetValue(L"Resources/SECURITY_MANDATORY_MEDIUM_RID").ValueAsString()}},
            {SECURITY_MANDATORY_MEDIUM_PLUS_RID, std::wstring{rm.MainResourceMap().GetValue(L"Resources/SECURITY_MANDATORY_MEDIUM_PLUS_RID").ValueAsString()}},
            {SECURITY_MANDATORY_HIGH_RID, std::wstring{rm.MainResourceMap().GetValue(L"Resources/SECURITY_MANDATORY_HIGH_RID").ValueAsString()}},
            {SECURITY_MANDATORY_SYSTEM_RID, std::wstring{rm.MainResourceMap().GetValue(L"Resources/SECURITY_MANDATORY_SYSTEM_RID").ValueAsString()}},
            {SECURITY_MANDATORY_PROTECTED_PROCESS_RID, std::wstring{rm.MainResourceMap().GetValue(L"Resources/SECURITY_MANDATORY_PROTECTED_PROCESS_RID").ValueAsString()}},
        };

        return process_integrity_levels;
    }

    std::unordered_map<eHANDLE_TRACE_OPERATIONS, std::wstring> const& trace_operations()
    {
        winrt::Microsoft::Windows::ApplicationModel::Resources::ResourceManager const rm{};
        static std::unordered_map<eHANDLE_TRACE_OPERATIONS, std::wstring> const trace_operations =
        {
            {OperationDbUnused, std::wstring{rm.MainResourceMap().GetValue(L"Resources/OperationDbUnused").ValueAsString()}},
            {OperationDbOPEN, std::wstring{rm.MainResourceMap().GetValue(L"Resources/OperationDbOPEN").ValueAsString()}},
            {OperationDbCLOSE, std::wstring{rm.MainResourceMap().GetValue(L"Resources/OperationDbCLOSE").ValueAsString()}},
            {OperationDbBADREF, std::wstring{rm.MainResourceMap().GetValue(L"Resources/OperationDbBADREF").ValueAsString()}},
        };

        return trace_operations;
    }

    std::unordered_map<MINIDUMP_HANDLE_OBJECT_INFORMATION_TYPE, std::wstring> const& object_information_types()
    {
        winrt::Microsoft::Windows::ApplicationModel::Resources::ResourceManager const rm{};
        static std::unordered_map<MINIDUMP_HANDLE_OBJECT_INFORMATION_TYPE, std::wstring> const object_information_types =
        {
            {MiniHandleObjectInformationNone, std::wstring{rm.MainResourceMap().GetValue(L"Resources/MiniHandleObjectInformationNone").ValueAsString()}},
            {MiniThreadInformation1, std::wstring{rm.MainResourceMap().GetValue(L"Resources/MiniThreadInformation1").ValueAsString()}},
            {MiniMutantInformation1, std::wstring{rm.MainResourceMap().GetValue(L"Resources/MiniMutantInformation1").ValueAsString()}},
            {MiniMutantInformation2, std::wstring{rm.MainResourceMap().GetValue(L"Resources/MiniMutantInformation2").ValueAsString()}},
            {MiniProcessInformation1, std::wstring{rm.MainResourceMap().GetValue(L"Resources/MiniProcessInformation1").ValueAsString()}},
            {MiniProcessInformation2, std::wstring{rm.MainResourceMap().GetValue(L"Resources/MiniProcessInformation2").ValueAsString()}},
            {MiniEventInformation1, std::wstring{rm.MainResourceMap().GetValue(L"Resources/MiniEventInformation1").ValueAsString()}},
            {MiniSectionInformation1, std::wstring{rm.MainResourceMap().GetValue(L"Resources/MiniSectionInformation1").ValueAsString()}},
            {MiniSemaphoreInformation1, std::wstring{rm.MainResourceMap().GetValue(L"Resources/MiniSemaphoreInformation1").ValueAsString()}},
            {MiniHandleObjectInformationTypeMax, std::wstring{rm.MainResourceMap().GetValue(L"Resources/MiniHandleObjectInformationTypeMax").ValueAsString()}},
        };

        return object_information_types;
    }

    std::unordered_map<mem_execute_options, std::wstring> const& process_execute_flags()
    {
        winrt::Microsoft::Windows::ApplicationModel::Resources::ResourceManager const rm{};
        static std::unordered_map<mem_execute_options, std::wstring> const process_execute_flags =
        {
            {mem_execute_options::MEM_EXECUTE_OPTION_DISABLE, std::wstring{rm.MainResourceMap().GetValue(L"Resources/MEM_EXECUTE_OPTION_DISABLE").ValueAsString()}},
            {mem_execute_options::MEM_EXECUTE_OPTION_ENABLE, std::wstring{rm.MainResourceMap().GetValue(L"Resources/MEM_EXECUTE_OPTION_ENABLE").ValueAsString()}},
            {mem_execute_options::MEM_EXECUTE_OPTION_DISABLE_THUNK_EMULATION, std::wstring{rm.MainResourceMap().GetValue(L"Resources/MEM_EXECUTE_OPTION_DISABLE_THUNK_EMULATION").ValueAsString()}},
            {mem_execute_options::MEM_EXECUTE_OPTION_PERMANENT, std::wstring{rm.MainResourceMap().GetValue(L"Resources/MEM_EXECUTE_OPTION_PERMANENT").ValueAsString()}},
            {mem_execute_options::MEM_EXECUTE_OPTION_EXECUTE_DISPATCH_ENABLE, std::wstring{rm.MainResourceMap().GetValue(L"Resources/MEM_EXECUTE_OPTION_EXECUTE_DISPATCH_ENABLE").ValueAsString()}},
            {mem_execute_options::MEM_EXECUTE_OPTION_IMAGE_DISPATCH_ENABLE, std::wstring{rm.MainResourceMap().GetValue(L"Resources/MEM_EXECUTE_OPTION_IMAGE_DISPATCH_ENABLE").ValueAsString()}},
        };

        return process_execute_flags;
    }

    std::unordered_map<file_type, std::wstring> const& file_types()
    {
        winrt::Microsoft::Windows::ApplicationModel::Resources::ResourceManager const rm{};
        static std::unordered_map<file_type, std::wstring> const file_types =
        {
            {file_type{VFT_APP}, std::wstring{rm.MainResourceMap().GetValue(L"Resources/VFT_APP").ValueAsString()}},
            {file_type{VFT_DLL}, std::wstring{rm.MainResourceMap().GetValue(L"Resources/VFT_DLL").ValueAsString()}},
            {file_type{VFT_DRV, VFT2_DRV_COMM}, std::wstring{rm.MainResourceMap().GetValue(L"Resources/VFT_DRV_VFT2_DRV_COMM").ValueAsString()}},
            {file_type{VFT_DRV, VFT2_DRV_DISPLAY}, std::wstring{rm.MainResourceMap().GetValue(L"Resources/VFT_DRV_VFT2_DRV_DISPLAY").ValueAsString()}},
            {file_type{VFT_DRV, VFT2_DRV_INSTALLABLE}, std::wstring{rm.MainResourceMap().GetValue(L"Resources/VFT_DRV_VFT2_DRV_INSTALLABLE").ValueAsString()}},
            {file_type{VFT_DRV, VFT2_DRV_KEYBOARD}, std::wstring{rm.MainResourceMap().GetValue(L"Resources/VFT_DRV_VFT2_DRV_KEYBOARD").ValueAsString()}},
            {file_type{VFT_DRV, VFT2_DRV_LANGUAGE}, std::wstring{rm.MainResourceMap().GetValue(L"Resources/VFT_DRV_VFT2_DRV_LANGUAGE").ValueAsString()}},
            {file_type{VFT_DRV, VFT2_DRV_MOUSE}, std::wstring{rm.MainResourceMap().GetValue(L"Resources/VFT_DRV_VFT2_DRV_MOUSE").ValueAsString()}},
            {file_type{VFT_DRV, VFT2_DRV_NETWORK}, std::wstring{rm.MainResourceMap().GetValue(L"Resources/VFT_DRV_VFT2_DRV_NETWORK").ValueAsString()}},
            {file_type{VFT_DRV, VFT2_DRV_PRINTER}, std::wstring{rm.MainResourceMap().GetValue(L"Resources/VFT_DRV_VFT2_DRV_PRINTER").ValueAsString()}},
            {file_type{VFT_DRV, VFT2_DRV_SOUND}, std::wstring{rm.MainResourceMap().GetValue(L"Resources/VFT_DRV_VFT2_DRV_SOUND").ValueAsString()}},
            {file_type{VFT_DRV, VFT2_DRV_SYSTEM}, std::wstring{rm.MainResourceMap().GetValue(L"Resources/VFT_DRV_VFT2_DRV_SYSTEM").ValueAsString()}},
            {file_type{VFT_DRV, VFT2_DRV_VERSIONED_PRINTER}, std::wstring{rm.MainResourceMap().GetValue(L"Resources/VFT_DRV_VFT2_DRV_VERSIONED_PRINTER").ValueAsString()}},
            {file_type{VFT_DRV, VFT2_DRV_INPUTMETHOD}, std::wstring{rm.MainResourceMap().GetValue(L"Resources/VFT_DRV_VFT2_DRV_INPUTMETHOD").ValueAsString()}},
            {file_type{VFT_DRV, VFT2_UNKNOWN}, std::wstring{rm.MainResourceMap().GetValue(L"Resources/VFT_DRV_VFT2_UNKNOWN").ValueAsString()}},
            {file_type{VFT_DRV}, std::wstring{rm.MainResourceMap().GetValue(L"Resources/VFT_DRV").ValueAsString()}},
            {file_type{VFT_FONT, VFT2_FONT_RASTER}, std::wstring{rm.MainResourceMap().GetValue(L"Resources/VFT_FONT_VFT2_FONT_RASTER").ValueAsString()}},
            {file_type{VFT_FONT, VFT2_FONT_TRUETYPE}, std::wstring{rm.MainResourceMap().GetValue(L"Resources/VFT_FONT_VFT2_FONT_TRUETYPE").ValueAsString()}},
            {file_type{VFT_FONT, VFT2_FONT_VECTOR}, std::wstring{rm.MainResourceMap().GetValue(L"Resources/VFT_FONT_VFT2_FONT_VECTOR").ValueAsString()}},
            {file_type{VFT_FONT, VFT2_UNKNOWN}, std::wstring{rm.MainResourceMap().GetValue(L"Resources/VFT_FONT_VFT2_UNKNOWN").ValueAsString()}},
            {file_type{VFT_FONT}, std::wstring{rm.MainResourceMap().GetValue(L"Resources/VFT_FONT").ValueAsString()}},
            {file_type{VFT_STATIC_LIB}, std::wstring{rm.MainResourceMap().GetValue(L"Resources/VFT_STATIC_LIB").ValueAsString()}},
            {file_type{VFT_UNKNOWN}, std::wstring{rm.MainResourceMap().GetValue(L"Resources/VFT_UNKNOWN").ValueAsString()}},
            {file_type{VFT_VXD}, std::wstring{rm.MainResourceMap().GetValue(L"Resources/VFT_VXD").ValueAsString()}},
        };

        return file_types;
    }

    std::unordered_map<SYM_TYPE, std::wstring> const& sym_types()
    {
        winrt::Microsoft::Windows::ApplicationModel::Resources::ResourceManager const rm{};
        static std::unordered_map<SYM_TYPE, std::wstring> const sym_types =
        {
            {SymNone, std::wstring{rm.MainResourceMap().GetValue(L"Resources/SymNone").ValueAsString()}},
            {SymCoff, std::wstring{rm.MainResourceMap().GetValue(L"Resources/SymCoff").ValueAsString()}},
            {SymCv, std::wstring{rm.MainResourceMap().GetValue(L"Resources/SymCv").ValueAsString()}},
            {SymPdb, std::wstring{rm.MainResourceMap().GetValue(L"Resources/SymPdb").ValueAsString()}},
            {SymExport, std::wstring{rm.MainResourceMap().GetValue(L"Resources/SymExport").ValueAsString()}},
            {SymDeferred, std::wstring{rm.MainResourceMap().GetValue(L"Resources/SymDeferred").ValueAsString()}},
            {SymSym, std::wstring{rm.MainResourceMap().GetValue(L"Resources/SymSym").ValueAsString()}},
            {SymDia, std::wstring{rm.MainResourceMap().GetValue(L"Resources/SymDia").ValueAsString()}},
            {SymVirtual, std::wstring{rm.MainResourceMap().GetValue(L"Resources/SymVirtual").ValueAsString()}},
            {NumSymTypes, std::wstring{rm.MainResourceMap().GetValue(L"Resources/NumSymTypes").ValueAsString()}},
        };

        return sym_types;
    }


    std::unordered_map<uint32_t, std::wstring> const& misc_info_flags()
    {
        winrt::Microsoft::Windows::ApplicationModel::Resources::ResourceManager const rm{};
        static std::unordered_map<uint32_t, std::wstring> const misc_info_flags =
        {
            { MINIDUMP_MISC1_PROCESS_ID, std::wstring{rm.MainResourceMap().GetValue(L"Resources/MINIDUMP_MISC1_PROCESS_ID").ValueAsString()}},
            { MINIDUMP_MISC1_PROCESS_TIMES, std::wstring{rm.MainResourceMap().GetValue(L"Resources/MINIDUMP_MISC1_PROCESS_TIMES").ValueAsString()}},
            { MINIDUMP_MISC1_PROCESSOR_POWER_INFO, std::wstring{rm.MainResourceMap().GetValue(L"Resources/MINIDUMP_MISC1_PROCESSOR_POWER_INFO").ValueAsString()}},
            { MINIDUMP_MISC3_PROCESS_INTEGRITY, std::wstring{rm.MainResourceMap().GetValue(L"Resources/MINIDUMP_MISC3_PROCESS_INTEGRITY").ValueAsString()}},
            { MINIDUMP_MISC3_PROCESS_EXECUTE_FLAGS, std::wstring{rm.MainResourceMap().GetValue(L"Resources/MINIDUMP_MISC3_PROCESS_EXECUTE_FLAGS").ValueAsString()}},
            { MINIDUMP_MISC3_TIMEZONE, std::wstring{rm.MainResourceMap().GetValue(L"Resources/MINIDUMP_MISC3_TIMEZONE").ValueAsString()}},
            { MINIDUMP_MISC3_PROTECTED_PROCESS, std::wstring{rm.MainResourceMap().GetValue(L"Resources/MINIDUMP_MISC3_PROTECTED_PROCESS").ValueAsString()}},
            { MINIDUMP_MISC4_BUILDSTRING, std::wstring{rm.MainResourceMap().GetValue(L"Resources/MINIDUMP_MISC4_BUILDSTRING").ValueAsString()}},
            { MINIDUMP_MISC5_PROCESS_COOKIE, std::wstring{rm.MainResourceMap().GetValue(L"Resources/MINIDUMP_MISC5_PROCESS_COOKIE").ValueAsString()}},
        };

        return misc_info_flags;
    }


    std::unordered_map<uint32_t, std::wstring> const& xstate_data_features()
    {
        winrt::Microsoft::Windows::ApplicationModel::Resources::ResourceManager const rm{};
        static std::unordered_map<uint32_t, std::wstring> const xstate_data_features =
        {
            {XSTATE_LEGACY_FLOATING_POINT, std::wstring{rm.MainResourceMap().GetValue(L"Resources/XSTATE_LEGACY_FLOATING_POINT").ValueAsString()}},
            {XSTATE_LEGACY_SSE, std::wstring{rm.MainResourceMap().GetValue(L"Resources/XSTATE_LEGACY_SSE").ValueAsString()}},
            {XSTATE_AVX, std::wstring{rm.MainResourceMap().GetValue(L"Resources/XSTATE_AVX").ValueAsString()}},
            {XSTATE_MPX_BNDREGS, std::wstring{rm.MainResourceMap().GetValue(L"Resources/XSTATE_MPX_BNDREGS").ValueAsString()}},
            {XSTATE_MPX_BNDCSR, std::wstring{rm.MainResourceMap().GetValue(L"Resources/XSTATE_MPX_BNDCSR").ValueAsString()}},
            {XSTATE_AVX512_KMASK, std::wstring{rm.MainResourceMap().GetValue(L"Resources/XSTATE_AVX512_KMASK").ValueAsString()}},
            {XSTATE_AVX512_ZMM_H, std::wstring{rm.MainResourceMap().GetValue(L"Resources/XSTATE_AVX512_ZMM_H").ValueAsString()}},
            {XSTATE_AVX512_ZMM, std::wstring{rm.MainResourceMap().GetValue(L"Resources/XSTATE_AVX512_ZMM").ValueAsString()}},
            {XSTATE_IPT, std::wstring{rm.MainResourceMap().GetValue(L"Resources/XSTATE_IPT").ValueAsString()}},
            {XSTATE_CET_U, std::wstring{rm.MainResourceMap().GetValue(L"Resources/XSTATE_CET_U").ValueAsString()}},
            {XSTATE_CET_S, std::wstring{rm.MainResourceMap().GetValue(L"Resources/XSTATE_CET_S").ValueAsString()}},
            {XSTATE_AMX_TILE_CONFIG, std::wstring{rm.MainResourceMap().GetValue(L"Resources/XSTATE_AMX_TILE_CONFIG").ValueAsString()}},
            {XSTATE_AMX_TILE_DATA, std::wstring{rm.MainResourceMap().GetValue(L"Resources/XSTATE_AMX_TILE_DATA").ValueAsString()}},
            {XSTATE_LWP, std::wstring{rm.MainResourceMap().GetValue(L"Resources/XSTATE_LWP").ValueAsString()}},
        };

        return xstate_data_features;
    }


    std::unordered_map<uint64_t, std::wstring> const& other_processor_features()
    {
        winrt::Microsoft::Windows::ApplicationModel::Resources::ResourceManager const rm{};
        static std::unordered_map<uint64_t, std::wstring> const other_processor_features =
        {
            {1ULL << PF_FLOATING_POINT_PRECISION_ERRATA, std::wstring{rm.MainResourceMap().GetValue(L"Resources/PF_FLOATING_POINT_PRECISION_ERRATA").ValueAsString()}},
            {1ULL << PF_FLOATING_POINT_EMULATED, std::wstring{rm.MainResourceMap().GetValue(L"Resources/PF_FLOATING_POINT_EMULATED").ValueAsString()}},
            {1ULL << PF_COMPARE_EXCHANGE_DOUBLE, std::wstring{rm.MainResourceMap().GetValue(L"Resources/PF_COMPARE_EXCHANGE_DOUBLE").ValueAsString()}},
            {1ULL << PF_MMX_INSTRUCTIONS_AVAILABLE, std::wstring{rm.MainResourceMap().GetValue(L"Resources/PF_MMX_INSTRUCTIONS_AVAILABLE").ValueAsString()}},
            {1ULL << PF_PPC_MOVEMEM_64BIT_OK, std::wstring{rm.MainResourceMap().GetValue(L"Resources/PF_PPC_MOVEMEM_64BIT_OK").ValueAsString()}},
            {1ULL << PF_ALPHA_BYTE_INSTRUCTIONS, std::wstring{rm.MainResourceMap().GetValue(L"Resources/PF_ALPHA_BYTE_INSTRUCTIONS").ValueAsString()}},
            {1ULL << PF_XMMI_INSTRUCTIONS_AVAILABLE, std::wstring{rm.MainResourceMap().GetValue(L"Resources/PF_XMMI_INSTRUCTIONS_AVAILABLE").ValueAsString()}},
            {1ULL << PF_3DNOW_INSTRUCTIONS_AVAILABLE, std::wstring{rm.MainResourceMap().GetValue(L"Resources/PF_3DNOW_INSTRUCTIONS_AVAILABLE").ValueAsString()}},
            {1ULL << PF_RDTSC_INSTRUCTION_AVAILABLE, std::wstring{rm.MainResourceMap().GetValue(L"Resources/PF_RDTSC_INSTRUCTION_AVAILABLE").ValueAsString()}},
            {1ULL << PF_PAE_ENABLED, std::wstring{rm.MainResourceMap().GetValue(L"Resources/PF_PAE_ENABLED").ValueAsString()}},
            {1ULL << PF_XMMI64_INSTRUCTIONS_AVAILABLE, std::wstring{rm.MainResourceMap().GetValue(L"Resources/PF_XMMI64_INSTRUCTIONS_AVAILABLE").ValueAsString()}},
            {1ULL << PF_SSE_DAZ_MODE_AVAILABLE, std::wstring{rm.MainResourceMap().GetValue(L"Resources/PF_SSE_DAZ_MODE_AVAILABLE").ValueAsString()}},
            {1ULL << PF_NX_ENABLED, std::wstring{rm.MainResourceMap().GetValue(L"Resources/PF_NX_ENABLED").ValueAsString()}},
            {1ULL << PF_SSE3_INSTRUCTIONS_AVAILABLE, std::wstring{rm.MainResourceMap().GetValue(L"Resources/PF_SSE3_INSTRUCTIONS_AVAILABLE").ValueAsString()}},
            {1ULL << PF_COMPARE_EXCHANGE128, std::wstring{rm.MainResourceMap().GetValue(L"Resources/PF_COMPARE_EXCHANGE128").ValueAsString()}},
            {1ULL << PF_COMPARE64_EXCHANGE128, std::wstring{rm.MainResourceMap().GetValue(L"Resources/PF_COMPARE64_EXCHANGE128").ValueAsString()}},
            {1ULL << PF_CHANNELS_ENABLED, std::wstring{rm.MainResourceMap().GetValue(L"Resources/PF_CHANNELS_ENABLED").ValueAsString()}},
            {1ULL << PF_XSAVE_ENABLED, std::wstring{rm.MainResourceMap().GetValue(L"Resources/PF_XSAVE_ENABLED").ValueAsString()}},
            {1ULL << PF_ARM_VFP_32_REGISTERS_AVAILABLE, std::wstring{rm.MainResourceMap().GetValue(L"Resources/PF_ARM_VFP_32_REGISTERS_AVAILABLE").ValueAsString()}},
            {1ULL << PF_ARM_NEON_INSTRUCTIONS_AVAILABLE, std::wstring{rm.MainResourceMap().GetValue(L"Resources/PF_ARM_NEON_INSTRUCTIONS_AVAILABLE").ValueAsString()}},
            {1ULL << PF_SECOND_LEVEL_ADDRESS_TRANSLATION, std::wstring{rm.MainResourceMap().GetValue(L"Resources/PF_SECOND_LEVEL_ADDRESS_TRANSLATION").ValueAsString()}},
            {1ULL << PF_VIRT_FIRMWARE_ENABLED, std::wstring{rm.MainResourceMap().GetValue(L"Resources/PF_VIRT_FIRMWARE_ENABLED").ValueAsString()}},
            {1ULL << PF_RDWRFSGSBASE_AVAILABLE, std::wstring{rm.MainResourceMap().GetValue(L"Resources/PF_RDWRFSGSBASE_AVAILABLE").ValueAsString()}},
            {1ULL << PF_FASTFAIL_AVAILABLE, std::wstring{rm.MainResourceMap().GetValue(L"Resources/PF_FASTFAIL_AVAILABLE").ValueAsString()}},
            {1ULL << PF_ARM_DIVIDE_INSTRUCTION_AVAILABLE, std::wstring{rm.MainResourceMap().GetValue(L"Resources/PF_ARM_DIVIDE_INSTRUCTION_AVAILABLE").ValueAsString()}},
            {1ULL << PF_ARM_64BIT_LOADSTORE_ATOMIC, std::wstring{rm.MainResourceMap().GetValue(L"Resources/PF_ARM_64BIT_LOADSTORE_ATOMIC").ValueAsString()}},
            {1ULL << PF_ARM_EXTERNAL_CACHE_AVAILABLE, std::wstring{rm.MainResourceMap().GetValue(L"Resources/PF_ARM_EXTERNAL_CACHE_AVAILABLE").ValueAsString()}},
            {1ULL << PF_ARM_FMAC_INSTRUCTIONS_AVAILABLE, std::wstring{rm.MainResourceMap().GetValue(L"Resources/PF_ARM_FMAC_INSTRUCTIONS_AVAILABLE").ValueAsString()}},
            {1ULL << PF_RDRAND_INSTRUCTION_AVAILABLE, std::wstring{rm.MainResourceMap().GetValue(L"Resources/PF_RDRAND_INSTRUCTION_AVAILABLE").ValueAsString()}},
            {1ULL << PF_ARM_V8_INSTRUCTIONS_AVAILABLE, std::wstring{rm.MainResourceMap().GetValue(L"Resources/PF_ARM_V8_INSTRUCTIONS_AVAILABLE").ValueAsString()}},
            {1ULL << PF_ARM_V8_CRYPTO_INSTRUCTIONS_AVAILABLE, std::wstring{rm.MainResourceMap().GetValue(L"Resources/PF_ARM_V8_CRYPTO_INSTRUCTIONS_AVAILABLE").ValueAsString()}},
            {1ULL << PF_ARM_V8_CRC32_INSTRUCTIONS_AVAILABLE, std::wstring{rm.MainResourceMap().GetValue(L"Resources/PF_ARM_V8_CRC32_INSTRUCTIONS_AVAILABLE").ValueAsString()}},
            {1ULL << PF_RDTSCP_INSTRUCTION_AVAILABLE, std::wstring{rm.MainResourceMap().GetValue(L"Resources/PF_RDTSCP_INSTRUCTION_AVAILABLE").ValueAsString()}},
            {1ULL << PF_RDPID_INSTRUCTION_AVAILABLE, std::wstring{rm.MainResourceMap().GetValue(L"Resources/PF_RDPID_INSTRUCTION_AVAILABLE").ValueAsString()}},
            {1ULL << PF_ARM_V81_ATOMIC_INSTRUCTIONS_AVAILABLE, std::wstring{rm.MainResourceMap().GetValue(L"Resources/PF_ARM_V81_ATOMIC_INSTRUCTIONS_AVAILABLE").ValueAsString()}},
            {1ULL << PF_MONITORX_INSTRUCTION_AVAILABLE, std::wstring{rm.MainResourceMap().GetValue(L"Resources/PF_MONITORX_INSTRUCTION_AVAILABLE").ValueAsString()}},
            {1ULL << PF_SSSE3_INSTRUCTIONS_AVAILABLE, std::wstring{rm.MainResourceMap().GetValue(L"Resources/PF_SSSE3_INSTRUCTIONS_AVAILABLE").ValueAsString()}},
            {1ULL << PF_SSE4_1_INSTRUCTIONS_AVAILABLE, std::wstring{rm.MainResourceMap().GetValue(L"Resources/PF_SSE4_1_INSTRUCTIONS_AVAILABLE").ValueAsString()}},
            {1ULL << PF_SSE4_2_INSTRUCTIONS_AVAILABLE, std::wstring{rm.MainResourceMap().GetValue(L"Resources/PF_SSE4_2_INSTRUCTIONS_AVAILABLE").ValueAsString()}},
            {1ULL << PF_AVX_INSTRUCTIONS_AVAILABLE, std::wstring{rm.MainResourceMap().GetValue(L"Resources/PF_AVX_INSTRUCTIONS_AVAILABLE").ValueAsString()}},
            {1ULL << PF_AVX2_INSTRUCTIONS_AVAILABLE, std::wstring{rm.MainResourceMap().GetValue(L"Resources/PF_AVX2_INSTRUCTIONS_AVAILABLE").ValueAsString()}},
            {1ULL << PF_AVX512F_INSTRUCTIONS_AVAILABLE, std::wstring{rm.MainResourceMap().GetValue(L"Resources/PF_AVX512F_INSTRUCTIONS_AVAILABLE").ValueAsString()}},
            {1ULL << PF_ERMS_AVAILABLE, std::wstring{rm.MainResourceMap().GetValue(L"Resources/PF_ERMS_AVAILABLE").ValueAsString()}},
            {1ULL << PF_ARM_V82_DP_INSTRUCTIONS_AVAILABLE, std::wstring{rm.MainResourceMap().GetValue(L"Resources/PF_ARM_V82_DP_INSTRUCTIONS_AVAILABLE").ValueAsString()}},
            {1ULL << PF_ARM_V83_JSCVT_INSTRUCTIONS_AVAILABLE, std::wstring{rm.MainResourceMap().GetValue(L"Resources/PF_ARM_V83_JSCVT_INSTRUCTIONS_AVAILABLE").ValueAsString()}},
            {1ULL << PF_ARM_V83_LRCPC_INSTRUCTIONS_AVAILABLE, std::wstring{rm.MainResourceMap().GetValue(L"Resources/PF_ARM_V83_LRCPC_INSTRUCTIONS_AVAILABLE").ValueAsString()}},
        };

        return other_processor_features;
    }
}

namespace dlg_help_utils::system_info_utils
{
    namespace resources
    {
        std::wstring to_wstring(platform_type const type)
        {
            return flags_string_utils::generate_enum_string(type, platform_types());
        }
    }

    std::wstring mini_dump_type_to_string(dump_file_type const type)
    {
        return flags_string_utils::generate_enum_string(type, dump_file_types());
    }

    std::wstring processor_architecture_to_string(unsigned short const processor_architecture)
    {
        return flags_string_utils::generate_enum_string(processor_architecture, processor_architectures());
    }

    std::wstring product_type_to_string(unsigned char const product_type)
    {
        return flags_string_utils::generate_enum_string(product_type, product_types());
    }

    std::wstring platform_id_to_string(uint32_t const platform_id)
    {
        return flags_string_utils::generate_enum_string(platform_id, platform_ids());
    }


    std::wstring windows_version_to_string(uint32_t const major_version, uint32_t const minor_version,
                                                uint32_t const build_number, unsigned char const product_type,
                                                unsigned short const processor_architecture,
                                                unsigned short const suite_mask)
    {
        windows_version const key{major_version, minor_version, build_number, product_type, processor_architecture, suite_mask};
        return flags_string_utils::generate_enum_string(key, version_strings());
    }

    std::vector<std::wstring> suite_mask_to_strings(unsigned short const suite_mask)  // NOLINT(misc-use-internal-linkage)
    {
        return generate_flags_strings(suite_mask, suite_masks(), flags_string_utils::mask_used_flags_t{true});
    }

    std::wstring misc_info_flags_to_string(uint32_t const flags)
    {
        return generate_flags_string(flags, misc_info_flags(), flags_string_utils::mask_used_flags_t{true});
    }

    std::vector<std::wstring> misc_info_flags_to_strings(uint32_t const flags)  // NOLINT(misc-use-internal-linkage)
    {
        return generate_flags_strings(flags, misc_info_flags(), flags_string_utils::mask_used_flags_t{true});
    }

    std::wstring time_zone_id_to_string(uint32_t const timezone_id)
    {
        return flags_string_utils::generate_enum_string(timezone_id, timezone_ids());
    }

    std::wstring process_integrity_level_to_string(uint32_t const process_integrity_level)
    {
        return flags_string_utils::generate_enum_string(process_integrity_level, process_integrity_levels());
    }

    std::wstring vm_counters_2_flags_to_string(uint16_t const flags)
    {
        return generate_flags_string(flags, vm_counters_2_flag_masks(), flags_string_utils::mask_used_flags_t{true});
    }

    std::vector<std::wstring> vm_counters_2_flags_to_strings(uint16_t const flags)  // NOLINT(misc-use-internal-linkage)
    {
        return generate_flags_strings(flags, vm_counters_2_flag_masks(), flags_string_utils::mask_used_flags_t{true});
    }

    std::wstring handle_trace_operation_to_string(uint32_t const type)
    {
        return flags_string_utils::generate_enum_string(static_cast<eHANDLE_TRACE_OPERATIONS>(type), trace_operations());
    }

    std::wstring handle_object_information_type_to_string(uint32_t type)
    {
        return flags_string_utils::generate_enum_string(static_cast<MINIDUMP_HANDLE_OBJECT_INFORMATION_TYPE>(type), object_information_types());
    }

    std::vector<std::wstring> process_execute_flags_to_strings(uint32_t const flags)  // NOLINT(misc-use-internal-linkage)
    {
        return generate_flags_strings(static_cast<mem_execute_options>(flags), process_execute_flags(), flags_string_utils::mask_used_flags_t{true});
    }

    std::wstring version_file_flags_to_string(uint32_t const flags, uint32_t const mask)
    {
        return generate_flags_string(flags & mask, version_file_flag_masks(), flags_string_utils::mask_used_flags_t{true});
    }

    std::vector<std::wstring> version_file_flags_to_strings(uint32_t const flags, uint32_t const mask)  // NOLINT(misc-use-internal-linkage)
    {
        return generate_flags_strings(flags & mask, version_file_flag_masks(), flags_string_utils::mask_used_flags_t{true});
    }

    std::wstring version_file_os_to_string(uint32_t const file_os)
    {
        return generate_flags_string(file_os, version_file_os_masks(), flags_string_utils::mask_used_flags_t{true});
    }

    std::vector<std::wstring> version_file_os_to_strings(uint32_t const file_os)  // NOLINT(misc-use-internal-linkage)
    {
        return generate_flags_strings(file_os, version_file_os_masks(), flags_string_utils::mask_used_flags_t{true});
    }

    std::wstring version_file_type_to_string(uint32_t const file_type, uint32_t const file_sub_type)
    {
        return flags_string_utils::generate_enum_string(resources::file_type{file_type, file_sub_type}, file_types());
    }

    std::wstring sym_type_to_string(uint32_t const type)
    {
        return flags_string_utils::generate_enum_string(static_cast<SYM_TYPE>(type), sym_types());
    }

    std::wstring xstate_data_feature_to_string(uint32_t const feature)
    {
        return flags_string_utils::generate_enum_string(feature, xstate_data_features());
    }

    std::wstring other_processor_features_to_string(uint64_t const features1, [[maybe_unused]] uint64_t const features2)
    {
        return generate_flags_string(features1, other_processor_features(), flags_string_utils::mask_used_flags_t{true});
    }

    std::vector<std::wstring> other_processor_features_to_strings(uint64_t const features1, [[maybe_unused]] uint64_t const features2)  // NOLINT(misc-use-internal-linkage)
    {
        return generate_flags_strings(features1, other_processor_features(), flags_string_utils::mask_used_flags_t{true});
    }
}
