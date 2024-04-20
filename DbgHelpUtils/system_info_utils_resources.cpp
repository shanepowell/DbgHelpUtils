#include "system_info_utils.h"
#include "system_info_utils_resources.h"

#include "flags_string_utils.h"
#include "mini_dump.h"

#include <avrfsdk.h>
#include <map>
#include <string>
#include <unordered_map>


using namespace dlg_help_utils::system_info_utils::resources;
using namespace std::string_literals;
using namespace std::string_view_literals;

namespace
{
    std::unordered_map<platform_type, std::wstring> const g_platform_types =
    {
        {platform_type::workstation, L"workstation"s},
        {platform_type::home_server, L"home server"s},
        {platform_type::professional_edition, L"professional edition"s},
        {platform_type::server, L"server"s},
        {platform_type::server_2019, L"server 2019"s},
        {platform_type::server_v_next, L"server v next"s},
    };

    using namespace dlg_help_utils::system_info_utils::resources::windows_versions;

    // see https://en.wikipedia.org/wiki/List_of_Microsoft_Windows_versions
    std::unordered_map<windows_version, std::wstring> const g_version_strings =
    {
        // see https://en.wikipedia.org/wiki/Windows_Server_2022
        {windows_server_2022, L"Windows Server 2022"s},
        {windows_server_2022_preview, L"Windows Server 2022 (preview)"s},

        // see https://en.wikipedia.org/wiki/Windows_Server_2019
        {windows_server_2019_2004, L"Windows Server 2019 2004"s},
        {windows_server_2019_2009_20H2, L"Windows Server 2019 2009 20H2"s},
        {windows_server_2019_1903, L"Windows Server 2019 1903"s},
        {windows_server_2019_1909, L"Windows Server 2019 1909"s},
        {windows_server_2019_20H2, L"Windows Server 2019 20H2"s},
        {windows_server_vnext, L"Windows Server vNext"s},
        {windows_server_2019, L"Windows Server 2019"s},
        {windows_server_2019_datacenter, L"Windows Server 2019 (Semi-Annual Channel) (Datacenter, Standard)"s},

        {windows_server_2016_fall_creators_update, L"Windows Server 2016 Fall Creators Update"s},
        {windows_server_2016, L"Windows Server 2016"s},

        {windows_server_2012_R2, L"Windows Server 2012 R2"s},
        {windows_server_2012, L"Windows Server 2012"s},
        {windows_server_2008_R2, L"Windows Server 2008 R2"s},
        {windows_server_2008, L"Windows Server 2008"s},
        {windows_server_home_server, L"Windows Home Server"s},
        {windows_server_2003, L"Windows Server 2003"s},

        // see https://en.wikipedia.org/wiki/Windows_11_version_history
        {windows_11_23H2, L"Windows 11 23H2"s},
        {windows_11_22H2, L"Windows 11 22H2"s},
        {windows_11, L"Windows 11"s},

        // see https://en.wikipedia.org/wiki/Windows_10_version_history
        {windows_10_22H2, L"Windows 10 22H2"s},
        {windows_10_21H2, L"Windows 10 21H2"s},
        {windows_10_21H1, L"Windows 10 21H1"s},
        {windows_10_20H2, L"Windows 10 20H2"s},
        {windows_10_2004, L"Windows 10 2004"s},
        {windows_10_1909, L"Windows 10 1909"s},
        {windows_10_1903, L"Windows 10 1903"s},
        {windows_10_october_2018, L"Windows 10 October 2018"s},
        {windows_10_april_2018, L"Windows 10 April 2018"s},
        {windows_10_fall_creators_update, L"Windows 10 Fall Creators Update"s},
        {windows_10_creators_update, L"Windows 10 Creators Update"s},
        {windows_10_anniversary_update, L"Windows 10 Anniversary Update"s},
        {windows_10_november_update, L"Windows 10 November Update"s},
        {windows_10_gold, L"Windows 10 Gold"s},
        {windows_10, L"Windows 10"s},

        {windows_8_1, L"Windows 8.1"s},
        {windows_8, L"Windows 8"s},

        {windows_7, L"Windows 7"s},
        {windows_vista, L"Windows Vista"s},
        {windows_server, L"Windows Server"s},
        {windows_xp, L"Windows XP"s},
        {windows_2000, L"Windows 2000"s},
    };

