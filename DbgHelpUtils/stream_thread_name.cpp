#include "stream_thread_name.h"

#include "mini_dump.h"
#include "string_stream.h"

namespace dlg_help_utils
{
    stream_thread_name::stream_thread_name(mini_dump const& dump, MINIDUMP_THREAD_NAME const& thread)
        : is_valid_{true}
          , thread_{&thread}
    {
        name_ = string_stream::to_string(dump, thread_->RvaOfThreadName);
    }
}
