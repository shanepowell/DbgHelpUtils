#include "system_info_utils.h"

// ReSharper disable once CppUnusedIncludeDirective
#include "windows_setup.h"

#include <avrfsdk.h>
#include <unordered_map>

// ReSharper disable once CppUnusedIncludeDirective
#include <compare>

#include "flags_string_utils.h"
#include "hash_combine.h"
#include "mini_dump.h"
#include "stream_hex_dump.h"

// ReSharper disable CppClangTidyCppcoreguidelinesMacroUsage
#define MEM_EXECUTE_OPTION_DISABLE 0x1
#define MEM_EXECUTE_OPTION_ENABLE 0x2
#define MEM_EXECUTE_OPTION_DISABLE_THUNK_EMULATION 0x4
#define MEM_EXECUTE_OPTION_PERMANENT 0x8
#define MEM_EXECUTE_OPTION_EXECUTE_DISPATCH_ENABLE 0x10
#define MEM_EXECUTE_OPTION_IMAGE_DISPATCH_ENABLE 0x20
// #define MEM_EXECUTE_OPTION_VALID_FLAGS 0x3f
// ReSharper enable CppClangTidyCppcoreguidelinesMacroUsage

using namespace std::string_literals;
using namespace std::string_view_literals;

namespace
{
    enum class platform_type
    {
        workstation,
        server,
        server_2019,
        server_v_next,
        home_server,
        professional_edition
    };

    class windows_version
    {
    public:
        constexpr windows_version(uint32_t const major_version, uint32_t const minor_version,
                                  uint32_t const build_number, unsigned char const product_type,
                                  unsigned short const processor_architecture, unsigned short const suite_mask)
            : type_{platform_type::workstation}
              , major_version_{major_version}
              , minor_version_{minor_version}
        {
            if (major_version_ == 10 && minor_version == 0)
            {
                if (product_type == VER_NT_WORKSTATION)
                {
                    type_ = platform_type::workstation;
                }
                else if (build_number > 17763)
                {
                    type_ = platform_type::server_v_next;
                }
                else if (build_number > 17622)
                {
                    type_ = platform_type::server_2019;
                }
                else
                {
                    type_ = platform_type::server;
                }
            }
            else if (major_version_ >= 6)
            {
                type_ = product_type == VER_NT_WORKSTATION ? platform_type::workstation : platform_type::server;
            }
            else if (major_version == 5)
            {
                if (minor_version == 2)
                {
                    if (suite_mask & VER_SUITE_WH_SERVER)
                    {
                        type_ = platform_type::home_server;
                    }
                    else if (product_type == VER_NT_WORKSTATION)
                    {
                        type_ = processor_architecture == PROCESSOR_ARCHITECTURE_AMD64
                                    ? platform_type::professional_edition
                                    : platform_type::server;
                    }
                    else
                    {
                        type_ = platform_type::server;
                    }
                }
                else
                {
                    type_ = product_type == VER_NT_WORKSTATION ? platform_type::workstation : platform_type::server;
                }
            }
            else
            {
                type_ = platform_type::workstation;
            }
        }

        auto operator<=>(windows_version const&) const = default; // NOLINT(clang-diagnostic-unused-member-function)

        [[nodiscard]] std::size_t hash() const
        {
            std::size_t hash{0};
            dlg_help_utils::hash_combine(hash, std::hash<platform_type>{}(type_));
            dlg_help_utils::hash_combine(hash, std::hash<uint32_t>{}(major_version_));
            dlg_help_utils::hash_combine(hash, std::hash<uint32_t>{}(minor_version_));
            return hash;
        }

    private:
        platform_type type_;
        uint32_t const major_version_;
        uint32_t const minor_version_;
    };

    std::filesystem::path init_running_application_path()
    {
        if (wchar_t tmp_buffer[MAX_PATH]; ::GetModuleFileName(nullptr, tmp_buffer, MAX_PATH) > 0)
        {
            return {tmp_buffer};
        }

        return {};
    }

    std::filesystem::path init_temp_directory()
    {
        if (wchar_t tmp_buffer[MAX_PATH + 2]; GetTempPathW(MAX_PATH + 2, tmp_buffer) > 0)
        {
            return {tmp_buffer};
        }

        return {};
    }
}

