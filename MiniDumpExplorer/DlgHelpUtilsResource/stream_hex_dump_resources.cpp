#include "pch.h"
#include "DbgHelpUtils/stream_hex_dump.h"

#include "Utility/GetResourceValue.h"

namespace dlg_help_utils::stream_hex_dump::details::resources
{
    std::wstring get_hex_number_prefix()
    {
        return Utility::GetResourceValue(L"Resources/HexNumberPrefix");
    }
}
