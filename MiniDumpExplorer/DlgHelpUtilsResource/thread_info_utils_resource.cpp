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


    std::unordered_map<uint32_t, std::wstring> const& thread_priority_classes()
    {
        winrt::Microsoft::Windows::ApplicationModel::Resources::ResourceManager const rm{};
        static std::unordered_map<uint32_t, std::wstring> const thread_priority_classes =
        {
            {ABOVE_NORMAL_PRIORITY_CLASS, std::wstring{rm.MainResourceMap().GetValue(L"Resources/ABOVE_NORMAL_PRIORITY_CLASS").ValueAsString()}},
            {BELOW_NORMAL_PRIORITY_CLASS, std::wstring{rm.MainResourceMap().GetValue(L"Resources/BELOW_NORMAL_PRIORITY_CLASS").ValueAsString()}},
            {HIGH_PRIORITY_CLASS, std::wstring{rm.MainResourceMap().GetValue(L"Resources/HIGH_PRIORITY_CLASS").ValueAsString()}},
            {IDLE_PRIORITY_CLASS, std::wstring{rm.MainResourceMap().GetValue(L"Resources/IDLE_PRIORITY_CLASS").ValueAsString()}},
            {NORMAL_PRIORITY_CLASS, std::wstring{rm.MainResourceMap().GetValue(L"Resources/NORMAL_PRIORITY_CLASS").ValueAsString()}},
            {PROCESS_MODE_BACKGROUND_END, std::wstring{rm.MainResourceMap().GetValue(L"Resources/PROCESS_MODE_BACKGROUND_END").ValueAsString()}},
            {REALTIME_PRIORITY_CLASS, std::wstring{rm.MainResourceMap().GetValue(L"Resources/REALTIME_PRIORITY_CLASS").ValueAsString()}},
        };

        return thread_priority_classes;
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

    std::wstring thread_priority_class_to_string(uint32_t const thread_priority_class)
    {
        return flags_string_utils::generate_enum_string(thread_priority_class, thread_priority_classes());
    }
}
