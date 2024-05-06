#include "stream_token_info.h"

#include <cstdint>

namespace dlg_help_utils
{
    stream_token_info::stream_token_info(MINIDUMP_TOKEN_INFO_HEADER const& header)
        : header_(&header)
    {
        if (header_->TokenSize > 0)
        {
            data_ = reinterpret_cast<uint8_t const*>(header_) + sizeof(MINIDUMP_TOKEN_INFO_HEADER);
        }
    }
}