    std::unordered_map<uint16_t, std::wstring> const g_vm_counters_2_flag_masks =
    {
        {static_cast<uint16_t>(MINIDUMP_PROCESS_VM_COUNTERS), L"vm counters"s},
        {static_cast<uint16_t>(MINIDUMP_PROCESS_VM_COUNTERS_VIRTUALSIZE), L"vm counters virtual size"s},
        {static_cast<uint16_t>(MINIDUMP_PROCESS_VM_COUNTERS_EX), L"vm counters extension"s},
        {static_cast<uint16_t>(MINIDUMP_PROCESS_VM_COUNTERS_EX2), L"vm counters extension 2"s},
        {static_cast<uint16_t>(MINIDUMP_PROCESS_VM_COUNTERS_JOB), L"vm counters job"s},
    };

    std::unordered_map<uint32_t, std::wstring> const g_version_file_flag_masks =
    {
        {static_cast<uint32_t>(VS_FF_DEBUG), L"debug build"s},
        {static_cast<uint32_t>(VS_FF_INFOINFERRED), L"info inferred build"s},
        {static_cast<uint32_t>(VS_FF_PATCHED), L"patched build"s},
        {static_cast<uint32_t>(VS_FF_PRERELEASE), L"pre-release build"s},
        {static_cast<uint32_t>(VS_FF_PRIVATEBUILD), L"private build"s},
        {static_cast<uint32_t>(VS_FF_SPECIALBUILD), L"special build"s},
    };

    std::unordered_map<uint32_t, std::wstring> const g_version_file_os_masks =
    {
        {static_cast<uint32_t>(VOS__WINDOWS16), L"16bit Windows"s},
        {static_cast<uint32_t>(VOS__WINDOWS32), L"32bit Windows"s},
        {static_cast<uint32_t>(VOS__PM16), L"16bit Presentation Manager"s},
        {static_cast<uint32_t>(VOS__PM32), L"32bit Presentation Manager"s},
        {static_cast<uint32_t>(VOS_DOS), L"MS-DOS"s},
        {static_cast<uint32_t>(VOS_NT), L"Windows NT"s},
        {static_cast<uint32_t>(VOS_OS216), L"16bit OS/2"s},
        {static_cast<uint32_t>(VOS_OS232), L"32bit OS/2"s},
        {static_cast<uint32_t>(VOS_WINCE), L"Windows CE"s},
    };

    std::unordered_map<dlg_help_utils::dump_file_type, std::wstring> g_dump_file_types =
    {
        {dlg_help_utils::dump_file_type::invalid, L"invalid"s},
        {dlg_help_utils::dump_file_type::user_mode_dump, L"user mode dump"s},
        {dlg_help_utils::dump_file_type::kernel_mode_dump_x86, L"kernel mode x86 dump"s},
        {dlg_help_utils::dump_file_type::kernel_mode_dump_x64, L"kernel mode x64 dump"s},
    };

    std::unordered_map<unsigned short, std::wstring> g_processor_architectures =
    {
        {static_cast<unsigned short>(PROCESSOR_ARCHITECTURE_INTEL), L"x86"s},
        {static_cast<unsigned short>(PROCESSOR_ARCHITECTURE_MIPS), L"mips"s},
        {static_cast<unsigned short>(PROCESSOR_ARCHITECTURE_ALPHA), L"alpha"s},
        {static_cast<unsigned short>(PROCESSOR_ARCHITECTURE_PPC), L"powerpc"s},
        {static_cast<unsigned short>(PROCESSOR_ARCHITECTURE_SHX), L"shx"s},
        {static_cast<unsigned short>(PROCESSOR_ARCHITECTURE_ARM), L"arm"s},
        {static_cast<unsigned short>(PROCESSOR_ARCHITECTURE_IA64), L"itanium"s},
        {static_cast<unsigned short>(PROCESSOR_ARCHITECTURE_ALPHA64), L"alpha64"s},
        {static_cast<unsigned short>(PROCESSOR_ARCHITECTURE_MSIL), L"msil"s},
        {static_cast<unsigned short>(PROCESSOR_ARCHITECTURE_AMD64), L"x64"s},
        {static_cast<unsigned short>(PROCESSOR_ARCHITECTURE_IA32_ON_WIN64), L"x86 on win64"s},
        {static_cast<unsigned short>(PROCESSOR_ARCHITECTURE_NEUTRAL), L"neutral"s},
        {static_cast<unsigned short>(PROCESSOR_ARCHITECTURE_ARM64), L"arm64"s},
        {static_cast<unsigned short>(PROCESSOR_ARCHITECTURE_ARM32_ON_WIN64), L"arm32 on win64"s},
        {static_cast<unsigned short>(PROCESSOR_ARCHITECTURE_IA32_ON_ARM64), L"x86 on arm64"s},
        {static_cast<unsigned short>(PROCESSOR_ARCHITECTURE_UNKNOWN), L"unknown"s},
    };

