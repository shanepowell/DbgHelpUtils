#pragma once
#include "tagged_bool.h"

namespace dlg_help_utils
{
    using throw_on_error_t = tagged_bool<struct throw_on_error_type>;
}
