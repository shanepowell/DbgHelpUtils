#include "pch.h"
#include "DbgHelpUtils/thread_info_utils.h"

#include <winrt/Microsoft.Windows.ApplicationModel.Resources.h>

// ReSharper disable once CppUnusedIncludeDirective
#include "DbgHelpUtils/windows_setup.h"
#include <DbgHelp.h>

#include "DbgHelpUtils/flags_string_utils.h"
#include "DbgHelpUtils/stream_hex_dump.h"

using namespace std::string_literals;

namespace
{
    std::unordered_map<uint32_t, std::wstring> const& dump_flag_masks()
    {
        winrt::Microsoft::Windows::ApplicationModel::Resources::ResourceManager const rm{};
        static std::unordered_map<uint32_t, std::wstring> const dump_flag_masks =
        {
            {MINIDUMP_THREAD_INFO_ERROR_THREAD,     std::wstring{rm.MainResourceMap().GetValue(L"Resources/MINIDUMP_THREAD_INFO_ERROR_THREAD").ValueAsString()}},
            {MINIDUMP_THREAD_INFO_WRITING_THREAD,   std::wstring{rm.MainResourceMap().GetValue(L"Resources/MINIDUMP_THREAD_INFO_WRITING_THREAD").ValueAsString()}},
            {MINIDUMP_THREAD_INFO_EXITED_THREAD,    std::wstring{rm.MainResourceMap().GetValue(L"Resources/MINIDUMP_THREAD_INFO_EXITED_THREAD").ValueAsString()}},
            {MINIDUMP_THREAD_INFO_INVALID_INFO,     std::wstring{rm.MainResourceMap().GetValue(L"Resources/MINIDUMP_THREAD_INFO_INVALID_INFO").ValueAsString()}},
            {MINIDUMP_THREAD_INFO_INVALID_CONTEXT,  std::wstring{rm.MainResourceMap().GetValue(L"Resources/MINIDUMP_THREAD_INFO_INVALID_CONTEXT").ValueAsString()}},
            {MINIDUMP_THREAD_INFO_INVALID_TEB,      std::wstring{rm.MainResourceMap().GetValue(L"Resources/MINIDUMP_THREAD_INFO_INVALID_TEB").ValueAsString()}},
        };

        return dump_flag_masks;
    }
}

namespace dlg_help_utils::thread_info_utils
{
    std::wstring dump_flags_to_string(uint32_t const dump_flags)
    {
        return flags_string_utils::generate_flags_string(dump_flags, dump_flag_masks());
    }

    std::vector<std::wstring_view> dump_flags_to_strings(uint32_t const dump_flags)
    {
        return flags_string_utils::generate_flags_strings(dump_flags, dump_flag_masks());
    }
}