    std::unordered_map<unsigned char, std::wstring> g_product_types =
    {
        {static_cast<unsigned char>(VER_NT_WORKSTATION), L"NT Workstation"s},
        {static_cast<unsigned char>(VER_NT_SERVER), L"NT Server"s},
        {static_cast<unsigned char>(VER_NT_DOMAIN_CONTROLLER), L"NT Domain Controller"s},
    };

    std::unordered_map<uint32_t, std::wstring> g_platform_ids =
    {
        {VER_PLATFORM_WIN32_NT, L"Win32 NT"s},
        {VER_PLATFORM_WIN32s, L"Win32s"s},
        {VER_PLATFORM_WIN32_WINDOWS, L"Win32"s},
    };

    std::map<unsigned short, std::wstring> g_suite_masks =
    {
        {static_cast<unsigned short>(VER_SUITE_SMALLBUSINESS), L"Small Business Server"s},
        {static_cast<unsigned short>(VER_SUITE_ENTERPRISE), L"Enterprise Edition / Advanced Server"s},
        {static_cast<unsigned short>(VER_SUITE_BACKOFFICE), L"BackOffice"s},
        {static_cast<unsigned short>(VER_SUITE_COMMUNICATIONS), L"Communications"s},
        {static_cast<unsigned short>(VER_SUITE_TERMINAL | VER_SUITE_SINGLEUSERTS), L"Terminal Services"s},
        {static_cast<unsigned short>(VER_SUITE_TERMINAL), L"Application Server"s},
        {static_cast<unsigned short>(VER_SUITE_SINGLEUSERTS), L"Single User"s},
        {static_cast<unsigned short>(VER_SUITE_SMALLBUSINESS_RESTRICTED), L"Small Business Server Restricted"s},
        {static_cast<unsigned short>(VER_SUITE_EMBEDDEDNT), L"Windows XP Embedded"s},
        {static_cast<unsigned short>(VER_SUITE_DATACENTER), L"Datacenter Edition"s},
        {static_cast<unsigned short>(VER_SUITE_PERSONAL), L"Windows Vista Home Premium, Windows Vista Home Basic, or Windows XP Home Edition"s},
        {static_cast<unsigned short>(VER_SUITE_BLADE), L"Web Edition"s},
        {static_cast<unsigned short>(VER_SUITE_EMBEDDED_RESTRICTED), L"Embedded Restricted"s},
        {static_cast<unsigned short>(VER_SUITE_SECURITY_APPLIANCE), L"Security Appliance"s},
        {static_cast<unsigned short>(VER_SUITE_STORAGE_SERVER), L"Storage Server"s},
        {static_cast<unsigned short>(VER_SUITE_COMPUTE_SERVER), L"Compute Server"s},
        {static_cast<unsigned short>(VER_SUITE_WH_SERVER), L"Home Server"s},
        {static_cast<unsigned short>(VER_SUITE_MULTIUSERTS), L"Multi User"s},
    };

    std::unordered_map<uint32_t, std::wstring> g_process_integrity_levels =
    {
        {SECURITY_MANDATORY_UNTRUSTED_RID, L"untrusted"s},
        {SECURITY_MANDATORY_LOW_RID, L"low"s},
        {SECURITY_MANDATORY_MEDIUM_RID, L"normal"s},
        {SECURITY_MANDATORY_MEDIUM_PLUS_RID, L"above normal"s},
        {SECURITY_MANDATORY_HIGH_RID, L"elevated"s},
        {SECURITY_MANDATORY_SYSTEM_RID, L"system"s},
        {SECURITY_MANDATORY_PROTECTED_PROCESS_RID, L"protected"s},
    };

