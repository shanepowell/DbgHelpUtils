#pragma once

namespace dlg_help_utils
{
    enum class function_table_entry_type : uint8_t
    {
        unknown,
        fpo_data,
        image_function_entry,
        image_function_entry_64,
    };
}
