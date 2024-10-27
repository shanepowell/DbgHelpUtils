#include "pch.h"
#include "DbgHelpUtils/memory_info_utils.h"

#include <winrt/Microsoft.Windows.ApplicationModel.Resources.h>

#include "DbgHelpUtils/windows_setup.h"
#include <DbgHelp.h>

#include "DbgHelpUtils/flags_string_utils.h"

using namespace std::string_literals;
using namespace std::string_view_literals;

namespace
{
    std::unordered_map<uint32_t, std::wstring> const& memory_protection_option_masks()
    {
        winrt::Microsoft::Windows::ApplicationModel::Resources::ResourceManager const rm{};
        static std::unordered_map<uint32_t, std::wstring> const memory_protection_option_masks =
        {
            {PAGE_NOACCESS, std::wstring{rm.MainResourceMap().GetValue(L"Resources/PAGE_NOACCESS").ValueAsString()}},
            {PAGE_READONLY, std::wstring{rm.MainResourceMap().GetValue(L"Resources/PAGE_READONLY").ValueAsString()}},
            {PAGE_READWRITE, std::wstring{rm.MainResourceMap().GetValue(L"Resources/PAGE_READWRITE").ValueAsString()}},
            {PAGE_WRITECOPY, std::wstring{rm.MainResourceMap().GetValue(L"Resources/PAGE_WRITECOPY").ValueAsString()}},
            {PAGE_EXECUTE, std::wstring{rm.MainResourceMap().GetValue(L"Resources/PAGE_EXECUTE").ValueAsString()}},
            {PAGE_EXECUTE_READ, std::wstring{rm.MainResourceMap().GetValue(L"Resources/PAGE_EXECUTE_READ").ValueAsString()}},
            {PAGE_EXECUTE_READWRITE, std::wstring{rm.MainResourceMap().GetValue(L"Resources/PAGE_EXECUTE_READWRITE").ValueAsString()}},
            {PAGE_EXECUTE_WRITECOPY, std::wstring{rm.MainResourceMap().GetValue(L"Resources/PAGE_EXECUTE_WRITECOPY").ValueAsString()}},
            {PAGE_GUARD, std::wstring{rm.MainResourceMap().GetValue(L"Resources/PAGE_GUARD").ValueAsString()}},
            {PAGE_NOCACHE, std::wstring{rm.MainResourceMap().GetValue(L"Resources/PAGE_NOCACHE").ValueAsString()}},
            {PAGE_WRITECOMBINE, std::wstring{rm.MainResourceMap().GetValue(L"Resources/PAGE_WRITECOMBINE").ValueAsString()}},
            {PAGE_GRAPHICS_NOACCESS, std::wstring{rm.MainResourceMap().GetValue(L"Resources/PAGE_GRAPHICS_NOACCESS").ValueAsString()}},
            {PAGE_GRAPHICS_READONLY, std::wstring{rm.MainResourceMap().GetValue(L"Resources/PAGE_GRAPHICS_READONLY").ValueAsString()}},
            {PAGE_GRAPHICS_READWRITE, std::wstring{rm.MainResourceMap().GetValue(L"Resources/PAGE_GRAPHICS_READWRITE").ValueAsString()}},
            {PAGE_GRAPHICS_EXECUTE, std::wstring{rm.MainResourceMap().GetValue(L"Resources/PAGE_GRAPHICS_EXECUTE").ValueAsString()}},
            {PAGE_GRAPHICS_EXECUTE_READ, std::wstring{rm.MainResourceMap().GetValue(L"Resources/PAGE_GRAPHICS_EXECUTE_READ").ValueAsString()}},
            {PAGE_GRAPHICS_EXECUTE_READWRITE, std::wstring{rm.MainResourceMap().GetValue(L"Resources/PAGE_GRAPHICS_EXECUTE_READWRITE").ValueAsString()}},
            {PAGE_GRAPHICS_COHERENT, std::wstring{rm.MainResourceMap().GetValue(L"Resources/PAGE_GRAPHICS_COHERENT").ValueAsString()}},
            {PAGE_ENCLAVE_THREAD_CONTROL, std::wstring{rm.MainResourceMap().GetValue(L"Resources/PAGE_ENCLAVE_THREAD_CONTROL").ValueAsString()}},
            {PAGE_REVERT_TO_FILE_MAP, std::wstring{rm.MainResourceMap().GetValue(L"Resources/PAGE_REVERT_TO_FILE_MAP").ValueAsString()}},
            {PAGE_TARGETS_NO_UPDATE, std::wstring{rm.MainResourceMap().GetValue(L"Resources/PAGE_TARGETS_NO_UPDATE").ValueAsString()}},
            {PAGE_TARGETS_INVALID, std::wstring{rm.MainResourceMap().GetValue(L"Resources/PAGE_TARGETS_INVALID").ValueAsString()}},
            {PAGE_ENCLAVE_UNVALIDATED, std::wstring{rm.MainResourceMap().GetValue(L"Resources/PAGE_ENCLAVE_UNVALIDATED").ValueAsString()}},
            {PAGE_ENCLAVE_DECOMMIT, std::wstring{rm.MainResourceMap().GetValue(L"Resources/PAGE_ENCLAVE_DECOMMIT").ValueAsString()}},
        };

        return memory_protection_option_masks;
    }