template <>
struct std::hash<windows_version>
{
    std::size_t operator()(windows_version const& v) const noexcept
    {
        return v.hash();
    }
};

namespace
{
    std::unordered_map<windows_version, std::wstring_view> const version_strings =
    {
        {windows_version{11, 0, 0, VER_NT_WORKSTATION, 0, 0}, L"Windows 11"sv},
        {windows_version{11, 0, 0, VER_NT_SERVER, 0, 0}, L"Windows Server 2022"sv},
        {windows_version{10, 0, 17764, VER_NT_SERVER, 0, 0}, L"Windows Server vNext"sv},
        {windows_version{10, 0, 17623, VER_NT_SERVER, 0, 0}, L"Windows Server 2019"sv},
        {windows_version{10, 0, 0, VER_NT_WORKSTATION, 0, 0}, L"Windows 10"sv},
        {windows_version{10, 0, 0, VER_NT_SERVER, 0, 0}, L"Windows Server 2016"sv},
        {windows_version{6, 3, 0, VER_NT_WORKSTATION, 0, 0}, L"Windows 8.1"sv},
        {windows_version{6, 3, 0, VER_NT_SERVER, 0, 0}, L"Windows Server 2012 R2"sv},
        {windows_version{6, 2, 0, VER_NT_WORKSTATION, 0, 0}, L"Windows 8"sv},
        {windows_version{6, 2, 0, VER_NT_SERVER, 0, 0}, L"Windows Server 2012"sv},
        {windows_version{6, 1, 0, VER_NT_WORKSTATION, 0, 0}, L"Windows 7"sv},
        {windows_version{6, 1, 0, VER_NT_SERVER, 0, 0}, L"Windows Server 2008 R2"sv},
        {windows_version{6, 0, 0, VER_NT_WORKSTATION, 0, 0}, L"Windows Vista"sv},
        {windows_version{6, 0, 0, VER_NT_SERVER, 0, 0}, L"Windows Server 2008"sv},
        {windows_version{5, 2, 0, 0, 0, VER_SUITE_WH_SERVER}, L"Windows Home Server"sv},
        {windows_version{5, 2, 0, 0, 0, 0}, L"Windows Server 2003"sv},
        {windows_version{5, 2, 0, VER_NT_WORKSTATION, 0, 0}, L"Windows Server"sv},
        {windows_version{5, 1, 0, VER_NT_WORKSTATION, 0, 0}, L"Windows XP"sv},
        {windows_version{5, 0, 0, VER_NT_WORKSTATION, 0, 0}, L"Windows 2000"sv},
    };

    std::map<uint16_t, std::wstring_view> const vm_counters_2_flag_masks =
    {
        {static_cast<uint16_t>(MINIDUMP_PROCESS_VM_COUNTERS), L"vm counters"sv},
        {static_cast<uint16_t>(MINIDUMP_PROCESS_VM_COUNTERS_VIRTUALSIZE), L"virtual size"sv},
        {static_cast<uint16_t>(MINIDUMP_PROCESS_VM_COUNTERS_EX), L"ex"sv},
        {static_cast<uint16_t>(MINIDUMP_PROCESS_VM_COUNTERS_EX2), L"ex2"sv},
        {static_cast<uint16_t>(MINIDUMP_PROCESS_VM_COUNTERS_JOB), L"job"sv},
    };

    std::map<uint32_t, std::wstring_view> const version_file_flag_masks =
    {
        {static_cast<uint32_t>(VS_FF_DEBUG), L"debug build"sv},
        {static_cast<uint32_t>(VS_FF_INFOINFERRED), L"info inferred build"sv},
        {static_cast<uint32_t>(VS_FF_PATCHED), L"patched build"sv},
        {static_cast<uint32_t>(VS_FF_PATCHED), L"prerelease build"sv},
        {static_cast<uint32_t>(VS_FF_PRIVATEBUILD), L"private build"sv},
        {static_cast<uint32_t>(VS_FF_SPECIALBUILD), L"special build"sv},
    };

