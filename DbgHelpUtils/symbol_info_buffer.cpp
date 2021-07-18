#include "symbol_info_buffer.h"

namespace dlg_help_utils::dbg_help
{
    std::shared_ptr<symbol_info_buffer> symbol_info_buffer::make()
    {
        auto rv = std::make_shared<symbol_info_buffer>();
        memset(rv.get(), 0, rv->buffer.size());
        rv->info.SizeOfStruct = sizeof(SYMBOL_INFOW);
        rv->info.MaxNameLen = MAX_SYM_NAME;
        return rv;
    }
}
