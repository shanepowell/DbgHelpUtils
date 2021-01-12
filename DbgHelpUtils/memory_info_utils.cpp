#include "memory_info_utils.h"

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>
#include <DbgHelp.h>

#include <map>

#include "flags_string_utils.h"
#include "stream_hex_dump.h"

using namespace std::string_view_literals;

namespace
{
    std::map<uint32_t, std::wstring_view> const memory_protection_option_masks =
    {
        {PAGE_NOACCESS, L"No Access"sv},
        {PAGE_READONLY, L"Read Only"sv},
        {PAGE_READWRITE, L"Read Write"sv},
        {PAGE_WRITECOPY, L"Write Copy"sv},
        {PAGE_EXECUTE, L"Execute"sv},
        {PAGE_EXECUTE_READ, L"Execute Read"sv},
        {PAGE_EXECUTE_READWRITE, L"Execute Read Write"sv},
        {PAGE_EXECUTE_WRITECOPY, L"Execute Read Copy"sv},
        {PAGE_GUARD, L"Guard"sv},
        {PAGE_NOCACHE, L"No Cache"sv},
        {PAGE_WRITECOMBINE, L"Write Combine"sv},
        {PAGE_GRAPHICS_NOACCESS, L"Graphics No Access"sv},
        {PAGE_GRAPHICS_READONLY, L"Graphics Read Only"sv},
        {PAGE_GRAPHICS_READWRITE, L"Graphics Read Write"sv},
        {PAGE_GRAPHICS_EXECUTE, L"Graphics Execute"sv},
        {PAGE_GRAPHICS_EXECUTE_READ, L"Graphics Execute Read"sv},
        {PAGE_GRAPHICS_EXECUTE_READWRITE, L"Graphics Execute Read Write"sv},
        {PAGE_GRAPHICS_COHERENT, L"Graphics Noherent"sv},
        {PAGE_ENCLAVE_THREAD_CONTROL, L"Enclave Thread Control"sv},
        {PAGE_REVERT_TO_FILE_MAP, L"Revert to File Map"sv},
        {PAGE_TARGETS_NO_UPDATE, L"Targets No Update"sv},
        {PAGE_TARGETS_INVALID, L"Targets Invalid"sv},
        {PAGE_ENCLAVE_UNVALIDATED, L"Enclave Unvalidated"sv},
        {PAGE_ENCLAVE_DECOMMIT, L"Enclave Decommit"sv},
    };

    std::map<uint16_t, std::wstring_view> const system_memory_info_flag_masks =
    {
        {
            static_cast<uint16_t>(MINIDUMP_SYSMEMINFO1_FILECACHE_TRANSITIONREPURPOSECOUNT_FLAGS),
            L"FileCache TransitionRePurposeCount"sv
        },
        {static_cast<uint16_t>(MINIDUMP_SYSMEMINFO1_BASICPERF), L"BasicPref"sv},
        {
            static_cast<uint16_t>(MINIDUMP_SYSMEMINFO1_PERF_CCTOTALDIRTYPAGES_CCDIRTYPAGETHRESHOLD),
            L"Pref CcTotalDirtyPages/CcDirtyPageThreshold"sv
        },
        {
            static_cast<uint16_t>(MINIDUMP_SYSMEMINFO1_PERF_RESIDENTAVAILABLEPAGES_SHAREDCOMMITPAGES),
            L"Pref ResidentAvailablePages/SharedCommittedPages"sv
        },
    };
}

namespace dlg_help_utils::memory_info_utils
{
    std::wstring memory_protection_options_to_string(uint32_t const memory_protection_options)
    {
        return flags_string_utils::generate_flags_string(memory_protection_options, memory_protection_option_masks);
    }

    std::vector<std::wstring_view> memory_protection_options_to_strings(uint32_t const memory_protection_options)
    {
        return flags_string_utils::generate_flags_strings(memory_protection_options, memory_protection_option_masks);
    }

    std::wstring_view memory_state_to_string(uint32_t const state)
    {
        switch (state)
        {
        case MEM_COMMIT:
            return L"Commit"sv;

        case MEM_FREE:
            return L"Free"sv;

        case MEM_RESERVE:
            return L"Reserve"sv;

        case 0:
            return L"none"sv;

        default:
            break;
        }

        return L"Unknown";
    }

    std::wstring_view memory_type_to_string(uint32_t const type)
    {
        switch (type)
        {
        case MEM_IMAGE:
            return L"Image"sv;

        case MEM_MAPPED:
            return L"Mapped"sv;

        case MEM_PRIVATE:
            return L"Private"sv;

        case 0:
            return L"none"sv;

        default:
            break;
        }

        return L"Unknown";
    }

    std::wstring system_memory_info_flags_to_string(uint16_t const flags)
    {
        return flags_string_utils::generate_flags_string(flags, system_memory_info_flag_masks);
    }

    std::vector<std::wstring_view> system_memory_info_flags_to_strings(uint16_t const flags)
    {
        return flags_string_utils::generate_flags_strings(flags, system_memory_info_flag_masks);
    }
}