    std::map<uint32_t, std::wstring_view> const version_file_os_masks =
    {
        {static_cast<uint32_t>(VOS__WINDOWS16), L"16bit Windows"sv},
        {static_cast<uint32_t>(VOS__WINDOWS32), L"32bit Windows"sv},
        {static_cast<uint32_t>(VOS__PM16), L"16bit Presentation Manager"sv},
        {static_cast<uint32_t>(VOS__PM32), L"32bit Presentation Manager"sv},
        {static_cast<uint32_t>(VOS_DOS), L"MS-DOS"sv},
        {static_cast<uint32_t>(VOS_NT), L"Windows NT"sv},
        {static_cast<uint32_t>(VOS_OS216), L"16bit OS/2"sv},
        {static_cast<uint32_t>(VOS_OS232), L"32bit OS/2"sv},
        {static_cast<uint32_t>(VOS_WINCE), L"Windows CE"sv},
    };
}


namespace dlg_help_utils::system_info_utils
{
    std::wstring_view mini_dump_type_to_string(dump_file_type const type)
    {
        switch (type)
        {
        case dump_file_type::invalid:
            return L"invalid"sv;
        case dump_file_type::user_mode_dump:
            return L"user mode dump"sv;
        case dump_file_type::kernel_mode_dump_x86:
            return L"kernel mode x86 dump"sv;
        case dump_file_type::kernel_mode_dump_x64:
            return L"kernel mode x64 dump"sv;
        }

        return L"unknown"sv;
    }

    std::wstring processor_architecture_to_string(unsigned short const processor_architecture)
    {
        switch (processor_architecture)
        {
        case PROCESSOR_ARCHITECTURE_INTEL:
            return L"x86"s;
        case PROCESSOR_ARCHITECTURE_MIPS:
            return L"mips"s;
        case PROCESSOR_ARCHITECTURE_ALPHA:
            return L"alpha"s;
        case PROCESSOR_ARCHITECTURE_PPC:
            return L"powerpc"s;
        case PROCESSOR_ARCHITECTURE_SHX:
            return L"shx"s;
        case PROCESSOR_ARCHITECTURE_ARM:
            return L"arm"s;
        case PROCESSOR_ARCHITECTURE_IA64:
            return L"itanium"s;
        case PROCESSOR_ARCHITECTURE_ALPHA64:
            return L"alpha64"s;
        case PROCESSOR_ARCHITECTURE_MSIL:
            return L"msil"s;
        case PROCESSOR_ARCHITECTURE_AMD64:
            return L"x64"s;
        case PROCESSOR_ARCHITECTURE_IA32_ON_WIN64:
            return L"x86 on win64"s;
        case PROCESSOR_ARCHITECTURE_NEUTRAL:
            return L"neutral"s;
        case PROCESSOR_ARCHITECTURE_ARM64:
            return L"arm64"s;
        case PROCESSOR_ARCHITECTURE_ARM32_ON_WIN64:
            return L"arm32 on win64"s;
        case PROCESSOR_ARCHITECTURE_IA32_ON_ARM64:
            return L"x86 on arm64"s;
        case PROCESSOR_ARCHITECTURE_UNKNOWN:
            return L"unknown"s;

        default:
            return std::format(L"unknown processor type [{}]", stream_hex_dump::to_hex(processor_architecture));
        }
    }

    std::wstring product_type_to_string(unsigned char const product_type)
    {
        switch (product_type)
        {
        case VER_NT_WORKSTATION:
            return L"NT Workstation"s;

        case VER_NT_SERVER:
            return L"NT Server"s;

        case VER_NT_DOMAIN_CONTROLLER:
            return L"NT Domain Controller"s;

        default:
            return std::format(L"unknown product type [{}]", stream_hex_dump::to_hex(product_type));
        }
    }

    std::wstring platform_id_to_string(uint32_t const platform_id)
    {
        switch (platform_id)
        {
        case VER_PLATFORM_WIN32_NT:
            return L"Win32 NT"s;

        case VER_PLATFORM_WIN32s:
            return L"Win32s"s;

        case VER_PLATFORM_WIN32_WINDOWS:
            return L"Win32"s;

        default:
            return std::format(L"unknown platform id [{}]", stream_hex_dump::to_hex(platform_id));
        }
    }


