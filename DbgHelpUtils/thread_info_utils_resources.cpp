#include "thread_info_utils.h"

// ReSharper disable once CppUnusedIncludeDirective
#include "windows_setup.h"
#include <DbgHelp.h>

#include "flags_string_utils.h"
#include "stream_hex_dump.h"

using namespace std::string_literals;

namespace
{
    std::unordered_map<uint32_t, std::wstring> const g_dump_flag_masks =
    {
        {MINIDUMP_THREAD_INFO_ERROR_THREAD, L"Error Thread"s},
        {MINIDUMP_THREAD_INFO_WRITING_THREAD, L"Writing Thread"s},
        {MINIDUMP_THREAD_INFO_EXITED_THREAD, L"Exited Thread"s},
        {MINIDUMP_THREAD_INFO_INVALID_INFO, L"Invalid Info"s},
        {MINIDUMP_THREAD_INFO_INVALID_CONTEXT, L"Invalid Context"s},
        {MINIDUMP_THREAD_INFO_INVALID_TEB, L"Invalid TEB"s},
    };

    std::unordered_map<uint32_t, std::wstring> const g_thread_priority_classes =
    {
        {ABOVE_NORMAL_PRIORITY_CLASS, L"ABOVE_NORMAL_PRIORITY_CLASS"s},
        {BELOW_NORMAL_PRIORITY_CLASS, L"BELOW_NORMAL_PRIORITY_CLASS"s},
        {HIGH_PRIORITY_CLASS, L"HIGH_PRIORITY_CLASS"s},
        {IDLE_PRIORITY_CLASS, L"IDLE_PRIORITY_CLASS"s},
        {NORMAL_PRIORITY_CLASS, L"NORMAL_PRIORITY_CLASS"s},
        {PROCESS_MODE_BACKGROUND_END, L"PROCESS_MODE_BACKGROUND_END"s},
        {REALTIME_PRIORITY_CLASS, L"REALTIME_PRIORITY_CLASS"s},
    };
}

namespace dlg_help_utils::thread_info_utils
{
    std::wstring dump_flags_to_string(uint32_t const dump_flags)
    {
        return flags_string_utils::generate_flags_string(dump_flags, g_dump_flag_masks);
    }

    std::vector<std::wstring> dump_flags_to_strings(uint32_t const dump_flags)
    {
        return flags_string_utils::generate_flags_strings(dump_flags, g_dump_flag_masks);
    }

    std::wstring thread_priority_class_to_string(uint32_t const thread_priority_class)
    {
        return flags_string_utils::generate_enum_string(thread_priority_class, g_thread_priority_classes);
    }
}
