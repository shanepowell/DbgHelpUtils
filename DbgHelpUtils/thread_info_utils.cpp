#include "thread_info_utils.h"

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
// ReSharper disable once CppUnusedIncludeDirective
#include <Windows.h>
#include <DbgHelp.h>

#include <map>

#include "flags_string_utils.h"
#include "stream_hex_dump.h"

using namespace std::string_view_literals;

namespace
{
    std::map<uint32_t, std::wstring_view> const dump_flag_masks =
    {
        {MINIDUMP_THREAD_INFO_ERROR_THREAD, L"Error Thread"sv},
        {MINIDUMP_THREAD_INFO_WRITING_THREAD, L"Writing Thread"sv},
        {MINIDUMP_THREAD_INFO_EXITED_THREAD, L"Exited Thread"sv},
        {MINIDUMP_THREAD_INFO_INVALID_INFO, L"Invalid Info"sv},
        {MINIDUMP_THREAD_INFO_INVALID_CONTEXT, L"Invalid Context"sv},
        {MINIDUMP_THREAD_INFO_INVALID_TEB, L"Invalid TEB"sv},
    };
}

namespace dlg_help_utils::thread_info_utils
{
    std::wstring dump_flags_to_string(uint32_t const dump_flags)
    {
        return flags_string_utils::generate_flags_string(dump_flags, dump_flag_masks);
    }
}