    std::wstring_view windows_version_to_string(uint32_t const major_version, uint32_t const minor_version,
                                                uint32_t const build_number, unsigned char const product_type,
                                                unsigned short const processor_architecture,
                                                unsigned short const suite_mask)
    {
        windows_version const key{major_version, minor_version, build_number, product_type, processor_architecture, suite_mask};
        if (auto const it = version_strings.find(key); it != version_strings.end())
        {
            return it->second;
        }

        return {};
    }

    std::vector<std::wstring_view> suite_mask_to_strings(unsigned short const suite_mask)
    {
        std::vector<std::wstring_view> rv;

        if (suite_mask & VER_SUITE_SMALLBUSINESS)
        {
            rv.emplace_back(L"Small Business Server"sv);
        }

        if (suite_mask & VER_SUITE_ENTERPRISE)
        {
            rv.emplace_back(L"Enterprise Edition / Advanced Server"sv);
        }

        if (suite_mask & VER_SUITE_BACKOFFICE)
        {
            rv.emplace_back(L"BackOffice"sv);
        }

        if (suite_mask & VER_SUITE_COMMUNICATIONS)
        {
            rv.emplace_back(L"Communications"sv);
        }

        if (suite_mask & VER_SUITE_TERMINAL)
        {
            if (suite_mask & VER_SUITE_SINGLEUSERTS)
            {
                rv.emplace_back(L"Terminal Services"sv);
            }
            else
            {
                rv.emplace_back(L"Application Server"sv);
            }
        }
        else if (suite_mask & VER_SUITE_SINGLEUSERTS)
        {
            rv.emplace_back(L"Single User"sv);
        }

        if (suite_mask & VER_SUITE_SMALLBUSINESS_RESTRICTED)
        {
            rv.emplace_back(L"Small Business Server Restricted"sv);
        }

        if (suite_mask & VER_SUITE_EMBEDDEDNT)
        {
            rv.emplace_back(L"Windows XP Embedded"sv);
        }

        if (suite_mask & VER_SUITE_DATACENTER)
        {
            rv.emplace_back(L"Datacenter Edition"sv);
        }

        if (suite_mask & VER_SUITE_PERSONAL)
        {
            rv.emplace_back(L"Windows Vista Home Premium, Windows Vista Home Basic, or Windows XP Home Edition"sv);
        }

        if (suite_mask & VER_SUITE_BLADE)
        {
            rv.emplace_back(L"Web Edition"sv);
        }

        if (suite_mask & VER_SUITE_EMBEDDED_RESTRICTED)
        {
            rv.emplace_back(L"Embedded Restricted"sv);
        }

        if (suite_mask & VER_SUITE_SECURITY_APPLIANCE)
        {
            rv.emplace_back(L"Security Appliance"sv);
        }

        if (suite_mask & VER_SUITE_STORAGE_SERVER)
        {
            rv.emplace_back(L"Storage Server"sv);
        }

        if (suite_mask & VER_SUITE_COMPUTE_SERVER)
        {
            rv.emplace_back(L"Compute Server"sv);
        }

        if (suite_mask & VER_SUITE_WH_SERVER)
        {
            rv.emplace_back(L"Home Server"sv);
        }

        if (suite_mask & VER_SUITE_MULTIUSERTS)
        {
            rv.emplace_back(L"Multi User"sv);
        }

        return rv;
    }

    std::wstring_view process_integrity_level_to_string(uint32_t const process_integrity_level)
    {
        switch (process_integrity_level)
        {
        case SECURITY_MANDATORY_UNTRUSTED_RID:
            return L"untrusted"sv;

        case SECURITY_MANDATORY_LOW_RID:
            return L"low"sv;

        case SECURITY_MANDATORY_MEDIUM_RID:
            return L"normal"sv;

        case SECURITY_MANDATORY_MEDIUM_PLUS_RID:
            return L"above normal"sv;

        case SECURITY_MANDATORY_HIGH_RID:
            return L"elevated"sv;

        case SECURITY_MANDATORY_SYSTEM_RID:
            return L"system"sv;

        case SECURITY_MANDATORY_PROTECTED_PROCESS_RID:
            return L"protected"sv;

        default:
            break;
        }

        return L"unknown"sv;
    }