    std::unordered_map<uint16_t, std::wstring> const& system_memory_info_flag_masks()
    {
        winrt::Microsoft::Windows::ApplicationModel::Resources::ResourceManager const rm{};
        static std::unordered_map<uint16_t, std::wstring> const system_memory_info_flag_masks =
        {
            {static_cast<uint16_t>(MINIDUMP_SYSMEMINFO1_FILECACHE_TRANSITIONREPURPOSECOUNT_FLAGS), std::wstring{rm.MainResourceMap().GetValue(L"Resources/MINIDUMP_SYSMEMINFO1_FILECACHE_TRANSITIONREPURPOSECOUNT_FLAGS").ValueAsString()}},
            {static_cast<uint16_t>(MINIDUMP_SYSMEMINFO1_BASICPERF), std::wstring{rm.MainResourceMap().GetValue(L"Resources/MINIDUMP_SYSMEMINFO1_BASICPERF").ValueAsString()}},
            {static_cast<uint16_t>(MINIDUMP_SYSMEMINFO1_PERF_CCTOTALDIRTYPAGES_CCDIRTYPAGETHRESHOLD), std::wstring{rm.MainResourceMap().GetValue(L"Resources/MINIDUMP_SYSMEMINFO1_PERF_CCTOTALDIRTYPAGES_CCDIRTYPAGETHRESHOLD").ValueAsString()}},
            {static_cast<uint16_t>(MINIDUMP_SYSMEMINFO1_PERF_RESIDENTAVAILABLEPAGES_SHAREDCOMMITPAGES), std::wstring{rm.MainResourceMap().GetValue(L"Resources/MINIDUMP_SYSMEMINFO1_PERF_RESIDENTAVAILABLEPAGES_SHAREDCOMMITPAGES").ValueAsString()}},
        };

        return system_memory_info_flag_masks;
    }

    std::unordered_map<uint32_t, std::wstring> const& memory_state_enum()
    {
        winrt::Microsoft::Windows::ApplicationModel::Resources::ResourceManager const rm{};
        static std::unordered_map<uint32_t, std::wstring> const memory_state_enum =
        {
            {MEM_COMMIT, std::wstring{rm.MainResourceMap().GetValue(L"Resources/MEM_COMMIT").ValueAsString()}},
            {MEM_FREE, std::wstring{rm.MainResourceMap().GetValue(L"Resources/MEM_FREE").ValueAsString()}},
            {MEM_RESERVE, std::wstring{rm.MainResourceMap().GetValue(L"Resources/MEM_RESERVE").ValueAsString()}},
            {0, std::wstring{rm.MainResourceMap().GetValue(L"Resources/FlagsNone").ValueAsString()}},
        };

        return memory_state_enum;
    }

    std::unordered_map<uint32_t, std::wstring> const& memory_type_enum()
    {
        winrt::Microsoft::Windows::ApplicationModel::Resources::ResourceManager const rm{};
        static std::unordered_map<uint32_t, std::wstring> const memory_type_enum =
        {
            {MEM_IMAGE, std::wstring{rm.MainResourceMap().GetValue(L"Resources/MEM_IMAGE").ValueAsString()}},
            {MEM_MAPPED, std::wstring{rm.MainResourceMap().GetValue(L"Resources/MEM_MAPPED").ValueAsString()}},
            {MEM_PRIVATE, std::wstring{rm.MainResourceMap().GetValue(L"Resources/MEM_PRIVATE").ValueAsString()}},
            {0, std::wstring{rm.MainResourceMap().GetValue(L"Resources/FlagsNone").ValueAsString()}},
        };

        return memory_type_enum;
    }
}

namespace dlg_help_utils::memory_info_utils
{
    std::wstring memory_protection_options_to_string(uint32_t const memory_protection_options)
    {
        return generate_flags_string(memory_protection_options, memory_protection_option_masks(), flags_string_utils::mask_used_flags_t{true});
    }

    std::vector<std::wstring> memory_protection_options_to_strings(uint32_t const memory_protection_options)  // NOLINT(misc-use-internal-linkage)
    {
        return generate_flags_strings(memory_protection_options, memory_protection_option_masks(), flags_string_utils::mask_used_flags_t{true});
    }

    std::wstring memory_state_to_string(uint32_t const state)
    {
        return flags_string_utils::generate_enum_string(state, memory_state_enum());
    }

    std::wstring memory_type_to_string(uint32_t const type)
    {
        return flags_string_utils::generate_enum_string(type, memory_type_enum());
    }

    std::wstring system_memory_info_flags_to_string(uint16_t const flags)
    {
        return generate_flags_string(flags, system_memory_info_flag_masks(), flags_string_utils::mask_used_flags_t{true});
    }

    std::vector<std::wstring> system_memory_info_flags_to_strings(uint16_t const flags)  // NOLINT(misc-use-internal-linkage)
    {
        return generate_flags_strings(flags, system_memory_info_flag_masks(), flags_string_utils::mask_used_flags_t{true});
    }
}
