#pragma once

// ReSharper disable once CppUnusedIncludeDirective
#include <string>

// ReSharper disable once CommentTypo
// Found in "$(VSINSTALLDIR)\DIA SDK\include"
#include <cvconst.h>

namespace dlg_help_utils::register_names
{
    std::wstring get_register_name(CV_HREG_e register_type);
}