    std::wstring vm_counters_2_flags_to_string(uint16_t const flags)
    {
        return flags_string_utils::generate_flags_string(flags, vm_counters_2_flag_masks);
    }

    std::vector<std::wstring_view> vm_counters_2_flags_to_strings(uint16_t const flags)
    {
        return flags_string_utils::generate_flags_strings(flags, vm_counters_2_flag_masks);
    }

    std::wstring_view handle_trace_operation_to_string(uint32_t const type)
    {
        switch (static_cast<eHANDLE_TRACE_OPERATIONS>(type))
        {
        case OperationDbUnused:
            return L"Unused"sv;
        case OperationDbOPEN:
            return L"OPEN"sv;
        case OperationDbCLOSE:
            return L"CLOSE"sv;
        case OperationDbBADREF:
            return L"BADREF"sv;
        }
        return L"unknown"sv;
    }

    std::wstring_view handle_object_information_type_to_string(uint32_t type)
    {
        switch (static_cast<MINIDUMP_HANDLE_OBJECT_INFORMATION_TYPE>(type))
        {
        case MiniHandleObjectInformationNone:
            return L"None"sv;
        case MiniThreadInformation1:
            return L"MiniThreadInformation1"sv;
        case MiniMutantInformation1:
            return L"MiniMutantInformation1"sv;
        case MiniMutantInformation2:
            return L"MiniMutantInformation2"sv;
        case MiniProcessInformation1:
            return L"MiniProcessInformation1"sv;
        case MiniProcessInformation2:
            return L"MiniProcessInformation2"sv;
        case MiniEventInformation1:
            return L"MiniEventInformation1"sv;
        case MiniSectionInformation1:
            return L"MiniSectionInformation1"sv;
        case MiniSemaphoreInformation1:
            return L"MiniSemaphoreInformation1"sv;
        case MiniHandleObjectInformationTypeMax:
            return L"MiniHandleObjectInformationTypeMax"sv;
        }
        return L"unknown"sv;
    }

    std::wstring version_info_to_string(uint32_t const version)
    {
        return std::format(L"{0}.{1}", HIWORD(version), LOWORD(version));
    }

    std::wstring version_info_to_string(uint32_t const version_ms, uint32_t const version_ls)
    {
        return std::format(L"{0}.{1}.{2}.{3}", HIWORD(version_ms), LOWORD(version_ms), HIWORD(version_ls), LOWORD(version_ls));
    }

    std::vector<std::wstring_view> process_execute_flags_to_strings(uint32_t const process_execute_flags)
    {
        std::vector<std::wstring_view> rv;

        if(process_execute_flags & MEM_EXECUTE_OPTION_DISABLE)
        {
            rv.emplace_back(L"MEM_EXECUTE_OPTION_DISABLE"sv);
        }

        if(process_execute_flags & MEM_EXECUTE_OPTION_ENABLE)
        {
            rv.emplace_back(L"MEM_EXECUTE_OPTION_ENABLE"sv);
        }

        if(process_execute_flags & MEM_EXECUTE_OPTION_DISABLE_THUNK_EMULATION)
        {
            rv.emplace_back(L"MEM_EXECUTE_OPTION_DISABLE_THUNK_EMULATION"sv);
        }

        if(process_execute_flags & MEM_EXECUTE_OPTION_PERMANENT)
        {
            rv.emplace_back(L"MEM_EXECUTE_OPTION_PERMANENT"sv);
        }

        if(process_execute_flags & MEM_EXECUTE_OPTION_EXECUTE_DISPATCH_ENABLE)
        {
            rv.emplace_back(L"MEM_EXECUTE_OPTION_EXECUTE_DISPATCH_ENABLE"sv);
        }

        if(process_execute_flags & MEM_EXECUTE_OPTION_IMAGE_DISPATCH_ENABLE)
        {
            rv.emplace_back(L"MEM_EXECUTE_OPTION_IMAGE_DISPATCH_ENABLE"sv);
        }

        return rv;
    }

