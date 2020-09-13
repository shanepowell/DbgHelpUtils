#include "exception_stream.h"

#include "mini_dump.h"

namespace dlg_help_utils
{
    exception_stream::exception_stream(mini_dump const& dump, size_t const index)
    {
        index_ = index;
        auto const* entry = dump.find_stream_type(ExceptionStream, index_);
        if (entry == nullptr)
        {
            return;
        }

        exception_ = static_cast<MINIDUMP_EXCEPTION_STREAM const*>(dump.rva32(entry->Location));
        found_ = true;

        thread_context_ = stream_thread_context{dump, exception_->ThreadContext};
    }
}
