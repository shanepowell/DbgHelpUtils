#pragma once
#include <cstdint>

namespace dlg_help_utils::symbol_type_utils
{
    namespace symbol_visit_flags
    {
        enum flags : uint8_t
        {
            none = 0x0,
            detect_pointer_cycles = 0x1,
            no_header = 0x2
        };
    };
}