    std::unordered_map<uint32_t, std::wstring> g_timezone_ids =
    {
        {TIME_ZONE_ID_UNKNOWN, L"unknown"s},
        {TIME_ZONE_ID_STANDARD, L"standard"s},
        {TIME_ZONE_ID_DAYLIGHT, L"daylight"s},
    };

    std::unordered_map<eHANDLE_TRACE_OPERATIONS, std::wstring> g_trace_operations =
    {
        {OperationDbUnused, L"Unused"s},
        {OperationDbOPEN, L"OPEN"s},
        {OperationDbCLOSE, L"CLOSE"s},
        {OperationDbBADREF, L"BADREF"s},
    };

    std::unordered_map<MINIDUMP_HANDLE_OBJECT_INFORMATION_TYPE, std::wstring> g_object_information_types =
    {
        {MiniHandleObjectInformationNone, L"None"s},
        {MiniThreadInformation1, L"MiniThreadInformation1"s},
        {MiniMutantInformation1, L"MiniMutantInformation1"s},
        {MiniMutantInformation2, L"MiniMutantInformation2"s},
        {MiniProcessInformation1, L"MiniProcessInformation1"s},
        {MiniProcessInformation2, L"MiniProcessInformation2"s},
        {MiniEventInformation1, L"MiniEventInformation1"s},
        {MiniSectionInformation1, L"MiniSectionInformation1"s},
        {MiniSemaphoreInformation1, L"MiniSemaphoreInformation1"s},
        {MiniHandleObjectInformationTypeMax, L"MiniHandleObjectInformationTypeMax"s},
    };

    std::unordered_map<mem_execute_options, std::wstring> g_process_execute_flags =
    {
        {mem_execute_options::MEM_EXECUTE_OPTION_DISABLE, L"MEM_EXECUTE_OPTION_DISABLE"s},
        {mem_execute_options::MEM_EXECUTE_OPTION_ENABLE, L"MEM_EXECUTE_OPTION_ENABLE"s},
        {mem_execute_options::MEM_EXECUTE_OPTION_DISABLE_THUNK_EMULATION, L"MEM_EXECUTE_OPTION_DISABLE_THUNK_EMULATION"s},
        {mem_execute_options::MEM_EXECUTE_OPTION_PERMANENT, L"MEM_EXECUTE_OPTION_PERMANENT"s},
        {mem_execute_options::MEM_EXECUTE_OPTION_EXECUTE_DISPATCH_ENABLE, L"MEM_EXECUTE_OPTION_EXECUTE_DISPATCH_ENABLE"s},
        {mem_execute_options::MEM_EXECUTE_OPTION_IMAGE_DISPATCH_ENABLE, L"MEM_EXECUTE_OPTION_IMAGE_DISPATCH_ENABLE"s},
    };

    std::unordered_map<file_type, std::wstring> g_file_types =
    {
        {file_type{VFT_APP}, L"Application"s},
        {file_type{VFT_DLL}, L"DLL"s},
        {file_type{VFT_DRV, VFT2_DRV_COMM}, L"Communications Driver"s},
        {file_type{VFT_DRV, VFT2_DRV_DISPLAY}, L"Display Driver"s},
        {file_type{VFT_DRV, VFT2_DRV_INSTALLABLE}, L"Installable Driver"s},
        {file_type{VFT_DRV, VFT2_DRV_KEYBOARD}, L"Keyboard Driver"s},
        {file_type{VFT_DRV, VFT2_DRV_LANGUAGE}, L"Language Driver"s},
        {file_type{VFT_DRV, VFT2_DRV_MOUSE}, L"Mouse Driver"s},
        {file_type{VFT_DRV, VFT2_DRV_NETWORK}, L"Network Driver"s},
        {file_type{VFT_DRV, VFT2_DRV_PRINTER}, L"Printer Driver"s},
        {file_type{VFT_DRV, VFT2_DRV_SOUND}, L"Sound Driver"s},
        {file_type{VFT_DRV, VFT2_DRV_SYSTEM}, L"System Driver"s},
        {file_type{VFT_DRV, VFT2_DRV_VERSIONED_PRINTER}, L"Versioned Printer Driver"s},
        {file_type{VFT_DRV, VFT2_DRV_INPUTMETHOD}, L"Input Method Driver"s},
        {file_type{VFT_DRV, VFT2_UNKNOWN}, L"Unknown Driver"s},
        {file_type{VFT_DRV}, L"Driver"s},
        {file_type{VFT_FONT, VFT2_FONT_RASTER}, L"Raster Font"s},
        {file_type{VFT_FONT, VFT2_FONT_TRUETYPE}, L"Truetype Font"s},
        {file_type{VFT_FONT, VFT2_FONT_VECTOR}, L"Vector Font"s},
        {file_type{VFT_FONT, VFT2_UNKNOWN}, L"Unknown Font"s},
        {file_type{VFT_FONT}, L"Font"s},
        {file_type{VFT_STATIC_LIB}, L"Static-Link Library"s},
        {file_type{VFT_UNKNOWN}, L"Unknown"s},
        {file_type{VFT_VXD}, L"Virtual Device"s},
    };