    std::wstring version_file_flags_to_string(uint32_t const flags, uint32_t const mask)
    {
        return flags_string_utils::generate_flags_string(flags & mask, version_file_flag_masks);
    }

    std::vector<std::wstring_view> version_file_flags_to_strings(uint32_t const flags, uint32_t const mask)
    {
        return flags_string_utils::generate_flags_strings(flags & mask, version_file_flag_masks);
    }

    std::wstring version_file_os_to_string(uint32_t const file_os)
    {
        return flags_string_utils::generate_flags_string(file_os, version_file_os_masks);
    }

    std::vector<std::wstring_view> version_file_os_to_strings(uint32_t const file_os)
    {
        return flags_string_utils::generate_flags_strings(file_os, version_file_os_masks);
    }

    std::wstring_view version_file_type_to_string(uint32_t const file_type, uint32_t const file_sub_type)
    {
        switch(file_type)
        {
        case VFT_APP:
            return L"Application"sv;

        case VFT_DLL:
            return L"DLL"sv;

        case VFT_DRV:
            switch(file_sub_type)
            {
            case VFT2_DRV_COMM:
                return L"Communications Driver"sv;

            case VFT2_DRV_DISPLAY:
                return L"Display Driver"sv;

            case VFT2_DRV_INSTALLABLE:
                return L"Installable Driver"sv;

            case VFT2_DRV_KEYBOARD:
                return L"Keyboard Driver"sv;

            case VFT2_DRV_LANGUAGE:
                return L"Language Driver"sv;

            case VFT2_DRV_MOUSE:
                return L"Mouse Driver"sv;

            case VFT2_DRV_NETWORK:
                return L"Network Driver"sv;

            case VFT2_DRV_PRINTER:
                return L"Printer Driver"sv;

            case VFT2_DRV_SOUND:
                return L"Sound Driver"sv;

            case VFT2_DRV_SYSTEM:
                return L"System Driver"sv;

            case VFT2_DRV_VERSIONED_PRINTER:
                return L"Versioned Printer Driver"sv;

            case VFT2_DRV_INPUTMETHOD:
                return L"Input Method Driver"sv;

            case VFT2_UNKNOWN:
                return L"Unknown Driver"sv;

            default:
                return L"Driver"sv;
            }

        case VFT_FONT:
            switch(file_sub_type)
            {
            case VFT2_FONT_RASTER:
                return L"Raster Font"sv;

            case VFT2_FONT_TRUETYPE:
                return L"Truetype Font"sv;

            case VFT2_FONT_VECTOR:
                return L"Vector Font"sv;

            case VFT2_UNKNOWN:
                return L"Unknown Font"sv;

            default:
                return L"Font"sv;
            }

        case VFT_STATIC_LIB:
            return L"Static-Link Library"sv;

        case VFT_UNKNOWN:
            return L"Unknown"sv;

        case VFT_VXD:
            return L"Virtual Device"sv;

        default:
            return L"invalid"sv;
        }
    }

    std::wstring sym_type_to_string(uint32_t const type)
    {
        switch(static_cast<SYM_TYPE>(type))
        {
        case SymNone:
            return L"SymNone"s;
        case SymCoff:
            return L"SymCoff"s;
        case SymCv:
            return L"SymCv"s;
        case SymPdb:
            return L"SymPdb"s;
        case SymExport:
            return L"SymExport"s;
        case SymDeferred:
            return L"SymDeferred"s;
        case SymSym:
            return L"SymSym"s;
        case SymDia:
            return L"SymDia"s;
        case SymVirtual:
            return L"SymVirtual"s;
        case NumSymTypes:
            return L"NumSymTypes"s;
        default:  // NOLINT(clang-diagnostic-covered-switch-default)
            return std::format(L"Unknown:{}", static_cast<int>(type));
        }
    }

    std::filesystem::path const& get_running_application_path()
    {
        static auto running_application_path{init_running_application_path()};
        return running_application_path;
    }

    std::filesystem::path const& get_run_from_directory()
    {
        static auto run_from_directory{get_running_application_path().parent_path()};
        return run_from_directory;
    }

    std::filesystem::path const& get_temp_directory()
    {
        static auto temp_directory{init_temp_directory()};
        return temp_directory;
    }
}
