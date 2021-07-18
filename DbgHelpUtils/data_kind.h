#pragma once
namespace dlg_help_utils::dbg_help
{
    enum class data_kind
    {
        data_is_local,
        data_is_unknown,
        data_is_static_local,
        data_is_param,
        data_is_object_ptr,
        data_is_file_static,
        data_is_global,
        data_is_member,
        data_is_static_member,
        data_is_constant
    };
}