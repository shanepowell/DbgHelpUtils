#include "thread_info_utils.h"

// ReSharper disable once CppUnusedIncludeDirective
#include "windows_setup.h"
#include <DbgHelp.h>

#include "flags_string_utils.h"
#include "stream_hex_dump.h"

using namespace std::string_literals;

namespace
{
    std::unordered_map<uint32_t, std::wstring> const dump_flag_masks =
    {
        {MINIDUMP_THREAD_INFO_ERROR_THREAD, L"Error Thread"s},
        {MINIDUMP_THREAD_INFO_WRITING_THREAD, L"Writing Thread"s},
        {MINIDUMP_THREAD_INFO_EXITED_THREAD, L"Exited Thread"s},
        {MINIDUMP_THREAD_INFO_INVALID_INFO, L"Invalid Info"s},
        {MINIDUMP_THREAD_INFO_INVALID_CONTEXT, L"Invalid Context"s},
        {MINIDUMP_THREAD_INFO_INVALID_TEB, L"Invalid TEB"s},
    };
}

namespace dlg_help_utils::thread_info_utils
{
    std::wstring dump_flags_to_string(uint32_t const dump_flags)
    {
        return flags_string_utils::generate_flags_string(dump_flags, dump_flag_masks);
    }

    std::vector<std::wstring_view> dump_flags_to_strings(uint32_t const dump_flags)
    {
        return flags_string_utils::generate_flags_strings(dump_flags, dump_flag_masks);
    }
}
