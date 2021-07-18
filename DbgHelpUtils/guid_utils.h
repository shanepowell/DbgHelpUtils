#pragma once
#include "windows_setup.h"
#include <string>

namespace dlg_help_utils::guid_utils
{
    std::wstring to_string(GUID const& guid);
}
