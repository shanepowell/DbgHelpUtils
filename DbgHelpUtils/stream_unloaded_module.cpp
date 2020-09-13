#include "stream_unloaded_module.h"

#include "mini_dump.h"
#include "string_stream.h"

namespace dlg_help_utils
{
    stream_unloaded_module::stream_unloaded_module(mini_dump const& dump, MINIDUMP_UNLOADED_MODULE const& module)
        : module_{module}
    {
        name_ = string_stream::to_string(dump, module_.ModuleNameRva);
    }
}