    std::unordered_map<SYM_TYPE, std::wstring> g_sym_types =
    {
        {SymNone, L"SymNone"s},
        {SymCoff, L"SymCoff"s},
        {SymCv, L"SymCv"s},
        {SymPdb, L"SymPdb"s},
        {SymExport, L"SymExport"s},
        {SymDeferred, L"SymDeferred"s},
        {SymSym, L"SymSym"s},
        {SymDia, L"SymDia"s},
        {SymVirtual, L"SymVirtual"s},
        {NumSymTypes, L"NumSymTypes"s},
    };

    std::unordered_map<uint32_t, std::wstring> g_misc_info_flags =
    {
        { MINIDUMP_MISC1_PROCESS_ID, L"MINIDUMP_MISC1_PROCESS_ID"s},
        { MINIDUMP_MISC1_PROCESS_TIMES, L"MINIDUMP_MISC1_PROCESS_TIMES"s},
        { MINIDUMP_MISC1_PROCESSOR_POWER_INFO, L"MINIDUMP_MISC1_PROCESSOR_POWER_INFO"s},
        { MINIDUMP_MISC3_PROCESS_INTEGRITY, L"MINIDUMP_MISC3_PROCESS_INTEGRITY"s},
        { MINIDUMP_MISC3_PROCESS_EXECUTE_FLAGS, L"MINIDUMP_MISC3_PROCESS_EXECUTE_FLAGS"s},
        { MINIDUMP_MISC3_TIMEZONE, L"MINIDUMP_MISC3_TIMEZONE"s},
        { MINIDUMP_MISC3_PROTECTED_PROCESS, L"MINIDUMP_MISC3_PROTECTED_PROCESS"s},
        { MINIDUMP_MISC4_BUILDSTRING, L"MINIDUMP_MISC4_BUILDSTRING"s},
        { MINIDUMP_MISC5_PROCESS_COOKIE, L"MINIDUMP_MISC5_PROCESS_COOKIE"s},
    };

    std::unordered_map<uint32_t, std::wstring> g_xstate_data_features =
    {
        {XSTATE_LEGACY_FLOATING_POINT, L"XSTATE_LEGACY_FLOATING_POINT"s},
        {XSTATE_LEGACY_SSE, L"XSTATE_LEGACY_SSE"s},
        {XSTATE_AVX, L"XSTATE_AVX"s},
        {XSTATE_MPX_BNDREGS, L"XSTATE_MPX_BNDREGS"s},
        {XSTATE_MPX_BNDCSR, L"XSTATE_MPX_BNDCSR"s},
        {XSTATE_AVX512_KMASK, L"XSTATE_AVX512_KMASK"s},
        {XSTATE_AVX512_ZMM_H, L"XSTATE_AVX512_ZMM_H"s},
        {XSTATE_AVX512_ZMM, L"XSTATE_AVX512_ZMM"s},
        {XSTATE_IPT, L"XSTATE_IPT"s},
        {XSTATE_CET_U, L"XSTATE_CET_U"s},
        {XSTATE_CET_S, L"XSTATE_CET_S"s},
        {XSTATE_AMX_TILE_CONFIG, L"XSTATE_AMX_TILE_CONFIG"s},
        {XSTATE_AMX_TILE_DATA, L"XSTATE_AMX_TILE_DATA"s},
        {XSTATE_LWP, L"XSTATE_LWP"s},
    };
}

namespace dlg_help_utils::system_info_utils
{
    namespace resources
    {
        std::wstring to_wstring(platform_type const type)
        {
            return flags_string_utils::generate_enum_string(type, g_platform_types);
        }
    }

    std::wstring mini_dump_type_to_string(dump_file_type const type)
    {
        return flags_string_utils::generate_enum_string(type, g_dump_file_types);
    }

