#pragma once

namespace dlg_help_utils::dbg_help
{
    enum class udt_kind_type : uint8_t
    {
        struct_type = 0,
        class_type = 1,
        union_type = 2,
        interface_type = 3
    };
}