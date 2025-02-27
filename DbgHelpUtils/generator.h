#pragma once

#if _MSC_VER >= 1935

#include <generator>

namespace dlg_help_utils
{
    using std::generator;
}

#else

#include <experimental/generator>

namespace dlg_help_utils
{
    using std::experimental::generator;
}

#endif