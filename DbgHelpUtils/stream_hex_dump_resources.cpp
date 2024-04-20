#include "stream_hex_dump.h"

using namespace std::string_literals;

namespace dlg_help_utils::stream_hex_dump::details::resources
{
    std::wstring get_hex_number_prefix()
    {
        return L"0x"s;
    }
}
