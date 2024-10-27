#include "memory_info_utils.h"

#include "windows_setup.h"
#include <DbgHelp.h>

#include "flags_string_utils.h"

using namespace std::string_literals;
using namespace std::string_view_literals;

namespace
{
    std::unordered_map<uint32_t, std::wstring> const memory_protection_option_masks =
    {
        {PAGE_NOACCESS, L"No Access"s},
        {PAGE_READONLY, L"Read Only"s},
        {PAGE_READWRITE, L"Read Write"s},
        {PAGE_WRITECOPY, L"Write Copy"s},
        {PAGE_EXECUTE, L"Execute"s},
        {PAGE_EXECUTE_READ, L"Execute Read"s},
        {PAGE_EXECUTE_READWRITE, L"Execute Read Write"s},
        {PAGE_EXECUTE_WRITECOPY, L"Execute Read Copy"s},
        {PAGE_GUARD, L"Guard"s},
        {PAGE_NOCACHE, L"No Cache"s},
        {PAGE_WRITECOMBINE, L"Write Combine"s},
        {PAGE_GRAPHICS_NOACCESS, L"Graphics No Access"s},
        {PAGE_GRAPHICS_READONLY, L"Graphics Read Only"s},
        {PAGE_GRAPHICS_READWRITE, L"Graphics Read Write"s},
        {PAGE_GRAPHICS_EXECUTE, L"Graphics Execute"s},
        {PAGE_GRAPHICS_EXECUTE_READ, L"Graphics Execute Read"s},
        {PAGE_GRAPHICS_EXECUTE_READWRITE, L"Graphics Execute Read Write"s},
        {PAGE_GRAPHICS_COHERENT, L"Graphics Noherent"s},
        {PAGE_ENCLAVE_THREAD_CONTROL, L"Enclave Thread Control"s},
        {PAGE_REVERT_TO_FILE_MAP, L"Revert to File Map"s},
        {PAGE_TARGETS_NO_UPDATE, L"Targets No Update"s},
        {PAGE_TARGETS_INVALID, L"Targets Invalid"s},
        {PAGE_ENCLAVE_UNVALIDATED, L"Enclave Unvalidated"s},
        {PAGE_ENCLAVE_DECOMMIT, L"Enclave Decommit"s},
    };

    std::unordered_map<uint16_t, std::wstring> const system_memory_info_flag_masks =
    {
        {static_cast<uint16_t>(MINIDUMP_SYSMEMINFO1_FILECACHE_TRANSITIONREPURPOSECOUNT_FLAGS), L"FileCache TransitionRePurposeCount"s},
        {static_cast<uint16_t>(MINIDUMP_SYSMEMINFO1_BASICPERF), L"BasicPref"s},
        {static_cast<uint16_t>(MINIDUMP_SYSMEMINFO1_PERF_CCTOTALDIRTYPAGES_CCDIRTYPAGETHRESHOLD), L"Pref CcTotalDirtyPages/CcDirtyPageThreshold"s},
        {static_cast<uint16_t>(MINIDUMP_SYSMEMINFO1_PERF_RESIDENTAVAILABLEPAGES_SHAREDCOMMITPAGES), L"Pref ResidentAvailablePages/SharedCommittedPages"s},
    };

    std::unordered_map<uint32_t, std::wstring> const memory_state_enum =
    {
        {MEM_COMMIT, L"Commit"s},
        {MEM_FREE, L"Free"s},
        {MEM_RESERVE, L"Reserve"s},
        {0, L"none"s},
    };

    std::unordered_map<uint32_t, std::wstring> const memory_type_enum =
    {
        {MEM_IMAGE, L"Image"s},
        {MEM_MAPPED, L"Mapped"s},
        {MEM_PRIVATE, L"Private"s},
        {0, L"none"s},
    };
}

namespace dlg_help_utils::memory_info_utils
{
    std::wstring memory_protection_options_to_string(uint32_t const memory_protection_options)
    {
        return generate_flags_string(memory_protection_options, memory_protection_option_masks, flags_string_utils::mask_used_flags_t{true});
    }

    std::vector<std::wstring> memory_protection_options_to_strings(uint32_t const memory_protection_options)
    {
        return generate_flags_strings(memory_protection_options, memory_protection_option_masks, flags_string_utils::mask_used_flags_t{true});
    }

    std::wstring memory_state_to_string(uint32_t const state)
    {
        return flags_string_utils::generate_enum_string(state, memory_state_enum);
    }

    std::wstring memory_type_to_string(uint32_t const type)
    {
        return flags_string_utils::generate_enum_string(type, memory_type_enum);
    }

    std::wstring system_memory_info_flags_to_string(uint16_t const flags)
    {
        return generate_flags_string(flags, system_memory_info_flag_masks, flags_string_utils::mask_used_flags_t{true});
    }

    std::vector<std::wstring> system_memory_info_flags_to_strings(uint16_t const flags)
    {
        return generate_flags_strings(flags, system_memory_info_flag_masks, flags_string_utils::mask_used_flags_t{true});
    }
}
