#pragma once
#include "windows_setup.h"
#include <DbgHelp.h>

#include <array>
#include <memory>

namespace dlg_help_utils::dbg_help
{
    union symbol_info_buffer
    {
        std::array<uint8_t, sizeof(SYMBOL_INFOW) + ((MAX_SYM_NAME - 1) * sizeof(wchar_t))> buffer;
        SYMBOL_INFOW info;

        static std::shared_ptr<symbol_info_buffer> make();
    };
}