    std::wstring processor_architecture_to_string(unsigned short const processor_architecture)
    {
        return flags_string_utils::generate_enum_string(processor_architecture, g_processor_architectures);
    }

    std::wstring product_type_to_string(unsigned char const product_type)
    {
        return flags_string_utils::generate_enum_string(product_type, g_product_types);
    }

    std::wstring platform_id_to_string(uint32_t const platform_id)
    {
        return flags_string_utils::generate_enum_string(platform_id, g_platform_ids);
    }


    std::wstring windows_version_to_string(uint32_t const major_version, uint32_t const minor_version,
                                                uint32_t const build_number, unsigned char const product_type,
                                                unsigned short const processor_architecture,
                                                unsigned short const suite_mask)
    {
        windows_version const key{major_version, minor_version, build_number, product_type, processor_architecture, suite_mask};
        return flags_string_utils::generate_enum_string(key, g_version_strings);
    }

    std::vector<std::wstring> suite_mask_to_strings(unsigned short const suite_mask)
    {
        return flags_string_utils::generate_flags_strings(suite_mask, g_suite_masks);
    }

    std::wstring misc_info_flags_to_string(uint32_t const flags)
    {
        return flags_string_utils::generate_flags_string(flags, g_misc_info_flags);
    }

    std::vector<std::wstring> misc_info_flags_to_strings(uint32_t const flags)
    {
        return flags_string_utils::generate_flags_strings(flags, g_misc_info_flags);
    }

    std::wstring time_zone_id_to_string(uint32_t const timezone_id)
    {
        return flags_string_utils::generate_enum_string(timezone_id, g_timezone_ids);
    }

    std::wstring process_integrity_level_to_string(uint32_t const process_integrity_level)
    {
        return flags_string_utils::generate_enum_string(process_integrity_level, g_process_integrity_levels);
    }

    std::wstring vm_counters_2_flags_to_string(uint16_t const flags)
    {
        return flags_string_utils::generate_flags_string(flags, g_vm_counters_2_flag_masks);
    }

    std::vector<std::wstring> vm_counters_2_flags_to_strings(uint16_t const flags)
    {
        return flags_string_utils::generate_flags_strings(flags, g_vm_counters_2_flag_masks);
    }

    std::wstring handle_trace_operation_to_string(uint32_t const type)
    {
        return flags_string_utils::generate_enum_string(static_cast<eHANDLE_TRACE_OPERATIONS>(type), g_trace_operations);
    }

    std::wstring handle_object_information_type_to_string(uint32_t type)
    {
        return flags_string_utils::generate_enum_string(static_cast<MINIDUMP_HANDLE_OBJECT_INFORMATION_TYPE>(type), g_object_information_types);
    }

    std::vector<std::wstring> process_execute_flags_to_strings(uint32_t const flags)
    {
        return flags_string_utils::generate_flags_strings(static_cast<mem_execute_options>(flags), g_process_execute_flags);
    }

    std::wstring version_file_flags_to_string(uint32_t const flags, uint32_t const mask)
    {
        return flags_string_utils::generate_flags_string(flags & mask, g_version_file_flag_masks);
    }

    std::vector<std::wstring> version_file_flags_to_strings(uint32_t const flags, uint32_t const mask)
    {
        return flags_string_utils::generate_flags_strings(flags & mask, g_version_file_flag_masks);
    }

    std::wstring version_file_os_to_string(uint32_t const file_os)
    {
        return flags_string_utils::generate_flags_string(file_os, g_version_file_os_masks);
    }

    std::vector<std::wstring> version_file_os_to_strings(uint32_t const file_os)
    {
        return flags_string_utils::generate_flags_strings(file_os, g_version_file_os_masks);
    }

    std::wstring version_file_type_to_string(uint32_t const file_type, uint32_t const file_sub_type)
    {
        return flags_string_utils::generate_enum_string(resources::file_type{file_type, file_sub_type}, g_file_types);
    }

    std::wstring sym_type_to_string(uint32_t const type)
    {
        return flags_string_utils::generate_enum_string(static_cast<SYM_TYPE>(type), g_sym_types);
    }

    std::wstring xstate_data_feature_to_string(uint32_t const feature)
    {
        return flags_string_utils::generate_enum_string(feature, g_xstate_data_features);
    }
}
